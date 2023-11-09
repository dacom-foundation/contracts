#include <eosio/asset.hpp>
#include <eosio/contract.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>

#include "../common/consts.hpp"
#include "../common/utils.hpp"
#include "../common/coops.hpp"
#include "../common/counts.hpp"
#include "../common/permissions.hpp"
#include "../common/balances.hpp"

class [[eosio::contract(GATEWAY)]] gateway : public eosio::contract {

public:
  gateway(eosio::name receiver, eosio::name code,
      eosio::datastream<const char *> ds)
      : eosio::contract(receiver, code, ds) {}

  [[eosio::action]] void newid(uint64_t coop_id);

  void apply(uint64_t receiver, uint64_t code, uint64_t action);

  struct [[eosio::table, eosio::contract(ANO)]] counts : counts_base {};

  struct [[eosio::table, eosio::contract(ANO)]] balances : balances_base {};
};
