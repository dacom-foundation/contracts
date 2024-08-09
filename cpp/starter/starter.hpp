#include <eosio/asset.hpp>
#include <eosio/contract.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>

class [[eosio::contract]] starter : public eosio::contract {

public:
  starter(eosio::name receiver, eosio::name code,
              eosio::datastream<const char *> ds)
      : eosio::contract(receiver, code, ds) {}


  [[eosio::action]] void hello(std::string message);

};
