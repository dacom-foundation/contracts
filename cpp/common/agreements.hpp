struct [[eosio::table, eosio::contract(SOVIET)]] agreement {
  uint64_t id;
  eosio::name coopname;
  eosio::name username;
  eosio::name type; ///< Тип принимаемого документа (соответствует действию в контракте)
  uint64_t program_id; ///< Вторичный индекс используется для связи с типовой таблицей
  uint64_t draft_registry_id; ///< Шаблон документа по регистру
  uint64_t version; ///< Версия шаблона (draft), связанного с типовым документом
  document document;
  eosio::name status; ///< Статус приёма документа 
  eosio::time_point_sec updated_at; ///< Дата-время последнего обновления
  uint64_t primary_key() const { return id; };
  
  uint64_t by_status() const {return coopname.value; };
  uint64_t by_draft() const {return draft_registry_id; };  

  /**
   * @brief Возвращает ключ для индекса указанного соглашения пользователя.
   * @return uint128_t - составной ключ, включающий значения имени пользователя и идентификатор шаблона по реестру.
   */
  uint128_t by_user_and_draft() const {
    return combine_ids(username.value, draft_registry_id);
  };

};

typedef eosio::multi_index<"agreements"_n, agreement, 
  eosio::indexed_by<"bystatus"_n, eosio::const_mem_fun<agreement, uint64_t, &agreement::by_status>>,
  eosio::indexed_by<"bydraft"_n, eosio::const_mem_fun<agreement, uint64_t, &agreement::by_draft>>,
  eosio::indexed_by<"byuserdraft"_n, eosio::const_mem_fun<agreement, uint128_t, &agreement::by_user_and_draft>>
> agreements_index;
