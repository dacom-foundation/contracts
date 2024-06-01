#include "ano.hpp"
#include <ctime>
#include <eosio/transaction.hpp>

using namespace eosio;

void ano::newid(uint64_t coop_id) {
  require_auth(_ano);
};


extern "C" {

/// The apply method implements the dispatch of events to this contract
void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  if (code == _ano.value) {
    switch (action) {
      EOSIO_DISPATCH_HELPER(
          ano, (newid)
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

      if (op.to == _ano) {
        // вызов функции-обработчика пополнения баланса
        struct transfer {
          eosio::name from;
          eosio::name to;
          eosio::asset quantity;
          std::string memo;
        };

        auto op = eosio::unpack_action_data<transfer>();
        if (op.to == _ano) {

        }
      }
    }
  }
};
};
