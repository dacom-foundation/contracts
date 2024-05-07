#include <eosio/asset.hpp>
#include <eosio/contract.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>

#include "../common/consts.hpp"
#include "../common/utils.hpp"
#include "../common/counts.hpp"
#include "../common/drafts.hpp"

class [[eosio::contract(DRAFT)]] draft : public eosio::contract {

public:
  draft(eosio::name receiver, eosio::name code,
        eosio::datastream<const char *> ds)
      : eosio::contract(receiver, code, ds) {}

  void apply(uint64_t receiver, uint64_t code, uint64_t action);

  // [[eosio::action]] void verify(const checksum256& hash, const signature& sig, const public_key& pub_key);

  [[eosio::action]] void newid(uint64_t id);

  [[eosio::action]] void createdraft(uint64_t registry_id, eosio::name lang, std::string title, 
      std::string description, std::string context, std::string model, std::string translation_data);

  [[eosio::action]] void deldraft(uint64_t draft_id);

  [[eosio::action]] void createtrans(uint64_t draft_id, eosio::name lang, std::string data);

  [[eosio::action]] void deltrans(uint64_t translate_id);

  [[eosio::action]] void edittrans(uint64_t translate_id, std::string data);


  struct [[eosio::table, eosio::contract(DRAFT)]] counts : counts_base {};
};
