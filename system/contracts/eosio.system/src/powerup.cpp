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
                                       int64_t cpu_delta, int64_t ram_delta, bool must_not_be_managed) {
   if (!net_delta && !cpu_delta && !ram_delta)
      return;
  print("on adjust_resources for: ", account);
   user_resources_table totals_tbl(get_self(), account.value);
   auto                 tot_itr = totals_tbl.find(account.value);
   if (tot_itr == totals_tbl.end()) {
    print(" on 2");
      tot_itr = totals_tbl.emplace(payer, [&](auto& tot) {
         tot.owner      = account;
         tot.net_weight = asset{ net_delta, core_symbol };
         tot.cpu_weight = asset{ cpu_delta, core_symbol };
      });
   } else {
    print(" on 3");
      totals_tbl.modify(tot_itr, same_payer, [&](auto& tot) {
         tot.net_weight.amount += net_delta;
         tot.cpu_weight.amount += cpu_delta;
         tot.ram_bytes += ram_delta;
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

      if (!(net_managed && cpu_managed && ram_managed)) {
        int64_t old_ram_limit, net, cpu, ram_usage, ram_limit, ram_debt, ram_to_back;
        get_resource_limits(account, old_ram_limit, net, cpu);
        get_account_ram_usage(account, ram_usage);
        ram_debt = 0; // Initialize ram_debt to 0
        ram_to_back = 0; // Initialize ram_to_back to 0
        print(" previous_ram_limit: ", old_ram_limit);
        int64_t new_ram_limit = old_ram_limit;
        print(" delta: ", ram_delta);
        // int64_t free_ram = old_ram_limit - ram_usage;
        
        if (ram_delta != 0) { // применяем только если были изменения по квоте RAM
          // Вычисляем новую квоту RAM на основании того, что было, плюс изменения, за вычетом того, что продолжает использоваться.
          new_ram_limit = old_ram_limit + ram_delta;
          print(" new_ram_limit1: ", new_ram_limit);
          
          if (new_ram_limit < ram_usage) { // если новый лимит меньше, чем используется байт, то формируем долг
              ram_debt = - (ram_usage - new_ram_limit); // новый долг - это отрицательная величина
              print(" ram_debt: ", ram_debt);
              new_ram_limit = ram_usage; // новый лимит - это вся используемая память, т.е. квота будет равна 0 байт
              print(" new_ram_limit2: ", new_ram_limit);
          };
          
          
          if ( ram_delta < 0 ) { // если списание
            ram_to_back = old_ram_limit - new_ram_limit; //считаем байты, которые продать с аккаунта
            /*
              Если свободно всё, то будет продано ram_to_back = - abs(delta)
              Если с долгом, то будет продано ram_to_back = ram_usage, т.е. квота установится в ноль.
            */

            print(" ram_to_back: ", ram_to_back);  

            if(ram_to_back > 0) {// продаём только если значение положительно
                print(" on 6");
                back_ram(ram_to_back);
                print(" on 7");
            };
          };
          
          
          
          // Store the ram_debt in a persistent table if ram_debt is non-zero
          if(ram_debt != 0) {
              // Logic to update the account's ram debt record in the persistent table
              // Assuming a function update_ram_debt_table exists for this purpose
              update_ram_debt_table(payer, account, ram_debt);
              print(" on 5");
          }
      
          
        }
        // Set the new resource limits
        set_resource_limits(account, new_ram_limit, net + net_delta, cpu + cpu_delta);
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

      adjust_resources(get_self(), it->owner, core_symbol, -it->net_weight, -it->cpu_weight, -it -> ram_bytes);
      
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

   adjust_resources(get_self(), reserve_account, core_symbol, net_delta_available, cpu_delta_available, 0, true);
   state_sing.set(state, get_self());
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

   adjust_resources(get_self(), reserve_account, core_symbol, net_delta_available, cpu_delta_available, 0, true);
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
  
  int64_t net_delta_available = 0;
  int64_t cpu_delta_available = 0;
  process_powerup_queue(now, core_symbol, state, orders, 2, net_delta_available, cpu_delta_available);

  // Process NET and CPU fractions separately based on specified payment portions
  int64_t net_frac = 0;
  int64_t cpu_frac = 0;

  if (state.net.weight > 0)
    net_frac = int128_t(net_payment.amount);
  if (state.cpu.weight > 0)
    cpu_frac = int128_t(cpu_payment.amount);

  // Get RAM from linear market
  int64_t ram_bytes = get_ram(ram_payment);

  // Process the NET and CPU powerup
  auto process = [&](int64_t amount, powerup_state_resource& resource) {
    if(amount > 0) {
      eosio::check(resource.weight, "market doesn't have resources available");
      eosio::check(resource.utilization + amount <= resource.weight, "market doesn't have enough resources available");
      resource.utilization += amount;
    }
  };

  process(net_frac, state.net);
  process(cpu_frac, state.cpu);
  
  eosio::check(payment >= state.min_powerup_fee, "Payment is below minimum");

  state_sing.set(state, get_self());

  int64_t ram_after_pay_debt = update_ram_debt_table(payer, receiver, ram_bytes); 
  print("ram_after_pay_debt: ", ram_after_pay_debt);
  eosio::check(ram_after_pay_debt > 0, "Недостаточное пополнение ресурсов для погашения используемой RAM. Попробуйте увеличить сумму пополнения.");
  
  orders.emplace(payer, [&](auto& order) {
    order.id = orders.available_primary_key();
    order.owner = receiver;
    order.net_weight = net_frac;
    order.cpu_weight = cpu_frac;
    order.ram_bytes = ram_after_pay_debt;
    order.expires = eosio::current_time_point() + eosio::seconds(10); // TODO delete it
    // order.expires = now + eosio::days(days);
  });
  
  fill_tact(payer, payment);
  adjust_resources(payer, receiver, core_symbol, net_frac, cpu_frac, ram_after_pay_debt, true);
  powup_results::powupresult_action powup_result_act{ reserve_account, std::vector<eosio::permission_level>{} };
  powup_result_act.send(payment, net_frac, cpu_frac);
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
