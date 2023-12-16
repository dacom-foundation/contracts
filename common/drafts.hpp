#include <eosio/eosio.hpp>
#include <eosio/crypto.hpp>
#include <eosio/contract.hpp>
#include <eosio/system.hpp>


struct document {
  // Чексумма документа (можем использовать SHA256)
  std::string hash;
  // Публичный ключ (предполагаем, что используется тип ключа EOSIO)
  std::string pkey;
  // Подпись чексуммы
  std::string sign; 
  std::string meta; 
};


void verify(const document& doc) {
    // Проверка подписи
    
    // assert_recover_key(doc.hash, doc.sign, doc.pkey);

    // Если подпись недействительна, транзакция завершится ошибкой
};


struct [[eosio::table, eosio::contract(DRAFT)]] drafts {
  uint64_t id;
  eosio::name creator;
  eosio::name action_name;
  uint64_t version;
  uint64_t default_translation_id;
  std::string title;
  std::string description;
  std::string context;
  std::string model;
  
  bool is_published;
  bool is_activated;
  bool is_approved;
  bool is_standartized;

  uint64_t primary_key() const { return id; };

  uint64_t by_action() const { return action_name.value; };
  
  uint128_t by_action_version() const {
    return combine_ids(action_name.value, version);
  };

  uint128_t by_action_active() const {
    return combine_ids(action_name.value, is_activated == true ? 1 : 0);
  };

};

typedef eosio::multi_index<
    "drafts"_n, drafts,
    eosio::indexed_by<"byaction"_n, eosio::const_mem_fun<drafts, uint64_t, &drafts::by_action>>,
    eosio::indexed_by< "byactversion"_n, eosio::const_mem_fun<drafts, uint128_t, &drafts::by_action_version>>,
    eosio::indexed_by< "byactactive"_n, eosio::const_mem_fun<drafts, uint128_t, &drafts::by_action_active>>
> drafts_index;


struct [[eosio::table, eosio::contract(DRAFT)]] translations {
  uint64_t id;
  eosio::name creator;
  uint64_t draft_id;
  eosio::name lang;
  std::string data;
  bool is_published;
  bool is_approved;
  
  uint64_t primary_key() const { return id; };
  uint64_t by_draft() const { return draft_id;};

  uint128_t by_draft_lang() const { return combine_ids(draft_id, lang.value);};

};

typedef eosio::multi_index<
    "translations"_n, translations,
    eosio::indexed_by<"bydraft"_n, eosio::const_mem_fun<translations, uint64_t, &translations::by_draft>>,
    eosio::indexed_by<
        "bydraftlang"_n,
        eosio::const_mem_fun<translations, uint128_t, &translations::by_draft_lang>>> translations_index;



