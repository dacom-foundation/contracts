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

#include "coopenomics_tester.hpp"

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;
using mvo = fc::mutable_variant_object;

struct fund_tester : coopenomics_tester {
public:
    fund_tester() {
      //constructor
    }
    
};


BOOST_AUTO_TEST_SUITE(fund_tests) 
    
BOOST_FIXTURE_TEST_CASE(config_tests, fund_tester) try {
      
  // ilog("test!: ${wallet}", ("wallet", wallet));
  // printf("wallet: %ld\n", wallet);
  // printf(wallet);
  BOOST_REQUIRE_EQUAL(get_cooperative_wallet(_provider)["circulating_account"]["available"].as<asset>(), asset::from_string("0.0000 TRUB"));
  BOOST_REQUIRE_EQUAL(get_cooperative_wallet(_provider)["initial_account"]["available"].as<asset>(), asset::from_string("0.0000 TRUB"));
  BOOST_REQUIRE_EQUAL(get_cooperative_wallet(_provider)["accumulative_account"]["available"].as<asset>(), asset::from_string("0.0000 TRUB"));
  BOOST_REQUIRE_EQUAL(get_cooperative_wallet(_provider)["accumulative_expense_account"]["available"].as<asset>(), asset::from_string("0.0000 TRUB"));
  
  BOOST_REQUIRE_EQUAL(get_cooperative_wallet(_provider)["circulating_account"]["withdrawed"].as<asset>(), asset::from_string("0.0000 TRUB"));
  BOOST_REQUIRE_EQUAL(get_cooperative_wallet(_provider)["initial_account"]["withdrawed"].as<asset>(), asset::from_string("0.0000 TRUB"));
  BOOST_REQUIRE_EQUAL(get_cooperative_wallet(_provider)["accumulative_account"]["withdrawed"].as<asset>(), asset::from_string("0.0000 TRUB"));
  BOOST_REQUIRE_EQUAL(get_cooperative_wallet(_provider)["accumulative_expense_account"]["withdrawed"].as<asset>(), asset::from_string("0.0000 TRUB"));
  
  //неделимый фонд
  BOOST_REQUIRE_EQUAL(get_accumulation_wallet(_provider, 1)["coopname"].as<name>(), _provider);
  BOOST_REQUIRE_EQUAL(get_accumulation_wallet(_provider, 1)["contract"].as<name>(), ""_n);
  BOOST_REQUIRE_EQUAL(get_accumulation_wallet(_provider, 1)["available"].as<asset>(), asset::from_string("0.0000 TRUB"));
  BOOST_REQUIRE_EQUAL(get_accumulation_wallet(_provider, 1)["withdrawed"].as<asset>(), asset::from_string("0.0000 TRUB"));
  BOOST_REQUIRE_EQUAL(get_accumulation_wallet(_provider, 1)["name"].as<std::string>(), std::string("Неделимый фонд"));

  
      
} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(addinitial_action, fund_tester) try {
  
  const asset quantity      = asset::from_string("100.0000 TRUB");
  //пополняем счет вступительных взносов  
  BOOST_REQUIRE_EQUAL("", addinitial(_gateway, _provider, quantity));
  BOOST_REQUIRE_EQUAL(get_cooperative_wallet(_provider)["initial_account"]["available"].as<asset>(), quantity);
    
  BOOST_REQUIRE_EQUAL("assertion failure with message: Недостаточно прав доступа", addinitial("bob111111111"_n, _provider, quantity));
  

} FC_LOG_AND_RETHROW()



        
BOOST_AUTO_TEST_SUITE_END()

