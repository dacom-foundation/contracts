/**
 * @brief Метод создания кооперативного соглашения. Связывает шаблон и программу. 
 * Таблица используется для хранения типов соглашений, по которым на фронте проверяется наличие подписанного соглашения по версиям.
 * Отсутствие подписи последней версии соглашения не блокирует деятельность по контрактам. Полное отсутствие подписанных соглашений не допускает использование программы. 
 * @param coopname 
 * @param administrator 
 * @param type 
 * @param draft_id 
 * @param program_id 
 */
[[eosio::action]] void soviet::makecoagreem(eosio::name coopname, eosio::name administrator, eosio::name type, uint64_t draft_id, uint64_t program_id){
  check_auth_or_fail(coopname, administrator, "makecoagreem"_n);
  
  coagreements_index coagreements(_soviet, coopname.value);
  auto exist = coagreements.find(type.value);
  eosio::check(exist == coagreements.end(), "Соглашение указанного типа уже существует");
  
  auto draft = get_scoped_draft_by_registry_or_fail(_draft, draft_id);
  
  if (program_id > 0)
    auto program = get_program_or_fail(coopname, program_id); 
  
  coagreements.emplace(_soviet, [&](auto &row){
    row.type = type;
    row.coopname = coopname;
    row.program_id = program_id;
    row.draft_id = draft_id;
  });
   
};


/**
 * @brief 
 1. Пользователь вызывает метод sndagreement и прекрепляет подписанное заявление
    Вызываем метод первичной фиксации документа в реестре newsubmitted
 2. Администратор проверяет документ и вызывает метод acceptjoin или declinejoin
 3. При acceptjoin создаётся кошелек с документом и его подписанным хэшем
    Вызываем метод окончательной фиксации документа в реестре.
 */
[[eosio::action]] void soviet::sndagreement(eosio::name coopname, eosio::name administrator, eosio::name username, eosio::name agreement_type, document document) {
  
  eosio::check(has_auth(username) || has_auth(administrator), "Недостаточно прав доступа");
  
  auto coop = get_cooperative_or_fail(coopname);
    
  if (has_auth(administrator)) {
    check_auth_or_fail(coopname, administrator, "createboard"_n);
  }
  
  verify_document_or_fail(document);
  
  auto coagreement = get_coagreement_or_fail(coopname, agreement_type);
  
  //получаем шаблон документа
  auto draft = get_scoped_draft_by_registry_or_fail(_draft, coagreement.draft_id);
  
  //извлекаем версию. считаем, что входящий документ текущей версии. 
  uint64_t version = draft.version;
  
  auto agreement_id = get_global_id_in_scope(_soviet, coopname, "agreements"_n);
  
  if (coagreement.program_id > 0) {
    
    auto program = get_program_or_fail(coopname, coagreement.program_id);  
    eosio::check(program.draft_id == coagreement.draft_id, "Указан неверный идентификатор шаблона целевой программы");
    
    /**
    * @brief Создаём кошелёк программы для пайщика если его ещё нет
    */
    
    progwallets_index progwallets(_soviet, coopname.value);
    auto wallets_by_username_and_program = progwallets.template get_index<"byuserprog"_n>();
    auto username_and_program_index = combine_ids(username.value, coagreement.program_id);
    auto wallet = wallets_by_username_and_program.find(username_and_program_index);
  
    if (wallet == wallets_by_username_and_program.end()) {
      
      progwallets.emplace(_soviet, [&](auto &b) {
        b.id = progwallets.available_primary_key();
        b.program_id = coagreement.program_id;
        b.coopname = coopname;
        b.username = username;
        b.available = asset(0, coop.initial.symbol);      
        b.agreement_id = agreement_id;
      });      
    } 
    
  } 
  
  /**
   * @brief Здесь надо проверить соглашение по ключу регистра шаблона для пользователя. 
   * Если есть и непринятое или отклоненное - редактировать. Если есть и принятое - отказать. 
   * Если нет - создать. 
   */
  agreements_index agreements(_soviet, coopname.value);
  auto agreements_by_username_and_draft = agreements.template get_index<"byuserdraft"_n>();
  auto index = combine_ids(username.value, coagreement.draft_id);

  auto agreement = agreements_by_username_and_draft.find(index);

  if (agreement == agreements_by_username_and_draft.end()) {
    
    agreements.emplace(_soviet, [&](auto &row){
      row.id = agreement_id;
      row.coopname = coopname;
      row.status = ""_n;
      row.type = agreement_type;
      row.version = version;
      row.draft_id = coagreement.draft_id;
      row.program_id = coagreement.program_id;
      row.username = username;
      row.document = document;
      row.updated_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    });    
  } else {
    
    eosio::check(agreement -> status != "confirmed"_n, "Соглашение уже принято");
    
    agreements_by_username_and_draft.modify(agreement, _soviet, [&](auto &row){
      row.status = ""_n;
      row.program_id = coagreement.program_id;
      row.version = version;
      row.document = document;
      row.updated_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    });
    
  }
  
  
  // action(
  //   permission_level{ _soviet, "active"_n},
  //   _soviet,
  //   "newsubmitted"_n,
  //   std::make_tuple(coopname, username, agreement_type, uint64_t(0), document)
  // ).send();
  
};


[[eosio::action]] void soviet::confirmagree(eosio::name coopname, eosio::name administrator, eosio::name username, uint64_t agreement_id) {
  check_auth_or_fail(coopname, administrator, "confirmagree"_n);
  
  
  agreements_index agreements(_soviet, coopname.value);
  auto indoc = agreements.find(agreement_id);
  
  bool is = is_participant_of_cpp_by_program_id(coopname, username, indoc -> program_id);
  eosio::check(!is, "Участник уже принимает участие данной целевой программы");
  
  eosio::check(indoc != agreements.end(), "Документ не найден");
  eosio::check(indoc -> username == username, "Имя пользователя не соответствует документу");
  
  agreements.modify(indoc, administrator, [&](auto &d) { 
    d.status = "confirmed"_n;
  });
  
  // action(
  //   permission_level{ _soviet, "active"_n},
  //   _soviet,
  //   "newresolved"_n,
  //   std::make_tuple(coopname, username, indoc -> type, 0, indoc -> document)
  // ).send();
  
}

[[eosio::action]] void soviet::declineagree(eosio::name coopname, eosio::name administrator, eosio::name username, uint64_t agreement_id, std::string comment){
  check_auth_or_fail(coopname, administrator, "editprog"_n);
  
  agreements_index agreements(_soviet, coopname.value);
  auto indoc = agreements.find(agreement_id);
  
  eosio::check(indoc != agreements.end(), "Документ не найден");
  eosio::check(indoc -> username == username, "Имя пользователя не соответствует документу");
  
  agreements.modify(indoc, administrator, [&](auto &d) { 
    d.status = "declined"_n;
  });
}