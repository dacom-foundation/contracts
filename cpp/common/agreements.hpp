struct [[eosio::table, eosio::contract(SOVIET)]] coagreement {
  eosio::name type;  ///< wallet | user | signature | privary | ...
  eosio::name coopname;  
  uint64_t program_id;
  uint64_t draft_id;
  
  uint64_t primary_key() const { return type.value;};
    
};

typedef eosio::multi_index<"coagreements"_n, coagreement> coagreements_index;


coagreement get_coagreement_or_fail(eosio::name coopname, eosio::name type) {
  coagreements_index coagreements(_soviet, coopname.value);
  auto coagreement = coagreements.find(type.value);
  eosio::check(coagreement != coagreements.end(), "Соглашение указанного типа не найдено");
  
  return *coagreement;
};


struct [[eosio::table, eosio::contract(SOVIET)]] agreement {
  uint64_t id;
  eosio::name coopname;
  eosio::name username;
  eosio::name type; ///< Тип принимаемого документа (соответствует действию в контракте)
  uint64_t program_id; ///< Вторичный индекс используется для связи с типовой таблицей
  uint64_t draft_id; ///< Шаблон документа по регистру
  uint64_t version; ///< Версия шаблона (draft), связанного с типовым документом
  document document;
  eosio::name status; ///< Статус приёма документа 
  eosio::time_point_sec updated_at; ///< Дата-время последнего обновления
  uint64_t primary_key() const { return id; };
  uint64_t by_username() const { return username.value; };
  uint64_t by_status() const {return coopname.value; };
  uint64_t by_draft() const {return draft_id; };  

  /**
   * @brief Возвращает ключ для индекса указанного соглашения пользователя.
   * @return uint128_t - составной ключ, включающий значения имени пользователя и идентификатор шаблона по реестру.
   */
  uint128_t by_user_and_draft() const {
    return combine_ids(username.value, draft_id);
  };

};

typedef eosio::multi_index<"agreements"_n, agreement, 
  eosio::indexed_by<"byusername"_n, eosio::const_mem_fun<agreement, uint64_t, &agreement::by_username>>,
  eosio::indexed_by<"bystatus"_n, eosio::const_mem_fun<agreement, uint64_t, &agreement::by_status>>,
  eosio::indexed_by<"bydraft"_n, eosio::const_mem_fun<agreement, uint64_t, &agreement::by_draft>>,
  eosio::indexed_by<"byuserdraft"_n, eosio::const_mem_fun<agreement, uint128_t, &agreement::by_user_and_draft>>
> agreements_index;
