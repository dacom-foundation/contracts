/**
 * @brief 
 1. Пользователь вызывает метод sndagreement и прекрепляет подписанное заявление
    Вызываем метод первичной фиксации документа в реестре newsubmitted
 2. Администратор проверяет документ и вызывает метод acceptjoin или declinejoin
 3. При acceptjoin создаётся кошелек с документом и его подписанным хэшем
    Вызываем метод окончательной фиксации документа в реестре.
 */

[[eosio::action]] void soviet::sndagreement(eosio::name coopname, eosio::name username, eosio::name agreement_type, uint64_t program_id, uint64_t draft_registry_id, document document) {
  require_auth(username);
  
  verify_document_or_fail(document);
  
  eosio::check(agreement_type == "wallet"_n || agreement_type == ""_n, "Неверный тип соглашения");
  
  //получаем шаблон документа
  auto draft = get_draft_by_registry_or_fail(coopname, draft_registry_id);
  
  //извлекаем версию. считаем, что входящий документ текущей версии. 
  uint64_t version = draft.version;
  
  auto agreement_id = get_global_id_in_scope(_soviet, coopname, "agreements"_n);
    
  if (agreement_type == "wallet"_n) {
    //TODO проверить программу на соответстетствие по указанному program_id  
    eosio::check(program_id == 0, "Идентификатор программы должен быть указан для поиска программы");
    auto program = get_program_or_fail(coopname, program_id);  
    eosio::check(program.draft_registry_id == draft_registry_id, "Указан неверный идентификатор шаблона целевой программы");
    
    /**
    * @brief Создаём кошелёк программы для пайщика
    */
    progwallets_index progwallets(_soviet, coopname.value);
    auto coop = get_cooperative_or_fail(coopname);

    auto wallets_by_username_and_program = progwallets.template get_index<"byuserprog"_n>();
    auto username_and_program_index = combine_ids(username.value, program_id);
    auto wallet = wallets_by_username_and_program.find(username_and_program_index);

    if (wallet == wallets_by_username_and_program.end()) {
      
      progwallets.emplace(_soviet, [&](auto &b) {
        b.id = progwallets.available_primary_key();
        b.program_id = program_id;
        b.coopname = coopname;
        b.username = username;
        b.available = asset(0, coop.initial.symbol);      
        b.agreement_id = agreement_id;
      });      
    } 
    
  } else if (agreement_type == ""_n){
    eosio::check(program_id == 0, "Указание вторичного идентификатора не требуется для простых соглашений");
  };
  
  
  
  /**
   * @brief Здесь надо проверить соглашение по ключу регистра шаблона для пользователя. 
   * Если есть и непринятое или отклоненное - редактировать. Если есть и принятое - отказать. 
   * Если нет - создать. 
   */
  agreements_index agreements(_soviet, coopname.value);
  auto agreements_by_username_and_draft = agreements.template get_index<"byuserdraft"_n>();
  auto index = combine_ids(username.value, draft_registry_id);

  auto agreement = agreements_by_username_and_draft.find(index);

  if (agreement == agreements_by_username_and_draft.end()) {
    
    agreements.emplace(_soviet, [&](auto &row){
      row.id = agreement_id;
      row.coopname = coopname;
      row.status = "pending"_n;
      row.type = agreement_type;
      row.version = version;
      row.draft_registry_id = draft_registry_id;
      row.program_id = program_id;
      row.username = username;
      row.document = document;
      row.updated_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    });    
  } else {
    
    eosio::check(agreement -> status != "confirmed"_n, "Соглашение уже принято");
    
    agreements_by_username_and_draft.modify(agreement, username, [&](auto &row){
      row.status = "pending"_n;
      row.program_id = program_id;
      row.version = version;
      row.document = document;
      row.updated_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    });
    
  }
  
  
  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "newsubmitted"_n,
    std::make_tuple(coopname, username, agreement_type, 0, document)
  ).send();
  
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
  
  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "newresolved"_n,
    std::make_tuple(coopname, username, indoc -> type, 0, indoc -> document)
  ).send();
  
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