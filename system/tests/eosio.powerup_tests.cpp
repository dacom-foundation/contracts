#include <boost/test/unit_test.hpp>
#include <cstdlib>
#include <eosio/chain/contract_table_objects.hpp>
#include <eosio/chain/exceptions.hpp>
#include <eosio/chain/global_property_object.hpp>
#include <eosio/chain/resource_limits.hpp>
#include <eosio/chain/wast_to_wasm.hpp>
#include <fc/log/logger.hpp>
#include <iostream>
#include <sstream>
#include "contracts.hpp"

#include "eosio.system_tester.hpp"

inline constexpr int64_t powerup_frac  = 1'000'000'000'000'000ll; // 1.0 = 10^15
inline constexpr int64_t stake_weight = 100'000'000'0000ll; // 10^12

struct powerup_config {
   std::optional<uint32_t> powerup_days    = {};
   std::optional<asset>    min_powerup_fee = {};
};
FC_REFLECT(powerup_config, (powerup_days)(min_powerup_fee))

struct powerup_state_resource {
   int64_t        weight;
   int64_t        utilization;
};
FC_REFLECT(powerup_state_resource,                                                                           //
           (weight)(utilization))

struct powerup_state {
   powerup_state_resource net;
   powerup_state_resource cpu;
   powerup_state_resource ram;
   uint32_t              powerup_days;
   asset                 min_powerup_fee;
};
FC_REFLECT(powerup_state, (net)(cpu)(ram)(powerup_days)(min_powerup_fee))

using namespace eosio_system;

struct powerup_tester : eosio_system_tester {

   powerup_tester() { create_accounts_with_resources({ "eosio.power"_n }); }

   void start_rex() {
      create_account_with_resources("rexholder111"_n, config::system_account_name, core_sym::from_string("1.0000"),
                                    false);
      transfer(config::system_account_name, "rexholder111"_n, core_sym::from_string("1001.0000"));
      BOOST_REQUIRE_EQUAL("", stake("rexholder111"_n, "rexholder111"_n, core_sym::from_string("500.0000"),
                                    core_sym::from_string("500.0000")));
      create_account_with_resources("proxyaccount"_n, config::system_account_name, core_sym::from_string("1.0000"),
                                    false, core_sym::from_string("500.0000"), core_sym::from_string("500.0000"));
      BOOST_REQUIRE_EQUAL("",
                          push_action("proxyaccount"_n, "regproxy"_n, mvo()("proxy", "proxyaccount")("isproxy", true)));
      BOOST_REQUIRE_EQUAL("", vote("rexholder111"_n, {}, "proxyaccount"_n));
      BOOST_REQUIRE_EQUAL("", push_action("rexholder111"_n, "deposit"_n,
                                          mvo()("owner", "rexholder111")("amount", asset::from_string("1.0000 TST"))));
      BOOST_REQUIRE_EQUAL("", push_action("rexholder111"_n, "buyrex"_n,
                                          mvo()("from", "rexholder111")("amount", asset::from_string("1.0000 TST"))));
   }

   
   template <typename F>
   powerup_config make_config(F f) {
      powerup_config config;

      config.powerup_days    = 1;
      config.min_powerup_fee = asset::from_string("1.0000 TST");

      f(config);
      return config;
   }

   powerup_config make_config() {
      return make_config([](auto&) {});
   }

   template <typename F>
   powerup_config make_default_config(F f) {
      powerup_config config;
      f(config);
      return config;
   }

   action_result configbw(const powerup_config& config) {
      // Verbose solution needed to work around bug in abi_serializer that fails if optional values aren't explicitly
      // specified with a null value.

      auto optional_to_variant = []( const auto& v ) -> fc::variant {
         return (!v ? fc::variant() : fc::variant(*v));
      };

      auto conf = mvo("powerup_days",    optional_to_variant(config.powerup_days))
                     ("min_powerup_fee", optional_to_variant(config.min_powerup_fee))
      ;

      //idump((fc::json::to_pretty_string(conf)));
      return push_action(config::system_account_name, "cfgpowerup"_n, mvo()("args", std::move(conf)));

      // If abi_serializer worked correctly, the following is all that would be needed:
      //return push_action(config::system_account_name, "cfgpowerup"_n, mvo()("args", config));
   }

   action_result powerupexec(name user, uint16_t max) {
      return push_action(user, "powerupexec"_n, mvo()("user", user)("max", max));
   }

   action_result init_emission(asset init_supply, uint64_t tact_duration, double emission_factor) {
      return push_action(config::system_account_name, "initemission"_n, mvo()("init_supply", init_supply)("tact_duration", tact_duration)("emission_factor", emission_factor));
   }

   action_result powerup(const name& payer, const name& receiver, uint32_t days, asset payment, bool transfer = false) {
      return push_action(payer, "powerup"_n,
                         mvo()("payer", payer)("receiver", receiver)("days", days)("payment", payment)("transfer", transfer));
   }

   powerup_state get_state() {
      vector<char> data = get_row_by_account(config::system_account_name, {}, "powerstate"_n, "powerstate"_n);
      return fc::raw::unpack<powerup_state>(data);
   }

   int64_t get_global_free_ram() {
    const auto free_ram = get_global_state()["max_ram_size"].as<int64_t>() - get_global_state()["total_ram_bytes_reserved"].as<int64_t>();
    return free_ram;
   }
   
   int64_t get_global_used_ram() {
    const auto used_ram = get_global_state()["total_ram_bytes_reserved"].as<int64_t>();
    return used_ram;
   }

   struct account_info {
      int64_t ram = 0;
      int64_t net = 0;
      int64_t cpu = 0;
      asset   liquid;
      int64_t ram_usage = 0;
   };

   account_info get_account_info(account_name acc) {
      account_info info;
      control->get_resource_limits_manager().get_account_limits(acc, info.ram, info.net, info.cpu);
      info.ram_usage = control->get_resource_limits_manager().get_account_ram_usage(acc);
      info.liquid = get_balance(acc);
      return info;
   };

   void check_powerup(const name& payer, const name& receiver, uint32_t days, const asset& payment) {
      
      auto ram_payment = payment.get_amount() / 2;
      auto net_payment = ram_payment / 2;
      auto cpu_payment = ram_payment / 2;
      
      auto before_payer    = get_account_info(payer);
      auto before_receiver = get_account_info(receiver);
      auto before_reserve  = get_account_info("eosio.power"_n);
      
      auto before_bpay  = get_account_info("eosio.bpay"_n);
      auto before_fund  = get_account_info("eosio.saving"_n);

      auto before_state    = get_state();
      auto before_global_used_ram = get_global_used_ram();
      // printf("before_reserve.ram: %ld\n", before_reserve.ram);
      BOOST_REQUIRE_EQUAL("", powerup(payer, receiver, days, payment));

      auto after_payer    = get_account_info(payer);
      auto after_receiver = get_account_info(receiver);
      auto after_reserve  = get_account_info("eosio.power"_n);

      auto after_bpay  = get_account_info("eosio.bpay"_n);
      auto after_fund  = get_account_info("eosio.saving"_n);

      auto after_global_used_ram = get_global_used_ram();
      auto after_state    = get_state();
      
      if (true) {
          printf("\n");
          // printf("before_receiver.cpu: %ld\n", before_receiver.cpu);
          // printf("after_receiver.cpu: %ld\n", after_receiver.cpu);
          // printf("before_receiver.net: %ld\n", before_receiver.net);
          // printf("after_receiver.net: %ld\n", after_receiver.net);
          printf("before_receiver.ram: %ld\n", before_receiver.ram);
          printf("after_receiver.ram: %ld\n", after_receiver.ram);
          printf("\n");
          // printf("before_reserve.net: %ld\n", before_reserve.net);
          // printf("after_reserve.net: %ld\n", after_reserve.net);
          // printf("before_reserve.cpu: %ld\n", before_reserve.cpu);
          // printf("after_reserve.cpu: %ld\n", after_reserve.cpu);
          printf("before_reserve.ram: %ld\n", before_reserve.ram);
          printf("after_reserve.ram: %ld\n", after_reserve.ram);
          printf("\n");
          printf("ram_payment: %ld\n", ram_payment);
          // printf("net_payment: %ld\n", net_payment);
          // printf("cpu_payment: %ld\n", cpu_payment);

          
      }

      if (payer != receiver) {
         BOOST_REQUIRE_EQUAL(before_payer.ram, after_payer.ram );
         BOOST_REQUIRE_EQUAL(before_payer.net, after_payer.net);
         BOOST_REQUIRE_EQUAL(before_payer.cpu, after_payer.cpu);
         BOOST_REQUIRE_EQUAL(before_receiver.liquid, after_receiver.liquid);
      }

      BOOST_REQUIRE_EQUAL(after_receiver.ram - before_receiver.ram, ram_payment);
      BOOST_REQUIRE_EQUAL(after_receiver.net - before_receiver.net, net_payment);
      BOOST_REQUIRE_EQUAL(after_receiver.cpu - before_receiver.cpu, cpu_payment);
      BOOST_REQUIRE_EQUAL(before_payer.liquid - after_payer.liquid, payment);

      BOOST_REQUIRE_EQUAL(before_reserve.ram - after_reserve.ram, ram_payment);
      BOOST_REQUIRE_EQUAL(before_reserve.net - after_reserve.net, net_payment);
      BOOST_REQUIRE_EQUAL(before_reserve.cpu - after_reserve.cpu, cpu_payment);

      // проверяем изменение использования      
      BOOST_REQUIRE_EQUAL(after_state.net.utilization - before_state.net.utilization, net_payment);
      BOOST_REQUIRE_EQUAL(after_state.cpu.utilization - before_state.cpu.utilization, cpu_payment);
      BOOST_REQUIRE_EQUAL(after_state.ram.utilization - before_state.ram.utilization, ram_payment);

      // проверяем увеличение ликвида аккаунта eosio.bpay и фонда
      uint64_t producers_percent = 90;
      uint64_t fund_percent = 100 - producers_percent;
      
      auto block_payment = asset(payment.get_amount() / 100 * producers_percent, payment.get_symbol());
      auto fund_payment = asset(payment.get_amount() / 100 * fund_percent, payment.get_symbol());
      BOOST_REQUIRE_EQUAL(after_bpay.liquid - before_bpay.liquid, block_payment);
   }
};

template <typename A, typename B, typename D>
bool near(A a, B b, D delta) {
   if (abs(a - b) <= delta)
      return true;
   elog("near: ${a} ${b}", ("a", a)("b", b));
   return false;
}

BOOST_AUTO_TEST_SUITE(eosio_system_powerup_tests)

BOOST_FIXTURE_TEST_CASE(config_tests, powerup_tester) try {
   
   BOOST_REQUIRE_EQUAL("missing authority of eosio",
                       push_action("alice1111111"_n, "cfgpowerup"_n, mvo()("args", make_config())));
   BOOST_REQUIRE_EQUAL(wasm_assert_msg("powerup hasn't been initialized"), powerupexec("alice1111111"_n, 10));

   BOOST_REQUIRE_EQUAL(wasm_assert_msg("powerup_days must be > 0"),
                       configbw(make_config([&](auto& c) { c.powerup_days = 0; })));
   BOOST_REQUIRE_EQUAL(wasm_assert_msg("min_powerup_fee doesn't match core symbol"), configbw(make_config([&](auto& c) {
                          c.min_powerup_fee = asset::from_string("1000000.000 TST");
                       })));
   BOOST_REQUIRE_EQUAL(wasm_assert_msg("min_powerup_fee does not have a default value"),
                       configbw(make_config([&](auto& c) { c.min_powerup_fee = {}; })));
   BOOST_REQUIRE_EQUAL(wasm_assert_msg("min_powerup_fee must be positive"),
                       configbw(make_config([&](auto& c) { c.min_powerup_fee = asset::from_string("0.0000 TST"); })));
   BOOST_REQUIRE_EQUAL(wasm_assert_msg("min_powerup_fee must be positive"),
                       configbw(make_config([&](auto& c) { c.min_powerup_fee = asset::from_string("-1.0000 TST"); })));

} // config_tests
FC_LOG_AND_RETHROW()


BOOST_AUTO_TEST_CASE(rent_tests) try {
   {
      powerup_tester t;
      t.produce_block();

      BOOST_REQUIRE_EQUAL(t.wasm_assert_msg("powerup hasn't been initialized"), //
                          t.powerup("bob111111111"_n, "alice1111111"_n, 1, asset::from_string("1.0000 TST")));

      BOOST_REQUIRE_EQUAL("", t.configbw(t.make_config([&](auto& config) {
         config.powerup_days    = 1;
         config.min_powerup_fee = asset::from_string("1.0000 TST");
      })));
      
      t.create_account_with_resources("aaaaaaaaaaaa"_n, config::system_account_name, core_sym::from_string("1.0000"), false, core_sym::from_string("10000.0000"), core_sym::from_string("10000.0000"));
      
      t.transfer(config::system_account_name, "aaaaaaaaaaaa"_n, core_sym::from_string("1000.0000"));
      
      int64_t before_before_net_utilization = t.get_state().net.utilization;
      int64_t before_before_ram_utilization = t.get_state().ram.utilization;
      int64_t before_before_cpu_utilization = t.get_state().cpu.utilization;
      

      t.check_powerup("aaaaaaaaaaaa"_n, "aaaaaaaaaaaa"_n, 1, asset::from_string("1000.0000 TST"));
      
      {

        int64_t before_net_utilization = t.get_state().net.utilization;
        int64_t before_ram_utilization = t.get_state().ram.utilization;
        int64_t before_cpu_utilization = t.get_state().cpu.utilization;
        auto before_payer    = t.get_account_info("aaaaaaaaaaaa"_n);
        auto before_reserve  = t.get_account_info("eosio.power"_n);
        
        t.produce_block(fc::days(1) - fc::milliseconds(1500));

        // обновляем состояние аренды
        BOOST_REQUIRE_EQUAL("", t.powerupexec(config::system_account_name, 10));
        
        // ещё нет изменений по RAM аккаунта после обновления powerexec
        BOOST_REQUIRE_EQUAL(t.get_state().net.utilization, before_net_utilization);
        BOOST_REQUIRE_EQUAL(t.get_state().ram.utilization, before_ram_utilization);
        BOOST_REQUIRE_EQUAL(t.get_state().cpu.utilization, before_cpu_utilization);

        t.produce_block(fc::milliseconds(2000));

        // теперь есть изменения по всем ресурсам аккаунта - они списаны
        int64_t ram_payment = 500 * 10000;
        int64_t net_payment = 250 * 10000;
        int64_t cpu_payment = 250 * 10000;

        // обновляем состояние аренды
        BOOST_REQUIRE_EQUAL("", t.powerupexec(config::system_account_name, 10));
        
        // проверяем как прошел возврат ресурсов
        auto after_payer    = t.get_account_info("aaaaaaaaaaaa"_n);
        auto after_reserve  = t.get_account_info("eosio.power"_n);
        int64_t after_net_utilization = t.get_state().net.utilization;
        int64_t after_ram_utilization = t.get_state().ram.utilization;
        int64_t after_cpu_utilization = t.get_state().cpu.utilization;

        BOOST_REQUIRE_EQUAL(after_net_utilization, before_net_utilization - net_payment);
        BOOST_REQUIRE_EQUAL(after_ram_utilization, before_ram_utilization - ram_payment);
        BOOST_REQUIRE_EQUAL(after_cpu_utilization, before_cpu_utilization - cpu_payment);
        
        printf("\n");
        printf("after_net_utilization: %ld\n", after_net_utilization);
        printf("before_net_utilization: %ld\n", before_net_utilization);
        printf("\n");
        
        BOOST_REQUIRE_EQUAL(after_net_utilization, before_before_net_utilization);
        BOOST_REQUIRE_EQUAL(after_ram_utilization, before_before_ram_utilization);
        BOOST_REQUIRE_EQUAL(after_cpu_utilization, before_before_cpu_utilization);
        
        BOOST_REQUIRE_EQUAL(after_reserve.ram - before_reserve.ram, ram_payment);
        BOOST_REQUIRE_EQUAL(after_reserve.net - before_reserve.net, net_payment);
        BOOST_REQUIRE_EQUAL(after_reserve.cpu - before_reserve.cpu, cpu_payment);

        BOOST_REQUIRE_EQUAL(before_payer.ram - after_payer.ram, ram_payment);
        BOOST_REQUIRE_EQUAL(before_payer.net - after_payer.net, net_payment);
        BOOST_REQUIRE_EQUAL(before_payer.cpu - after_payer.cpu, cpu_payment);

      }
      
   }
} // rent_tests
FC_LOG_AND_RETHROW()



BOOST_AUTO_TEST_CASE(debts_tests) try {
   {
      powerup_tester t;
      t.produce_block();

      BOOST_REQUIRE_EQUAL(t.wasm_assert_msg("powerup hasn't been initialized"), //
                          t.powerup("bob111111111"_n, "alice1111111"_n, 1, asset::from_string("1.0000 TST")));

      BOOST_REQUIRE_EQUAL("", t.configbw(t.make_config([&](auto& config) {
         config.powerup_days    = 1;
         config.min_powerup_fee = asset::from_string("1.0000 TST");
      })));
      
      BOOST_REQUIRE_EQUAL("", t.init_emission(asset::from_string("1000.0000 TST"), 86400, 0.618));
      
      int64_t before_before_net_utilization = t.get_state().net.utilization;
      int64_t before_before_ram_utilization = t.get_state().ram.utilization;
      int64_t before_before_cpu_utilization = t.get_state().cpu.utilization;

      { // проверяем долг
        // создаём аккаунт для контракта
        t.create_account_with_resources("bbbbbbbbbbbb"_n, config::system_account_name, core_sym::from_string("1.0000"), false, core_sym::from_string("10000.0000"), core_sym::from_string("10000.0000"));
        
        // насыпаем токенов
        t.transfer(config::system_account_name, "bbbbbbbbbbbb"_n, core_sym::from_string("3000.0000"));
        
        // сохраняем начальное состояние
        int64_t before_net_utilization = t.get_state().net.utilization;
        int64_t before_ram_utilization = t.get_state().ram.utilization;
        int64_t before_cpu_utilization = t.get_state().cpu.utilization;
        auto before_payer    = t.get_account_info("bbbbbbbbbbbb"_n);
        auto before_reserve  = t.get_account_info("eosio.power"_n);
        printf("\n");
        // арендуем ресурсы новому аккаунту
        t.check_powerup("bbbbbbbbbbbb"_n, "bbbbbbbbbbbb"_n, 1, asset::from_string("1000.0000 TST"));

        // устанавливаем контракт
        t.set_code( "bbbbbbbbbbbb"_n, contracts::bios_wasm() );
        t.produce_blocks();
        printf("\n");
        // сохраняем промежуточное состояние
        auto middle_payer    = t.get_account_info("bbbbbbbbbbbb"_n);
        
        auto ram_used_by_contract = middle_payer.ram_usage - before_payer.ram_usage;
        // пропускаем время
        t.produce_block(fc::days(1));
        
        // изымаем ресурсы без ошибок
        BOOST_REQUIRE_EQUAL("", t.powerupexec(config::system_account_name, 10));
        
        t.produce_blocks();
        // сохраняем финальное состояние
        int64_t after_net_utilization = t.get_state().net.utilization;
        int64_t after_ram_utilization = t.get_state().ram.utilization;
        int64_t after_cpu_utilization = t.get_state().cpu.utilization;
        auto after_payer    = t.get_account_info("bbbbbbbbbbbb"_n);
        auto after_reserve  = t.get_account_info("eosio.power"_n);
        
        printf("before_payer_ram_usage: %ld\n", before_payer.ram_usage);
        printf("before_payer_ram_limit: %ld\n", before_payer.ram);
        
        printf("middle_payer_ram_usage: %ld\n", middle_payer.ram_usage);
        printf("middle_payer_ram_limit: %ld\n", middle_payer.ram);
        
        printf("after_payer_ram_usage: %ld\n", after_payer.ram_usage);
        printf("after_payer_ram_limit: %ld\n", after_payer.ram);
        
        printf("ram_used_by_contract: %ld\n", ram_used_by_contract);
        
        int64_t ram_payment = 500 * 10000;
        int64_t net_payment = 250 * 10000;
        int64_t cpu_payment = 250 * 10000;

        BOOST_REQUIRE_EQUAL(after_net_utilization, before_net_utilization);
        BOOST_REQUIRE_EQUAL(after_ram_utilization, before_ram_utilization);
        BOOST_REQUIRE_EQUAL(after_cpu_utilization, before_cpu_utilization);
        
        // повторно арендуем ресурсы новому аккаунту
        BOOST_REQUIRE_EQUAL("", t.powerup("bbbbbbbbbbbb"_n, "bbbbbbbbbbbb"_n, 1, asset::from_string("1000.0000 TST")));
        t.produce_blocks();

        int64_t after_after_net_utilization = t.get_state().net.utilization;
        int64_t after_after_ram_utilization = t.get_state().ram.utilization;
        int64_t after_after_cpu_utilization = t.get_state().cpu.utilization;
        
        auto after_after_payer    = t.get_account_info("bbbbbbbbbbbb"_n);
        auto after_after_reserve  = t.get_account_info("eosio.power"_n);
        
        printf("\n");
        printf("after_after_payer_ram_limit: %ld\n", after_after_payer.ram);
        printf("after_after_payer_ram_usage: %ld\n", after_after_payer.ram_usage);
        
        BOOST_REQUIRE_EQUAL(after_after_payer.ram, middle_payer.ram);
        
        // повторно арендуем ресурсы новому аккаунту
        BOOST_REQUIRE_EQUAL("", t.powerup("bbbbbbbbbbbb"_n, "bbbbbbbbbbbb"_n, 1, asset::from_string("1000.0000 TST")));

        // пропускаем время
        t.produce_block(fc::days(31));
        
        // изымаем ресурсы без ошибок
        BOOST_REQUIRE_EQUAL("", t.powerupexec(config::system_account_name, 10));
        
        t.produce_blocks();

        auto payer_on_finish    = t.get_account_info("bbbbbbbbbbbb"_n);
        BOOST_REQUIRE_EQUAL(payer_on_finish.ram, after_after_payer.ram_usage);
        
      }

      { // повторно проверяем долг того же аккаунта
        
        // насыпаем токенов
        t.transfer(config::system_account_name, "bbbbbbbbbbbb"_n, core_sym::from_string("300000.0000"));
        
        // сохраняем начальное состояние
        int64_t before_net_utilization = t.get_state().net.utilization;
        int64_t before_ram_utilization = t.get_state().ram.utilization;
        int64_t before_cpu_utilization = t.get_state().cpu.utilization;
        auto before_payer    = t.get_account_info("bbbbbbbbbbbb"_n);
        auto before_reserve  = t.get_account_info("eosio.power"_n);
        printf("\n");
        // арендуем ресурсы новому аккаунту
        t.check_powerup("bbbbbbbbbbbb"_n, "bbbbbbbbbbbb"_n, 1, asset::from_string("100000.0000 TST"));

        // устанавливаем контракт (потяжелее)
        t.set_code( "bbbbbbbbbbbb"_n, contracts::system_wasm() );
        t.produce_blocks();
        printf("\n");
        // сохраняем промежуточное состояние
        auto middle_payer    = t.get_account_info("bbbbbbbbbbbb"_n);
        
        auto ram_used_by_contract = middle_payer.ram_usage - before_payer.ram_usage;
        // пропускаем время
        t.produce_block(fc::days(1));
        
        // изымаем ресурсы без ошибок
        BOOST_REQUIRE_EQUAL("", t.powerupexec(config::system_account_name, 10));
        
        t.produce_blocks();
        // сохраняем финальное состояние
        int64_t after_net_utilization = t.get_state().net.utilization;
        int64_t after_ram_utilization = t.get_state().ram.utilization;
        int64_t after_cpu_utilization = t.get_state().cpu.utilization;
        auto after_payer    = t.get_account_info("bbbbbbbbbbbb"_n);
        auto after_reserve  = t.get_account_info("eosio.power"_n);
        
        printf("before_payer_ram_usage: %ld\n", before_payer.ram_usage);
        printf("before_payer_ram_limit: %ld\n", before_payer.ram);
        
        printf("middle_payer_ram_usage: %ld\n", middle_payer.ram_usage);
        printf("middle_payer_ram_limit: %ld\n", middle_payer.ram);
        
        printf("after_payer_ram_usage: %ld\n", after_payer.ram_usage);
        printf("after_payer_ram_limit: %ld\n", after_payer.ram);
        
        printf("ram_used_by_contract: %ld\n", ram_used_by_contract);
        
        int64_t ram_payment = 50000 * 10000;
        int64_t net_payment = 25000 * 10000;
        int64_t cpu_payment = 25000 * 10000;

        BOOST_REQUIRE_EQUAL(after_net_utilization, before_net_utilization);
        BOOST_REQUIRE_EQUAL(after_ram_utilization, before_ram_utilization);
        BOOST_REQUIRE_EQUAL(after_cpu_utilization, before_cpu_utilization);
        
        // повторно арендуем ресурсы новому аккаунту
        BOOST_REQUIRE_EQUAL("", t.powerup("bbbbbbbbbbbb"_n, "bbbbbbbbbbbb"_n, 1, asset::from_string("100000.0000 TST")));
        t.produce_blocks();

        int64_t after_after_net_utilization = t.get_state().net.utilization;
        int64_t after_after_ram_utilization = t.get_state().ram.utilization;
        int64_t after_after_cpu_utilization = t.get_state().cpu.utilization;
        
        auto after_after_payer    = t.get_account_info("bbbbbbbbbbbb"_n);
        auto after_after_reserve  = t.get_account_info("eosio.power"_n);
        
        printf("\n");
        printf("after_after_payer_ram_limit: %ld\n", after_after_payer.ram);
        printf("after_after_payer_ram_usage: %ld\n", after_after_payer.ram_usage);
        
        BOOST_REQUIRE_EQUAL(after_after_payer.ram, middle_payer.ram);
        
        // повторно арендуем ресурсы новому аккаунту
        BOOST_REQUIRE_EQUAL("", t.powerup("bbbbbbbbbbbb"_n, "bbbbbbbbbbbb"_n, 1, asset::from_string("100000.0000 TST")));

        // пропускаем время
        t.produce_block(fc::days(31));
        
        // изымаем ресурсы без ошибок
        BOOST_REQUIRE_EQUAL("", t.powerupexec(config::system_account_name, 10));
        
        t.produce_blocks();

        auto payer_on_finish    = t.get_account_info("bbbbbbbbbbbb"_n);
        BOOST_REQUIRE_EQUAL(payer_on_finish.ram, after_after_payer.ram_usage);
      }
   }
} // debts_tests
FC_LOG_AND_RETHROW()


BOOST_AUTO_TEST_CASE(emission_tests) try {
   {
      powerup_tester t;
      t.produce_block();

      t.create_account_with_resources("aaaaaaaaaaaa"_n, config::system_account_name, core_sym::from_string("1.0000"), false, core_sym::from_string("10000.0000"), core_sym::from_string("10000.0000"));
      t.transfer(config::system_account_name, "aaaaaaaaaaaa"_n, core_sym::from_string("10000.0000"));

      BOOST_REQUIRE_EQUAL("", t.configbw(t.make_config([&](auto& config) {
         config.powerup_days    = 1;
         config.min_powerup_fee = asset::from_string("1.0000 TST");
      })));

      
      BOOST_REQUIRE_EQUAL(t.wasm_assert_msg("Стартовый объем токенов не может быть отрицательной"), t.init_emission(-asset::from_string("1000.0000 TST"), 86400, 0.618));
      BOOST_REQUIRE_EQUAL(t.wasm_assert_msg("Продолжительность такта должна быть положительной"), t.init_emission(asset::from_string("1000.0000 TST"), 0, 0.618));
      BOOST_REQUIRE_EQUAL(t.wasm_assert_msg("Продолжительность такта не может превышать 1 год"), t.init_emission(asset::from_string("1000.0000 TST"), 365 * 86400 + 1, 0.618));
      BOOST_REQUIRE_EQUAL(t.wasm_assert_msg("Фактор эмиссии должен быть больше нуля и меньше или равен 1"), t.init_emission(asset::from_string("1000.0000 TST"), 86400, 2));
      BOOST_REQUIRE_EQUAL(t.wasm_assert_msg("Фактор эмиссии должен быть больше нуля и меньше или равен 1"), t.init_emission(asset::from_string("1000.0000 TST"), 86400, 0));
      
      BOOST_REQUIRE_EQUAL("", t.init_emission(asset::from_string("1000.0000 TST"), 86400, 0.618));

      BOOST_REQUIRE_EQUAL(1, t.get_emission_state()["tact_number"].as<uint64_t>());
      BOOST_REQUIRE_EQUAL(86400, t.get_emission_state()["tact_duration"].as<uint64_t>());
      
      asset before_supply = t.get_token_supply();
      asset before_fund_balance = t.get_balance("eosio.saving"_n);
      
      t.check_powerup("aaaaaaaaaaaa"_n, "aaaaaaaaaaaa"_n, 1, asset::from_string("500.0000 TST"));

      // 10% от взноса из числа делегатских комиссий
      BOOST_REQUIRE_EQUAL(t.get_balance("eosio.saving"_n), asset::from_string("50.0000 TST"));

      // количество токенов комиссий
      BOOST_REQUIRE_EQUAL(asset::from_string("500.0000 TST"), t.get_emission_state()["tact_fees"].as<asset>());
      
      // общий оборот без изменений
      BOOST_REQUIRE_EQUAL(asset::from_string("1000.0000 TST"), t.get_emission_state()["current_supply"].as<asset>());
      
      t.check_powerup("aaaaaaaaaaaa"_n, "aaaaaaaaaaaa"_n, 1, asset::from_string("500.0000 TST"));

      // проверяем что эмитировали 618 токенов и 100 попали в фонд от делегатов
      BOOST_REQUIRE(near(t.get_balance("eosio.saving"_n).get_amount(), asset::from_string("718.0000 TST").get_amount(), 1));

      // общий объем токенов увеличился на 618 штук
      asset after_supply = t.get_token_supply();
      BOOST_REQUIRE(near(after_supply.get_amount() - before_supply.get_amount(), asset::from_string("618.0000 TST").get_amount(), 1));


      // арендуем ресурсов еще на 1000 токенов. 
      t.check_powerup("aaaaaaaaaaaa"_n, "aaaaaaaaaaaa"_n, 1, asset::from_string("1000.0000 TST"));

      // общий оборот увеличился до 618 + 1618 + 1000
      BOOST_REQUIRE(near(asset::from_string("3236.0000 TST").get_amount(), t.get_emission_state()["current_supply"].as<asset>().get_amount(), 1));
      

      // проверяем что всё что эмитировали еще 1618 токенов и 100 попали в фонд от делегатов
      BOOST_REQUIRE(near(t.get_balance("eosio.saving"_n).get_amount(), (t.get_emission_state()["tact_emission"].as<asset>().get_amount() + asset::from_string("200.0000 TST").get_amount()), 1));
      
      // объем дополнительно выпущенных 2236 штук
      asset after_after_supply = t.get_token_supply();
      BOOST_REQUIRE(near(after_after_supply.get_amount() - before_supply.get_amount(), asset::from_string("2236.0000 TST").get_amount(), 1));

      t.produce_block(fc::days(1) + fc::milliseconds(500));

      BOOST_REQUIRE_EQUAL(2, t.get_emission_state()["tact_number"].as<uint64_t>());
      
      // такт должен переключиться
      BOOST_REQUIRE_EQUAL(2, t.get_emission_state()["tact_number"].as<uint64_t>());
      
      // эмиссия такта обнулиться
      BOOST_REQUIRE_EQUAL(asset::from_string("0.0000 TST"), t.get_emission_state()["tact_emission"].as<asset>());
      
      // текущий оборот без изменений
      BOOST_REQUIRE(near(asset::from_string("3236.0000 TST").get_amount(), t.get_emission_state()["current_supply"].as<asset>().get_amount(), 1));

      // стартовый порог эмиссии это 0.618 от общего супплая
      BOOST_REQUIRE(near(asset::from_string("2000.0000 TST").get_amount(), t.get_emission_state()["emission_start"].as<asset>().get_amount(), 10));

      
   }
} //emission_tests
FC_LOG_AND_RETHROW()


BOOST_AUTO_TEST_SUITE_END()
