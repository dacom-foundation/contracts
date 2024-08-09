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
      
  my_custom_method(); // Вызов вашего метода
  BOOST_REQUIRE_EQUAL( true, 1 < 3 );
      
} FC_LOG_AND_RETHROW()
        
BOOST_AUTO_TEST_SUITE_END()

