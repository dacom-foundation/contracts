#include <eosio/asset.hpp>
#include <eosio/contract.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>

#include "../common/common.hpp"

class [[eosio::contract(DRAFT)]] draft : public eosio::contract {

public:
  draft(eosio::name receiver, eosio::name code,
        eosio::datastream<const char *> ds)
      : eosio::contract(receiver, code, ds) {}

  void apply(uint64_t receiver, uint64_t code, uint64_t action);
  
  [[eosio::action]] void migrate();
  
  [[eosio::action]] void newid(eosio::name scope, uint64_t id);

  [[eosio::action]] void createdraft(eosio::name scope, eosio::name username, uint64_t registry_id, eosio::name lang, std::string title, 
      std::string description, std::string context, std::string model, std::string translation_data);

  [[eosio::action]] void editdraft(eosio::name scope, eosio::name username, uint64_t registry_id, std::string title, std::string description, std::string context, std::string model);
    
  [[eosio::action]] void deldraft(eosio::name scope, eosio::name username, uint64_t registry_id);

  [[eosio::action]] void createtrans(eosio::name scope, eosio::name username, uint64_t registry_id, eosio::name lang, std::string data);

  [[eosio::action]] void deltrans(eosio::name scope, eosio::name username, uint64_t translate_id);

  [[eosio::action]] void edittrans(eosio::name scope, eosio::name username, uint64_t translate_id, std::string data);

  [[eosio::action]] void upversion(eosio::name scope, eosio::name username, uint64_t registry_id);

  struct [[eosio::table, eosio::contract(DRAFT)]] counts : counts_base {};
};
