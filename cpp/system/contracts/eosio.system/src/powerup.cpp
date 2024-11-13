#include <eosio.system/eosio.system.hpp>
#include <eosio.token/eosio.token.hpp>
#include <eosio/action.hpp>
#include <algorithm>
#include <cmath>

namespace eosiosystem {

void system_contract::adjust_resources(name payer, name account, symbol core_symbol, int64_t net_delta,
                                       int64_t cpu_delta, int64_t ram_delta, bool must_not_be_managed) {
   if (!net_delta && !cpu_delta && !ram_delta)
      return;
    
   user_resources_table totals_tbl(get_self(), account.value);
   auto                 tot_itr = totals_tbl.find(account.value);
   if (tot_itr == totals_tbl.end()) {
      tot_itr = totals_tbl.emplace(payer, [&](auto& tot) {
         tot.owner      = account;
         tot.net_weight = asset{ net_delta, core_symbol };
         tot.cpu_weight = asset{ cpu_delta, core_symbol };
         tot.ram_bytes  = ram_delta;
      });
   } else {
      totals_tbl.modify(tot_itr, payer, [&](auto& tot) {
         tot.net_weight.amount += net_delta;
         tot.cpu_weight.amount += cpu_delta;
         tot.ram_bytes += ram_delta;
      });
   }
   check(0 <= tot_itr->net_weight.amount, "insufficient staked total net bandwidth");
   check(0 <= tot_itr->cpu_weight.amount, "insufficient staked total cpu bandwidth");
   check(0 <= tot_itr->ram_bytes, "insufficient staked total ram");

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
        int64_t new_ram_limit = old_ram_limit;
        
        if (ram_delta != 0) { // применяем только если были изменения по квоте RAM
          new_ram_limit = old_ram_limit + ram_delta;
          
          if (new_ram_limit < ram_usage) { // если новый лимит меньше, чем используется байт, то формируем долг
              ram_debt = - (ram_usage - new_ram_limit); // новый долг - это отрицательная величина
              new_ram_limit = ram_usage; // новый лимит - это вся используемая память, т.е. квота будет равна 0 байт
              update_ram_debt_table(payer, account, ram_debt);
          };
          
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
                                           int64_t& cpu_delta_available, int64_t& ram_delta_available) {
   auto idx = orders.get_index<"byexpires"_n>();
   while (max_items--) {
      auto it = idx.begin();
      if (it == idx.end() || it->expires > now)
         break;
      net_delta_available += it->net_weight;
      cpu_delta_available += it->cpu_weight;
      ram_delta_available += it->ram_bytes;

      adjust_resources(get_self(), it->owner, core_symbol, -it->net_weight, -it->cpu_weight, -it -> ram_bytes);
      
      idx.erase(it);
   }
   state.net.utilization -= net_delta_available;
   state.cpu.utilization -= cpu_delta_available;
   state.ram.utilization -= ram_delta_available;
}

void system_contract::cfgpowerup(powerup_config& args) {
   require_auth(get_self());
   time_point_sec         now         = eosio::current_time_point();
   auto                   core_symbol = get_core_symbol();
   powerup_state_singleton state_sing{ get_self(), 0 };
   auto                   state = state_sing.get_or_default();

   eosio::check(eosio::is_account(_power_account), "eosio.power account must first be created"); 

   int64_t net_delta_available = 0;
   int64_t cpu_delta_available = 0;
   
   auto is_default_asset = []( const eosio::asset& a ) -> bool {
      return a.amount == 0 && a.symbol == symbol{};
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
   
   auto current_supply = eosio::token::get_supply(token_account, core_symbol.code() );
   bool first_launch = false;
   
   if (state.net.weight == 0 && state.cpu.weight == 0)
    first_launch = true;
  
   state.net.weight = current_supply.amount;
   state.cpu.weight = current_supply.amount;
   state.ram.weight = _gstate.max_ram_size;
   state.ram.utilization = first_launch == false ? state.ram.utilization : _gstate.max_ram_size - _gstate.free_ram();
   auto free_ram = _gstate.free_ram();
   
   _gstate.total_ram_bytes_reserved = _gstate.max_ram_size;
  
   if (first_launch)
    adjust_resources(get_self(), _power_account, core_symbol, state.net.weight, state.cpu.weight, free_ram, true);
   
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
   int64_t ram_delta_available = 0;
   process_powerup_queue(now, core_symbol, state, orders, max, net_delta_available, cpu_delta_available, ram_delta_available);

   adjust_resources(get_self(), _power_account, core_symbol, net_delta_available, cpu_delta_available, ram_delta_available, true);
   state_sing.set(state, get_self());
}

void system_contract::powerup(const name& payer, const name& receiver, uint32_t days, const asset& payment, const bool transfer) {
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

  if (transfer == true) {
    check(has_auth(_registrator) || has_auth(get_self()), "Недостаточно прав доступа для перевода ресурсов");
  } else { //если transfer == true, то проверку на сумму не делаем, т.к. при регистрации сумма оплаты может быть меньше минимума.
    eosio::check(payment >= state.min_powerup_fee, "Payment is below minimum");
  }


  // Divide payment
  eosio::asset net_payment = payment / 4; // 25%
  eosio::asset cpu_payment = payment / 4; // 25%
  eosio::asset ram_payment = payment / 2; // 50%
  
  int64_t net_delta_available = 0;
  int64_t cpu_delta_available = 0;
  int64_t ram_delta_available = 0;
  
  process_powerup_queue(now, core_symbol, state, orders, 2, net_delta_available, cpu_delta_available, ram_delta_available);
   
  // Process NET and CPU fractions separately based on specified payment portions
  int64_t net_frac = 0;
  int64_t cpu_frac = 0;
  int64_t ram_frac = 0;

  if (state.net.weight > 0)
    net_frac = net_payment.amount;
  if (state.cpu.weight > 0)
    cpu_frac = cpu_payment.amount;
  if (state.ram.weight > 0)
    ram_frac = ram_payment.amount;

  // Get RAM from linear market
  net_delta_available -= net_frac;
  cpu_delta_available -= cpu_frac;
  
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
  process(ram_frac, state.ram);
  
  state_sing.set(state, get_self());

  int64_t ram_after_pay_debt = update_ram_debt_table(payer, receiver, ram_frac); 

  ram_delta_available -= ram_after_pay_debt;
  eosio::check(ram_after_pay_debt > 0, "Недостаточное пополнение ресурсов для погашения используемой RAM. Попробуйте увеличить сумму пополнения.");
  
  if (transfer == false){
    orders.emplace(payer, [&](auto& order) {
      order.id = orders.available_primary_key();
      order.owner = receiver;
      order.net_weight = net_frac;
      order.cpu_weight = cpu_frac;
      order.ram_bytes = ram_after_pay_debt;
      order.expires = now + eosio::days(days);
    });
  };
  
  adjust_resources(get_self(), _power_account, core_symbol, net_delta_available, cpu_delta_available, ram_delta_available, true);
  adjust_resources(get_self(), receiver, core_symbol, net_frac, cpu_frac, ram_after_pay_debt, true);
  
  fill_tact(payer, payment);
  
}

int64_t system_contract::update_ram_debt_table(name payer, name account, int64_t ram_bytes) {
    ram_debts_table debts(get_self(), get_self().value);
    int64_t remaining_ram = ram_bytes;

    auto debt_itr = debts.find(account.value);
    if (debt_itr != debts.end()) {
        int64_t debt = debt_itr->ram_debt;
        eosio::check(debt <= 0, "Запись о долге имеет положительное значение, что недопустимо.");

        if (ram_bytes >= 0) {
            remaining_ram += debt; // Уменьшение доступного количества ram_bytes на величину долга
            if (remaining_ram >= 0) {
                debts.erase(debt_itr); // Долг полностью погашен или долга не было
            } else {
                debts.modify(debt_itr, payer, [&](auto& d) {
                    d.ram_debt += ram_bytes; // Часть долга погашена
                });
                remaining_ram = 0;
            }
        }
    } else {
        if (ram_bytes < 0) {
          // Создание новой записи о долге для отрицательного количества ram_bytes
          debts.emplace(payer, [&](auto& record) {
              record.account = account;
              record.ram_debt = ram_bytes;
          });
        }; // для положительного ничего не делаем
    }

    return remaining_ram;
}


void system_contract::initemission(eosio::asset init_supply, uint64_t tact_duration, double emission_factor) {
  require_auth(get_self());

  emission_state_singleton emission_state_sing{ get_self(), get_self().value};
  
  eosio::check(!emission_state_sing.exists(), "Эмиссия уже инициализирована. Повторная инициализация невозможна");
  eosio::check(init_supply.amount > 0, "Стартовый объем токенов не может быть отрицательной");
  eosio::check(init_supply.symbol == core_symbol(), "Неверный символ");
  eosio::check(tact_duration > 0, "Продолжительность такта должна быть положительной");
  eosio::check(tact_duration <= 365 * 86400, "Продолжительность такта не может превышать 1 год");
  eosio::check(emission_factor > 0 && emission_factor <= 1, "Фактор эмиссии должен быть больше нуля и меньше или равен 1");

  time_point_sec now =  eosio::current_time_point();

  auto state = emission_state_sing.get_or_default();
  state.tact_number = 1;
  state.tact_duration = tact_duration;
  state.emission_factor = emission_factor;
  state.current_supply = init_supply;
  state.tact_open_at = now;
  state.tact_close_at = eosio::time_point_sec(now.sec_since_epoch() + tact_duration);
  state.tact_fees = asset(0, core_symbol());
  state.back_from_producers = asset(0, core_symbol());
  
  state.tact_emission = asset(0, core_symbol());
  
  state.emission_start = asset(uint64_t(double(init_supply.amount) / double(1 + emission_factor)), core_symbol());

  emission_state_sing.set(state, get_self());
}

void system_contract::update_tact() {
  emission_state_singleton emission_state_sing{ get_self(), get_self().value};
  
  if (emission_state_sing.exists()) {
  
    auto state = emission_state_sing.get();
    
    time_point_sec now = eosio::current_time_point();
    print("state.tact_close_at <= now: ", state.tact_close_at <= now);
    if (state.tact_close_at <= now) {
      print("on inside!?!?!");
      state.tact_number += 1;
      state.tact_open_at = eosio::current_time_point();
      state.tact_close_at = eosio::time_point_sec(now.sec_since_epoch() + state.tact_duration);

      // Сдвигаем границу начала эмиссии
      state.emission_start = asset(uint64_t(double(state.current_supply.amount) / double(1 + state.emission_factor)), state.current_supply.symbol);
      
      state.tact_fees.amount = 0;
      state.tact_emission.amount = 0;
      emission_state_sing.set(state, get_self());
    }
  }
}

void system_contract::fill_tact(eosio::name payer, eosio::asset payment) {
  if (payment.amount > 0) {
    eosio::check(_producers_percent > 0, "Процент производителей должен быть положителен");
    eosio::check(_fund_percent >= 0, "Процент фонд должен быть больше или равен нулю");
    eosio::check(_producers_percent + _fund_percent == HUNDR_PERCENTS, "Сумма вознаграждений производителей и фонда должны в сумме равняться 100% (1000000)");

    // Распределяем поступления
    eosio::asset producers_amount = payment * _producers_percent / HUNDR_PERCENTS;
    eosio::asset fund_amount = payment * _fund_percent / HUNDR_PERCENTS; 

    // Передаем в фонд
    eosio::token::transfer_action transfer_act1{ token_account, { {payer, active_permission} } };
    transfer_act1.send( payer, _saving_account, fund_amount, "Передача токенов в фонд" );
        
    // Передаём комиссии делегатам за подписанные блоки
    eosio::token::transfer_action transfer_act2{ token_account, { payer, active_permission } };
    transfer_act2.send(payer, bpay_account, producers_amount, "Передача токенов делегатам");

    emission_state_singleton emission_state_sing{ get_self(), get_self().value};
    
    if (emission_state_sing.exists()) {
      update_tact();

      auto state = emission_state_sing.get();
      
      // Добавляем собранные комиссии к числу комиссий такта
      state.tact_fees += payment;
      state.back_from_producers += producers_amount;
      
      _gstate.perblock_bucket         += producers_amount.amount;
      _gstate.last_pervote_bucket_fill = eosio::current_time_point();
      
      print(" Супплай: ", state.current_supply);
      print(" Платеж: ", payment);

      //Производим эмиссию в фонд при условии:
      if (state.tact_fees > state.emission_start) {
        eosio::asset new_emission = asset(uint64_t((1 + state.emission_factor) * (double(state.tact_fees.amount)) - double(state.current_supply.amount)), get_core_symbol());
        
        print(" Эмиссия в фонд ", new_emission);

        if (new_emission.amount > 0) {
          // Увеличиваем объем эмиссии такта
          state.tact_emission += new_emission;
          
          // Увеличиваем текущий объем в обороте на сумму эмиссии
          state.current_supply += new_emission;
          
          // Эмимитируем токены в фонд
          system_contract::emit(new_emission);

          // Изменяем веса CPU & NET в соответствии с новой эмиссией
          system_contract::change_weights(payer, new_emission);
        };
      
      }

      emission_state_sing.set(state, get_self());
    }
  }
}

void system_contract::change_weights(eosio::name payer, eosio::asset new_emission) {
  powerup_state_singleton power_state_sing{ get_self(), 0 };
  eosio::check(power_state_sing.exists(), "powerup hasn't been initialized");
  auto power_state = power_state_sing.get();

  power_state.cpu.weight += new_emission.amount;
  power_state.net.weight += new_emission.amount;
  power_state.ram.weight = _gstate.max_ram_size;
   
  power_state_sing.set(power_state, get_self());
}

} // namespace eosiosystem
