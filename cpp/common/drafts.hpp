#include <eosio/eosio.hpp>
#include <eosio/crypto.hpp>
#include <eosio/contract.hpp>
#include <eosio/system.hpp>

struct document
{
  // Чексумма документа (можем использовать SHA256)
  checksum256 hash;
  // Публичный ключ (предполагаем, что используется тип ключа EOSIO)
  public_key public_key;
  // Подпись чексуммы
  signature signature;
  // Публичные мета-данные документа
  std::string meta;
};

void verify_document_or_fail(const document &doc)
{
  // Проверка завершится прерыванием, если восстановление подписи провалится
  assert_recover_key(doc.hash, doc.signature, doc.public_key);
};

struct [[eosio::table, eosio::contract(DRAFT)]] onedraft
{
  uint64_t registry_id;
  uint64_t version;
  uint64_t default_translation_id;
  std::string title;
  std::string description;
  std::string context;
  std::string model;

  uint64_t primary_key() const { return registry_id; };

};

typedef eosio::multi_index<"drafts"_n, onedraft> drafts_index;
    
    
onedraft get_scoped_draft_by_registry_or_fail(eosio::name scope, uint64_t draft_id) {
  drafts_index drafts(_draft, scope.value);
  auto draft = drafts.find(draft_id);
  
  eosio::check(draft != drafts.end(), "Шаблон документа не найден");
  
  return *draft;
}

struct [[eosio::table, eosio::contract(DRAFT)]] translation
{
  uint64_t id;
  uint64_t draft_id;
  eosio::name lang;
  std::string data;

  uint64_t primary_key() const { return id; };
  uint64_t by_draft() const { return draft_id; };

  uint128_t by_draft_lang() const { return combine_ids(draft_id, lang.value); };
};

typedef eosio::multi_index<
    "translations"_n, translation,
    eosio::indexed_by<"bydraft"_n, eosio::const_mem_fun<translation, uint64_t, &translation::by_draft>>,
    eosio::indexed_by<
        "bydraftlang"_n,
        eosio::const_mem_fun<translation, uint128_t, &translation::by_draft_lang>>>
    translations_index;


