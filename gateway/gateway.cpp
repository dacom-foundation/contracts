#include "gateway.hpp"
#include <ctime>
#include <eosio/transaction.hpp>

using namespace eosio;

void gateway::newid(uint64_t coop_id) {
  require_auth(_gateway);
};


extern "C" {

/// The apply method implements the dispatch of events to this contract
void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  if (code == _gateway.value) {
    switch (action) {
      EOSIO_DISPATCH_HELPER(
          gateway, (newid)
      )
    }
  } else {
    if (action == "transfer"_n.value) {
      struct transfer {
        eosio::name from;
        eosio::name to;
        eosio::asset quantity;
        std::string memo;
      };

      auto op = eosio::unpack_action_data<transfer>();

      if (op.to == _gateway) {
        // вызов функции-обработчика пополнения баланса
        struct transfer {
          eosio::name from;
          eosio::name to;
          eosio::asset quantity;
          std::string memo;
        };

        auto op = eosio::unpack_action_data<transfer>();
        if (op.to == _gateway) {

          eosio::name username = eosio::name(op.memo.c_str());
          add_balance(_gateway, username, op.quantity, eosio::name(code));
        }
      }
    }
  }
};
};
