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
  uint64_t registry_id;
  eosio::name creator;
  std::vector<eosio::name> actions;
  uint64_t version;
  uint64_t default_translation_id;
  std::string title;
  std::string description;
  std::string context;
  std::string model;
  
  uint64_t primary_key() const { return id; };

  uint64_t by_registry_id() const { return registry_id;}

};

typedef eosio::multi_index<
    "drafts"_n, drafts,
    eosio::indexed_by< "byregistryid"_n, eosio::const_mem_fun<drafts, uint64_t, &drafts::by_registry_id>>
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



