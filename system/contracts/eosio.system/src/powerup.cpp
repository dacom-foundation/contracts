#include <eosio.system/eosio.system.hpp>
#include <eosio.token/eosio.token.hpp>
#include <eosio/action.hpp>
#include <eosio.system/powerup.results.hpp>
#include <algorithm>
#include <cmath>

namespace eosiosystem {

void update_weight(time_point_sec now, powerup_state_resource& res, int64_t& delta_available);

/**
 *  @pre  now >= res.utilization_timestamp
 *  @post res.utilization <= new res.adjusted_utilization
 *  @post if res.utilization < old res.adjusted_utilization, then new res.adjusted_utilization <= old res.adjusted_utilization
 *  @post if res.utilization >= old res.adjusted_utilization, then new res.adjusted_utilization == res.utilization
 */
void update_utilization(time_point_sec now, powerup_state_resource& res);

void system_contract::adjust_resources(name payer, name account, symbol core_symbol, int64_t net_delta,
                                       int64_t cpu_delta, bool must_not_be_managed) {
   if (!net_delta && !cpu_delta)
      return;

   user_resources_table totals_tbl(get_self(), account.value);
   auto                 tot_itr = totals_tbl.find(account.value);
   if (tot_itr == totals_tbl.end()) {
      tot_itr = totals_tbl.emplace(payer, [&](auto& tot) {
         tot.owner      = account;
         tot.net_weight = asset{ net_delta, core_symbol };
         tot.cpu_weight = asset{ cpu_delta, core_symbol };
      });
   } else {
      totals_tbl.modify(tot_itr, same_payer, [&](auto& tot) {
         tot.net_weight.amount += net_delta;
         tot.cpu_weight.amount += cpu_delta;
      });
   }
   check(0 <= tot_itr->net_weight.amount, "insufficient staked total net bandwidth");
   check(0 <= tot_itr->cpu_weight.amount, "insufficient staked total cpu bandwidth");

   {
      bool ram_managed = false;
      bool net_managed = false;
      bool cpu_managed = false;

      auto voter_itr = _voters.find(account.value);
      if (voter_itr != _voters.end()) {
         ram_managed = has_field(voter_itr->flags1, voter_info::flags1_fields::ram_managed);
         net_managed = has_field(voter_itr->flags1, voter_info::flags1_fields::net_managed);
         cpu_managed = has_field(voter_itr->flags1, voter_info::flags1_fields::cpu_managed);
      }

      if (must_not_be_managed)
         eosio::check(!net_managed && !cpu_managed, "something is managed which shouldn't be");

      if (!(net_managed && cpu_managed)) {
         int64_t ram_bytes, net, cpu;
         get_resource_limits(account, ram_bytes, net, cpu);
         set_resource_limits(
               account, ram_managed ? ram_bytes : std::max(tot_itr->ram_bytes + ram_gift_bytes, ram_bytes),
               net_managed ? net : tot_itr->net_weight.amount, cpu_managed ? cpu : tot_itr->cpu_weight.amount);
      }
   }

   if (tot_itr->is_empty()) {
      totals_tbl.erase(tot_itr);
   }
} // system_contract::adjust_resources

void system_contract::process_powerup_queue(time_point_sec now, symbol core_symbol, powerup_state& state,
                                           powerup_order_table& orders, uint32_t max_items, int64_t& net_delta_available,
                                           int64_t& cpu_delta_available) {
   update_utilization(now, state.net);
   update_utilization(now, state.cpu);
   auto idx = orders.get_index<"byexpires"_n>();
   while (max_items--) {
      auto it = idx.begin();
      if (it == idx.end() || it->expires > now)
         break;
      net_delta_available += it->net_weight;
      cpu_delta_available += it->cpu_weight;
      adjust_resources(get_self(), it->owner, core_symbol, -it->net_weight, -it->cpu_weight);
      // TODO здесь где-то надо изъять RAM
      // sellram()
      idx.erase(it);
   }
   state.net.utilization -= net_delta_available;
   state.cpu.utilization -= cpu_delta_available;
   update_weight(now, state.net, net_delta_available);
   update_weight(now, state.cpu, cpu_delta_available);

}

void update_weight(time_point_sec now, powerup_state_resource& res, int64_t& delta_available) {
   if (now >= res.target_timestamp) {
      res.weight_ratio = res.target_weight_ratio;
   } else {
      res.weight_ratio = res.initial_weight_ratio + //
                         int128_t(res.target_weight_ratio - res.initial_weight_ratio) *
                               (now.utc_seconds - res.initial_timestamp.utc_seconds) /
                               (res.target_timestamp.utc_seconds - res.initial_timestamp.utc_seconds);
   }
   int64_t new_weight    = res.assumed_stake_weight * int128_t(powerup_frac) / res.weight_ratio - res.assumed_stake_weight;
   delta_available += new_weight - res.weight;
   res.weight = new_weight;
}

void update_utilization(time_point_sec now, powerup_state_resource& res) {
   if (now <= res.utilization_timestamp) return;

   if (res.utilization >= res.adjusted_utilization) {
      res.adjusted_utilization = res.utilization;
   } else {
      int64_t diff  = res.adjusted_utilization - res.utilization;
      int64_t delta = diff * std::exp(-double(now.utc_seconds - res.utilization_timestamp.utc_seconds) / double(res.decay_secs));
      delta = std::clamp( delta, 0ll, diff);
      res.adjusted_utilization = res.utilization + delta;
   }
   res.utilization_timestamp = now;
}

void system_contract::cfgpowerup(powerup_config& args) {
   require_auth(get_self());
   time_point_sec         now         = eosio::current_time_point();
   auto                   core_symbol = get_core_symbol();
   powerup_state_singleton state_sing{ get_self(), 0 };
   auto                   state = state_sing.get_or_default();

   eosio::check(eosio::is_account(reserve_account), "eosio.resource account must first be created"); // cspell:disable-line

   int64_t net_delta_available = 0;
   int64_t cpu_delta_available = 0;
   if (state_sing.exists()) {
      update_utilization(now, state.net);
      update_utilization(now, state.cpu);
      update_weight(now, state.net, net_delta_available);
      update_weight(now, state.cpu, cpu_delta_available);
   } else {
      state.net.utilization_timestamp = now;
      state.cpu.utilization_timestamp = now;
   }

   auto is_default_asset = []( const eosio::asset& a ) -> bool {
      return a.amount == 0 && a.symbol == symbol{};
   };

   auto update = [&](auto& state, auto& args) {
      if (!args.current_weight_ratio) {
         if (state.weight_ratio) {
            *args.current_weight_ratio = state.weight_ratio;
         } else {
            *args.current_weight_ratio = state.initial_weight_ratio;
         }
      }

      if (!args.target_weight_ratio) {
         *args.target_weight_ratio = state.target_weight_ratio;
      }

      if (!args.assumed_stake_weight) {
         eosio::check(state.assumed_stake_weight != 0, "assumed_stake_weight does not have a default value");
         *args.assumed_stake_weight = state.assumed_stake_weight;
      }

      if (*args.current_weight_ratio == *args.target_weight_ratio) {
         *args.target_timestamp = now;
      } else {
         if (!args.target_timestamp) {
            eosio::check(state.target_timestamp.utc_seconds != 0, "target_timestamp does not have a default value");
            *args.target_timestamp = state.target_timestamp;
         }
         eosio::check(*args.target_timestamp > now, "target_timestamp must be in the future");
      }

      if (!args.exponent) {
         *args.exponent = state.exponent;
      }

      if (!args.decay_secs) {
         *args.decay_secs = state.decay_secs;
      }

      if (!args.max_price) {
         eosio::check(!is_default_asset(state.max_price), "max_price does not have a default value");
         *args.max_price = state.max_price;
      }

      if (!args.min_price) {
         if (is_default_asset(state.min_price)) {
            *args.min_price = *args.max_price; // just to copy symbol of max_price
            args.min_price->amount = 0;        // min_price has a default of zero.
         } else {
            *args.min_price = state.min_price;
         }
      }

      eosio::check(*args.current_weight_ratio > 0, "current_weight_ratio is too small");
      eosio::check(*args.current_weight_ratio <= powerup_frac, "current_weight_ratio is too large");
      eosio::check(*args.target_weight_ratio > 0, "target_weight_ratio is too small");
      eosio::check(*args.target_weight_ratio <= *args.current_weight_ratio, "weight can't grow over time");
      eosio::check(*args.assumed_stake_weight >= 1,
                   "assumed_stake_weight must be at least 1; a much larger value is recommended");
      eosio::check(*args.assumed_stake_weight * int128_t(powerup_frac) / *args.target_weight_ratio <=
                         std::numeric_limits<int64_t>::max(),
                   "assumed_stake_weight/target_weight_ratio is too large");
      eosio::check(*args.exponent >= 1.0, "exponent must be >= 1");
      eosio::check(*args.decay_secs >= 1, "decay_secs must be >= 1");
      eosio::check(args.max_price->symbol == core_symbol, "max_price doesn't match core symbol");
      eosio::check(args.max_price->amount > 0, "max_price must be positive");
      eosio::check(args.min_price->symbol == core_symbol, "min_price doesn't match core symbol");
      eosio::check(args.min_price->amount >= 0, "min_price must be non-negative");
      eosio::check(args.min_price->amount <= args.max_price->amount, "min_price cannot exceed max_price");
      if (*args.exponent == 1.0) {
         eosio::check(args.min_price->amount == args.max_price->amount, "min_price and max_price must be the same if the exponent is 1");
      }

      state.assumed_stake_weight = *args.assumed_stake_weight;
      state.initial_weight_ratio = *args.current_weight_ratio;
      state.target_weight_ratio  = *args.target_weight_ratio;
      state.initial_timestamp    = now;
      state.target_timestamp     = *args.target_timestamp;
      state.exponent             = *args.exponent;
      state.decay_secs           = *args.decay_secs;
      state.min_price            = *args.min_price;
      state.max_price            = *args.max_price;
   };

   if (!args.powerup_days) {
      *args.powerup_days = state.powerup_days;
   }

   if (!args.min_powerup_fee) {
      eosio::check(!is_default_asset(state.min_powerup_fee), "min_powerup_fee does not have a default value");
      *args.min_powerup_fee = state.min_powerup_fee;
   }

   eosio::check(*args.powerup_days > 0, "powerup_days must be > 0");
   eosio::check(args.min_powerup_fee->symbol == core_symbol, "min_powerup_fee doesn't match core symbol");
   eosio::check(args.min_powerup_fee->amount > 0, "min_powerup_fee must be positive");

   state.powerup_days    = *args.powerup_days;
   state.min_powerup_fee = *args.min_powerup_fee;

   update(state.net, args.net);
   update(state.cpu, args.cpu);

   update_weight(now, state.net, net_delta_available);
   update_weight(now, state.cpu, cpu_delta_available);
   eosio::check(state.net.weight >= state.net.utilization, "weight can't shrink below utilization");
   eosio::check(state.cpu.weight >= state.cpu.utilization, "weight can't shrink below utilization");
   state.net.adjusted_utilization = std::min(state.net.adjusted_utilization, state.net.weight);
   state.cpu.adjusted_utilization = std::min(state.cpu.adjusted_utilization, state.cpu.weight);

   adjust_resources(get_self(), reserve_account, core_symbol, net_delta_available, cpu_delta_available, true);
   state_sing.set(state, get_self());
}

/**
 *  @pre 0 <= state.min_price.amount <= state.max_price.amount
 *  @pre 0 < state.max_price.amount
 *  @pre 1.0 <= state.exponent
 *  @pre 0 <= state.utilization <= state.adjusted_utilization <= state.weight
 *  @pre 0 <= utilization_increase <= (state.weight - state.utilization)
 */
int64_t calc_powerup_fee(const powerup_state_resource& state, int64_t utilization_increase) {
   if( utilization_increase <= 0 ) return 0;

   // Let p(u) = price as a function of the utilization fraction u which is defined for u in [0.0, 1.0].
   // Let f(u) = integral of the price function p(x) from x = 0.0 to x = u, again defined for u in [0.0, 1.0].

   // In particular we choose f(u) = min_price * u + ((max_price - min_price) / exponent) * (u ^ exponent).
   // And so p(u) = min_price + (max_price - min_price) * (u ^ (exponent - 1.0)).

   // Returns f(double(end_utilization)/state.weight) - f(double(start_utilization)/state.weight) which is equivalent to
   // the integral of p(x) from x = double(start_utilization)/state.weight to x = double(end_utilization)/state.weight.
   // @pre 0 <= start_utilization <= end_utilization <= state.weight
   auto price_integral_delta = [&state](int64_t start_utilization, int64_t end_utilization) -> double {
      double coefficient = (state.max_price.amount - state.min_price.amount) / state.exponent;
      double start_u     = double(start_utilization) / state.weight;
      double end_u       = double(end_utilization) / state.weight;
      return state.min_price.amount * end_u - state.min_price.amount * start_u +
               coefficient * std::pow(end_u, state.exponent) - coefficient * std::pow(start_u, state.exponent);
   };

   // Returns p(double(utilization)/state.weight).
   // @pre 0 <= utilization <= state.weight
   auto price_function = [&state](int64_t utilization) -> double {
      double price = state.min_price.amount;
      // state.exponent >= 1.0, therefore the exponent passed into std::pow is >= 0.0.
      // Since the exponent passed into std::pow could be 0.0 and simultaneously so could double(utilization)/state.weight,
      // the safest thing to do is handle that as a special case explicitly rather than relying on std::pow to return 1.0
      // instead of triggering a domain error.
      double new_exponent = state.exponent - 1.0;
      if (new_exponent <= 0.0) { 
         return state.max_price.amount;
      } else {
         price += (state.max_price.amount - state.min_price.amount) * std::pow(double(utilization) / state.weight, new_exponent);
      }

      return price;
   };

   double  fee = 0.0;
   int64_t start_utilization = state.utilization;
   int64_t end_utilization   = start_utilization + utilization_increase;
   
   if (start_utilization < state.adjusted_utilization) { 
      fee += price_function(state.adjusted_utilization) *
               std::min(utilization_increase, state.adjusted_utilization - start_utilization) / state.weight;
      start_utilization = state.adjusted_utilization;
   }

   if (start_utilization < end_utilization) { 
      fee += price_integral_delta(start_utilization, end_utilization);
   }
   return std::ceil(fee);
}

void system_contract::powerupexec(const name& user, uint16_t max) {
   require_auth(user);
   powerup_state_singleton state_sing{ get_self(), 0 };
   powerup_order_table     orders{ get_self(), 0 };
   eosio::check(state_sing.exists(), "powerup hasn't been initialized");
   auto           state       = state_sing.get();
   time_point_sec now         = eosio::current_time_point();
   auto           core_symbol = get_core_symbol();

   int64_t net_delta_available = 0;
   int64_t cpu_delta_available = 0;
   process_powerup_queue(now, core_symbol, state, orders, max, net_delta_available, cpu_delta_available);

   adjust_resources(get_self(), reserve_account, core_symbol, net_delta_available, cpu_delta_available, true);
   state_sing.set(state, get_self());
}

void system_contract::powerup(const name& payer, const name& receiver, uint32_t days, const asset& payment) {
  require_auth(payer);
  eosio::check(payment.amount > 0, "Payment must be positive");
  powerup_state_singleton state_sing{ get_self(), 0 };
  powerup_order_table orders{ get_self(), 0 };
  eosio::check(state_sing.exists(), "powerup hasn't been initialized");
  auto state = state_sing.get();
  time_point_sec now = eosio::current_time_point();
  auto core_symbol = get_core_symbol();
  eosio::check(payment.symbol == core_symbol, "Payment doesn't match core symbol");
  eosio::check(days == state.powerup_days, "Days doesn't match configuration");

  // Divide payment
  eosio::asset net_payment = payment / 4; // 25%
  eosio::asset cpu_payment = payment / 4; // 25%
  eosio::asset ram_payment = payment / 2; // 50%
  eosio::asset weight_payment = cpu_payment + cpu_payment;

  // Buy RAM
  buyram(payer, receiver, ram_payment);

  int64_t net_delta_available = 0;
  int64_t cpu_delta_available = 0;
  process_powerup_queue(now, core_symbol, state, orders, 2, net_delta_available, cpu_delta_available);

  // Process NET and CPU fractions separately based on specified payment portions
  int64_t net_frac = 0;
  int64_t cpu_frac = 0;

  if (state.net.weight > 0)
    net_frac = int128_t(net_payment.amount) * powerup_frac / state.net.weight;
  if (state.cpu.weight > 0)
    cpu_frac = int128_t(cpu_payment.amount) * powerup_frac / state.cpu.weight;

  eosio::asset fee{ 0, core_symbol };

  int64_t net_amount = 0;
  int64_t cpu_amount = 0;
  int64_t ram_bytes = ram_payment.amount * _ram_price_per_byte;
  
  // Process the NET and CPU powerup
  auto process = [&](int64_t frac, int64_t& amount, powerup_state_resource& resource) {
    if(frac > 0) {
      amount = int128_t(frac) * resource.weight / powerup_frac;
      
      eosio::check(resource.weight, "market doesn't have resources available");
      eosio::check(resource.utilization + amount <= resource.weight, "market doesn't have enough resources available");
      
      int64_t cost = calc_powerup_fee(resource, amount);
      eosio::check(cost > 0, "calculated fee is below minimum; try powering up with more resources");
      fee.amount += cost;
      resource.utilization += amount;
    }
  };

  process(net_frac, net_amount, state.net);
  process(cpu_frac, cpu_amount, state.cpu);

  eosio::check(fee <= payment, "Fee exceeds max payment");
  eosio::check(fee >= state.min_powerup_fee, "Fee is below minimum");

  state_sing.set(state, get_self());

  orders.emplace(payer, [&](auto& order) {
    order.id = orders.available_primary_key();
    order.owner = receiver;
    order.net_weight = net_amount;
    order.cpu_weight = cpu_amount;
    order.ram_bytes = ram_bytes;
    order.expires = now + eosio::days(days);
  });

  adjust_resources(payer, receiver, core_symbol, net_amount, cpu_amount, true);
  
  fill_tact(payer, payment);

  // Inline noop action
  powup_results::powupresult_action powup_result_act{ reserve_account, std::vector<eosio::permission_level>{} };
  powup_result_act.send(payment, net_amount, cpu_amount);
}

void system_contract::initemission(eosio::asset init_supply, uint64_t tact_duration, double emission_factor) {
  require_auth(get_self());

  emission_state_singleton emission_state_sing{ get_self(), get_self().value};
  
  eosio::check(!emission_state_sing.exists(), "Первый такт уже инициализирован. Повторная инициализация невозможна");

  eosio::check(tact_duration <= 365 * 86400, "Продолжительность такта не может превышать 1 год");
  eosio::check(emission_factor > 0 && emission_factor < 2.618, "Фактор эмиссии должен быть больше нуля и меньше 2.618");
  time_point_sec now =  eosio::current_time_point();

  auto state = emission_state_sing.get_or_default();
  state.tact_number = 1;
  state.tact_duration = tact_duration;
  state.emission_factor = emission_factor;
  state.current_supply = init_supply;
  state.tact_open_at = now;
  state.tact_close_at = eosio::time_point_sec(now.sec_since_epoch() + tact_duration);
  state.tact_fees = asset(0, core_symbol());
  state.tact_emission = asset(0, core_symbol());

  emission_state_sing.set(state, get_self());
}

void system_contract::update_tact(eosio::name payer) {
  require_auth(payer);

  emission_state_singleton emission_state_sing{ get_self(), get_self().value};
  
  if (emission_state_sing.exists()) {
  
    auto state = emission_state_sing.get();
    
    time_point_sec now = eosio::current_time_point();
    
    if (state.tact_close_at <= now) {
      state.tact_number += 1;
      state.tact_open_at = eosio::current_time_point();
      state.tact_close_at = eosio::time_point_sec(now.sec_since_epoch() + state.tact_duration);
      state.tact_fees.amount = 0;
      state.tact_emission.amount = 0;
      
      emission_state_sing.set(state, payer);
    }
  }
}

void system_contract::fill_tact(eosio::name payer, eosio::asset payment) {
  update_tact(payer);
  emission_state_singleton emission_state_sing{ get_self(), get_self().value};
  eosio::check(emission_state_sing.exists(), "Эмиссия не инициализирована");
    
  if (payment.amount > 0) {

    auto state = emission_state_sing.get();
    
    // eosio::check(state.tact_fees + payment <= state.current_supply, "Достигнут предел оборота в такте");

    // Добавляем собранные комиссии к числу комиссий такта
    state.tact_fees += payment;

    // Передаём комиссии делегатам за подписанные блоки
    token::transfer_action transfer_act{ token_account, { payer, active_permission } };
    transfer_act.send(payer, bpay_account, payment, "Witnesses per-block fee");
  
    _gstate.perblock_bucket         += payment.amount;
    _gstate.last_pervote_bucket_fill = current_time_point();
    
    print(" fact_fees: ", double(state.tact_fees.amount));
    print(" current_supply: ", double(state.current_supply.amount) / double(1 + state.emission_factor));
    
    //Производим эмиссию в фонд при достижении условия:
    if (double(state.tact_fees.amount) > double(state.current_supply.amount) / double(1 + state.emission_factor)) {
      eosio::asset new_emission = asset(uint64_t((1 + state.emission_factor) * double(state.tact_fees.amount) - double(state.current_supply.amount)), get_core_symbol());
      print("new_emission: ", new_emission);    
      
      if (new_emission.amount > 0) {
        state.tact_emission += new_emission;
        
        // Увеличиваем текущий объем в обороте на сумму эмиссии
        state.current_supply += new_emission;
        
        // Эмимитируем токены в фонд
        system_contract::emit(new_emission);

        // Изменяем веса CPU & NET в соответствии с новой эмиссией
        system_contract::change_weights(payer, new_emission);
      };
    
    }

    emission_state_sing.set(state, payer);
  }

}

void system_contract::change_weights(eosio::name payer, eosio::asset new_emission) {
  powerup_state_singleton power_state_sing{ get_self(), 0 };
  eosio::check(power_state_sing.exists(), "powerup hasn't been initialized");
  auto power_state = power_state_sing.get();
  
  print("on change weights: ", new_emission);

  power_state.cpu.weight += new_emission.amount;
  power_state.cpu.max_price += new_emission;
  power_state.cpu.min_price += new_emission;

  power_state.net.weight += new_emission.amount;
  power_state.net.max_price += new_emission;
  power_state.net.min_price += new_emission;

  power_state_sing.set(power_state, payer);
}

} // namespace eosiosystem
