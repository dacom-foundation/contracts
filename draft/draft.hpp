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
#include "../common/draft.hpp"

class [[eosio::contract(DRAFT)]] draft : public eosio::contract {

public:
  draft(eosio::name receiver, eosio::name code,
        eosio::datastream<const char *> ds)
      : eosio::contract(receiver, code, ds) {}

  [[eosio::action]] void newid(uint64_t id);

  [[eosio::action]] void createdraft(eosio::name creator, eosio::name action_name, uint64_t version, eosio::name lang, std::string title, 
                    std::string description, std::string context, std::string translation_data);

  [[eosio::action]] void editdraft(eosio::name creator, uint64_t draft_id, std::string title, std::string description, std::string context);

  [[eosio::action]] void publishdraft(eosio::name creator, uint64_t draft_id);

  [[eosio::action]] void standardize(uint64_t draft_id);

  [[eosio::action]] void approvedraft(uint64_t draft_id);

  [[eosio::action]] void deldraft(eosio::name creator, uint64_t draft_id);

  [[eosio::action]] void createtrans(eosio::name creator, uint64_t draft_id, eosio::name lang, std::string data);

  [[eosio::action]] void deltrans(eosio::name creator, uint64_t translate_id);

  [[eosio::action]] void approvetrans(uint64_t translate_id);

  [[eosio::action]] void publishtrans(eosio::name creator, uint64_t translate_id);

  [[eosio::action]] void edittrans(eosio::name creator, uint64_t translate_id, std::string data);


  void apply(uint64_t receiver, uint64_t code, uint64_t action);

  struct [[eosio::table, eosio::contract(DRAFT)]] counts : counts_base {};
};
