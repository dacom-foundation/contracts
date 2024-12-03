#include <eosio/asset.hpp>
#include <eosio/contract.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>

#include "../common/common.hpp"

/**
 *  \ingroup public_contracts
 *
 *  @brief  Класс `branch` управляет кооперативными участками.
 */
class [[eosio::contract(BRANCH)]] branch : public eosio::contract
{

public:
  branch(eosio::name receiver, eosio::name code,
              eosio::datastream<const char *> ds)
      : eosio::contract(receiver, code, ds) {}

  [[eosio::action]] void init();
  [[eosio::action]] void migrate();
  
  [[eosio::action]] void createbranch(eosio::name coopname, eosio::name braname, eosio::name trustee);
  [[eosio::action]] void editbranch(eosio::name coopname, eosio::name braname, eosio::name trustee);
  [[eosio::action]] void deletebranch(eosio::name coopname, eosio::name braname);
  [[eosio::action]] void addtrusted(eosio::name coopname, eosio::name braname, eosio::name trusted);
  [[eosio::action]] void deltrusted(eosio::name coopname, eosio::name braname, eosio::name trusted);
  
  
};
