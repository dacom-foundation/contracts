#include "draft.hpp"
#include <ctime>
#include <eosio/transaction.hpp>
using namespace eosio;

void draft::newid(uint64_t id) { require_auth(_draft); };

void draft::edittrans(eosio::name creator, uint64_t translate_id, std::string data) {
  require_auth(creator);

  translations_index translations(_draft, _draft.value);
  auto trans = translations.find(translate_id);
  eosio::check(trans != translations.end(), "Перевод не найден");
  eosio::check(trans -> creator == creator, "Только создатель перевода может его опубликовать"); 
  
  translations.modify(trans, _draft, [&](auto &t){
    t.data = data;
  });

};

void draft::publishtrans(eosio::name creator, uint64_t translate_id) {
  require_auth(creator);

  translations_index translations(_draft, _draft.value);
  auto trans = translations.find(translate_id);
  eosio::check(trans != translations.end(), "Перевод не найден");
  eosio::check(trans -> creator == creator, "Только создатель перевода может его опубликовать"); 
  
  translations.modify(trans, _draft, [&](auto &t){
    t.is_published = true;
  });

};

void draft::approvetrans(uint64_t translate_id) {
  require_auth(_draft);

  translations_index translations(_draft, _draft.value);
  auto trans = translations.find(translate_id);
  eosio::check(trans != translations.end(), "Перевод не найден");

  translations.modify(trans, _draft, [&](auto &t){
    t.is_approved = true;
  });

};


void draft::deltrans(eosio::name creator, uint64_t translate_id) {
  require_auth(creator);

  translations_index translations(_draft, _draft.value);
  
  auto trans = translations.find(translate_id);
  eosio::check(trans != translations.end(), "Объект с переводом не найден");
  eosio::check(trans -> creator == creator, "Только создатель перевода может его удалить");
  eosio::check(trans -> is_published == false, "Только не опубликованные переводы могут быть удалены");

  translations.erase(trans);

};

void draft::createtrans(eosio::name creator, uint64_t draft_id, eosio::name lang, std::string data) {
  require_auth(creator);

  drafts_index drafts(_draft, _draft.value);
  auto draft = drafts.find(draft_id);
  eosio::check(draft != drafts.end(), "Документ не найден");
  eosio::check(creator == draft -> creator, "Только создатель документа может опубликовать перевод");

  translations_index translations(_draft, _draft.value);
  auto trans_index_by_draft_and_lang = translations.template get_index<"bydraftlang"_n>();
  auto trans_combined_index = combine_ids(draft_id, lang.value);
  auto trans = trans_index_by_draft_and_lang.find(trans_combined_index);
  eosio::check(trans == trans_index_by_draft_and_lang.end(), "Перевод уже создан для документа");

  uint64_t translation_id = get_global_id(_draft, "translation"_n);

  translations.emplace(creator, [&](auto &d){
    d.id = translation_id;
  }); 
};

void draft::deldraft(eosio::name creator, uint64_t draft_id) {
  require_auth(creator);

  drafts_index drafts(_draft, _draft.value);

  auto draft = drafts.find(draft_id);

  eosio::check(draft != drafts.end(), "Шаблон не найден");
  eosio::check(draft -> is_published == false, "Только не опубликованные документы могут быть удалены");
  eosio::check(draft -> creator == creator, "Только создатель документа может его удалить");
  
  drafts.erase(draft);
};


void draft::approvedraft(uint64_t draft_id) {
  require_auth(_draft);

  drafts_index drafts(_draft, _draft.value);
  auto draft = drafts.find(draft_id);
  eosio::check(draft != drafts.end(), "Шаблон не найден");
  drafts.modify(draft, _draft, [&](auto &d){
    d.is_approved = true;
  });

};


void draft::standardize(uint64_t draft_id){
  require_auth(_draft);

  drafts_index drafts(_draft, _draft.value);
  auto draft = drafts.find(draft_id);
  eosio::check(draft != drafts.end(), "Шаблон не найден");
  drafts.modify(draft, _draft, [&](auto &d){
    d.is_standartized = true;
  });  
};


void draft::publishdraft(eosio::name creator, uint64_t draft_id) {
  require_auth(creator);

  drafts_index drafts(_draft, _draft.value);

  auto draft = drafts.find(draft_id);

  eosio::check(draft != drafts.end(), "Шаблон не найден");
  eosio::check(draft -> creator == creator, "Только создатель документа может его опубликовать");

  auto drafts_index_by_contact_and_action = drafts.template get_index<"byactactive"_n>();
  auto draft_combined_index = combine_ids(draft -> action_name.value, 1);
  auto existed_draft = drafts_index_by_contact_and_action.find(draft_combined_index);
  
  if (existed_draft != drafts_index_by_contact_and_action.end()){
    drafts_index_by_contact_and_action.modify(existed_draft, creator, [&](auto &d){
      d.is_activated = false;
    });
  };

  drafts.modify(draft, creator, [&](auto &d) {
    d.is_published = true;
    d.is_activated = true;
  });
};



void draft::editdraft(eosio::name creator, uint64_t draft_id, std::string title, std::string description, std::string context, std::string model){
  require_auth(creator);

  drafts_index drafts(_draft, _draft.value);

  auto draft = drafts.find(draft_id);

  eosio::check(draft != drafts.end(), "Шаблон не найден");
  eosio::check(draft -> creator == creator, "Только создатель документа может его отредактировать");
  eosio::check(draft -> is_published == false, "Только не опубликованные документы могут быть отредактированы");

  drafts.modify(draft, draft -> creator, [&](auto &d){
    d.title = title;
    d.description = description;
    d.context = context;
    d.model = model;
  });

};


void draft::createdraft(eosio::name creator, eosio::name action_name, uint64_t version, eosio::name lang, std::string title, 
                    std::string description, std::string context, std::string model, std::string translation_data) {

  require_auth(creator);

  drafts_index drafts(_draft, _draft.value);
  translations_index translations(_draft, _draft.value);

  auto drafts_index_by_contact_and_action = drafts.template get_index<"byactversion"_n>();
  auto draft_combined_index = combine_ids(action_name.value, version);
  auto draft = drafts_index_by_contact_and_action.find(draft_combined_index);
  eosio::check(draft == drafts_index_by_contact_and_action.end(), "Шаблон уже создан с этой версией");

  uint64_t draft_id = get_global_id(_draft, "draft"_n);
  uint64_t translation_id = get_global_id(_draft, "translation"_n);
  
  drafts.emplace(_draft, [&](auto &d) {
    d.id = draft_id;
    d.creator = creator;
    d.action_name = action_name;
    d.version = version;
    d.default_translation_id = translation_id;
    d.title = title;
    d.description = description;
    d.context = context;
    d.model = model;
    d.is_activated = false;
    d.is_approved = false;
    d.is_standartized = false;
  });

  translations.emplace(_draft, [&](auto &t) {
    t.id = translation_id;
    t.creator = creator;
    t.draft_id = draft_id;
    t.lang = lang;
    t.data = translation_data;
    t.is_approved = false;
    t.is_published  = false;
  });

  action(permission_level{_draft, "active"_n}, _draft, "newid"_n,
     std::make_tuple(draft_id))
  .send();
};


extern "C" {

/// The apply method implements the dispatch of events to this contract
void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  if (code == _draft.value) {
    switch (action) { EOSIO_DISPATCH_HELPER(draft, 
      (createdraft)(editdraft)(publishdraft)(standardize)(approvedraft)(deldraft)(createtrans)
      (deltrans)(approvetrans)(publishtrans)(edittrans)
      ) }
  } else {
    if (action == "transfer"_n.value) {
      struct transfer {
        eosio::name from;
        eosio::name to;
        eosio::asset quantity;
        std::string memo;
      };

      auto op = eosio::unpack_action_data<transfer>();

      if (op.to == _draft) {
        // вызов функции-обработчика пополнения баланса
       
      }
    }
  }
};
};
