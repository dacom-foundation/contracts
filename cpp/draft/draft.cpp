#include "draft.hpp"
#include <ctime>
#include <eosio/transaction.hpp>
#include <eosio/crypto.hpp>
using namespace eosio;

[[eosio::action]]
void draft::migrate() {
  require_auth(_draft);
}

void draft::newid(eosio::name scope, uint64_t id) { require_auth(_draft); };


name get_payer_and_check_auth_in_scope(eosio::name scope, eosio::name username, eosio::name action){
  eosio::name payer;
  
  if (scope == _draft) {
    require_auth(_system);
    payer = _system;
  }
  else {
    get_cooperative_or_fail(scope);
    check_auth_or_fail(scope, username, action);
    payer = username;
  };
  
  return payer;
}

void draft::createdraft(eosio::name scope, eosio::name username, uint64_t registry_id, eosio::name lang, std::string title, 
                    std::string description, std::string context, std::string model, std::string translation_data) {
  
  eosio::name payer = get_payer_and_check_auth_in_scope(scope, username, "createdraft"_n);
    
  drafts_index drafts(_draft, scope.value);

  // Проверка существующих записей с тем же registry_id
  auto exist = drafts.find(registry_id);
  eosio::check(exist == drafts.end(), "Уже существует шаблон с тем же идентификатором. Обновите его.");
  
  // Создание нового шаблона
  uint64_t translation_id = get_global_id(_draft, "translation"_n);
  
  drafts.emplace(payer, [&](auto &d) {
    d.registry_id = registry_id;
    d.version = 1;
    d.default_translation_id = translation_id;
    d.title = title;
    d.description = description;
    d.context = context;
    d.model = model;
  });

 
  translations_index translations(_draft, scope.value);
 
  translations.emplace(payer, [&](auto &t) {
    t.id = translation_id;
    t.draft_id = registry_id;
    t.lang = lang;
    t.data = translation_data;
  });

  // Отправка действия для обновления идентификаторов
  action(permission_level{_draft, "active"_n}, _draft, "newid"_n,
     std::make_tuple(scope, registry_id))
  .send();
};



void draft::editdraft(eosio::name scope, eosio::name username, uint64_t registry_id, std::string title, std::string description, std::string context, std::string model){
  eosio::name payer = get_payer_and_check_auth_in_scope(scope, username, "editdraft"_n);
  
  drafts_index drafts(_draft, scope.value);
  auto exist = drafts.find(registry_id);
  
  eosio::check(exist != drafts.end(), "Шаблон не найден");

  drafts.modify(exist, payer, [&](auto &d){
    d.version = exist -> version + 1;
    d.title = title;
    d.description = description;
    d.context = context;
    d.model = model;
  });

}


void draft::deldraft(eosio::name scope, eosio::name username, uint64_t registry_id) {
  eosio::name payer = get_payer_and_check_auth_in_scope(scope, username, "deldraft"_n);
  
  drafts_index drafts(_draft, _draft.value);

  auto draft = drafts.find(registry_id);
  eosio::check(draft != drafts.end(), "Шаблон не найден");
  
  drafts.erase(draft);
};


void draft::createtrans(eosio::name scope, eosio::name username, uint64_t registry_id, eosio::name lang, std::string data) {
  eosio::name payer = get_payer_and_check_auth_in_scope(scope, username, "createtrans"_n);
  
  drafts_index drafts(_draft, scope.value);
  auto draft = drafts.find(registry_id);
  eosio::check(draft != drafts.end(), "Документ не найден");
  
  translations_index translations(_draft, scope.value);
  
  auto trans_index_by_draft_and_lang = translations.template get_index<"bydraftlang"_n>();
  
  auto trans_combined_index = combine_ids(registry_id, lang.value);
  auto trans = trans_index_by_draft_and_lang.find(trans_combined_index);
  eosio::check(trans == trans_index_by_draft_and_lang.end(), "Перевод уже создан для документа");

  uint64_t translation_id = get_global_id(_draft, "translation"_n);

  translations.emplace(payer, [&](auto &d){
    d.id = translation_id;
  }); 
};


void draft::edittrans(eosio::name scope, eosio::name username, uint64_t translate_id, std::string data) {
  eosio::name payer = get_payer_and_check_auth_in_scope(scope, username, "edittrans"_n);
  
  translations_index translations(_draft, scope.value);
  auto trans = translations.find(translate_id);
  eosio::check(trans != translations.end(), "Перевод не найден");
  
  translations.modify(trans, _system, [&](auto &t){
    t.data = data;
  });

};


void draft::deltrans(eosio::name scope, eosio::name username, uint64_t translate_id) {
  eosio::name payer = get_payer_and_check_auth_in_scope(scope, username, "deltrans"_n);
  
  translations_index translations(_draft, scope.value);
  
  auto trans = translations.find(translate_id);
  eosio::check(trans != translations.end(), "Объект с переводом не найден");
  
  translations.erase(trans);

};
