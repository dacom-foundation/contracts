#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <eosio/chain/resource_limits.hpp>

#include <boost/test/unit_test.hpp>
#include "eosio.system_tester.hpp"
#include <fc/variant_object.hpp>
#include <fstream>

using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;

    
class coopenomics_tester : eosio_system::eosio_system_tester {
public:
    
   action_result push_action_to_fund( const account_name& signer, const action_name &name, const variant_object &data, bool auth = true ) {
      string action_type_name = fund_abi_ser.get_action_type(name);

      action act;
      act.account = _fund;
      act.name = name;
      act.data = fund_abi_ser.variant_to_binary( action_type_name, data, abi_serializer::create_yield_function(abi_serializer_max_time) );

      account_name auth_account = auth ? signer : "bob111111111"_n;
      return base_tester::push_action( std::move(act), auth_account.to_uint64_t() );
  }


    fc::variant get_cooperative_wallet( name coopname ) {
      // vector<char> get_row_by_account( name code, name scope, name table, const account_name& act ) const;
      vector<char> data = get_row_by_account( _fund, coopname, "coopwallet"_n, eosio::chain::name(0) );
      return data.empty() ? fc::variant() : fund_abi_ser.binary_to_variant( "coopwallet", data, abi_serializer::create_yield_function(abi_serializer_max_time) );
   }
  
    fc::variant get_accumulation_wallet( name coopname, uint64_t id ) {
      vector<char> data = get_row_by_account( _fund, coopname, "accfunds"_n, eosio::chain::name(id) );
      return data.empty() ? fc::variant() : fund_abi_ser.binary_to_variant( "accfund", data, abi_serializer::create_yield_function(abi_serializer_max_time) );
   }
    
    fc::variant get_expense_wallet( name coopname, uint64_t id ) {
      vector<char> data = get_row_by_account( _fund, coopname, "expfunds"_n, eosio::chain::name(id) );
      return data.empty() ? fc::variant() : fund_abi_ser.binary_to_variant( "expfund", data, abi_serializer::create_yield_function(abi_serializer_max_time) );
   }
   
    action_result addinitial(const name& signer, const name& coopname, const asset& quantity) {
      return push_action_to_fund(signer, "addinitial"_n, mvo()("coopname", coopname)("quantity", quantity));
   }
   
   action_result subinitial(const name& signer, const name& coopname, const asset& quantity) {
      return push_action_to_fund(signer, "subinitial"_n, mvo()("coopname", coopname)("quantity", quantity));
   }
  


    // Ваши дополнительные методы или поля
    void my_custom_method() {
        // Реализация вашего метода
    }
};
