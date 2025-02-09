using namespace eosio;


/**
\ingroup public_actions
\brief Метод создания целевой программы
*
* Этот метод позволяет председателю совета создать новую программу.
*
* @param coopname Имя кооператива
* @param chairman Имя председателя совета
* @param name Имя программы
* @param announce Объявление о программе
* @param description Описание программы
* @param preview Предварительный просмотр
* @param images Изображения для программы
* @param website Веб-сайт программы
* @param initial Вступительный взнос
* @param minimum Минимальный взнос
* @param membership Членский взнос
* @param period Периодичность
* @param category Категория
*
* @note Авторизация требуется от аккаунта: @p chairman
*/
void soviet::createprog(eosio::name coopname, eosio::name username, uint64_t draft_id, std::string title, std::string announce, std::string description, std::string preview, std::string images, eosio::name calculation_type, eosio::asset fixed_membership_contribution, uint64_t membership_percent_fee, bool is_can_coop_spend_share_contributions, std::string meta) { 
  
  check_auth_or_fail(_soviet, coopname, username, "createprog"_n);
  
  auto board = get_board_by_type_or_fail(coopname, "soviet"_n);
  
  auto cooperative = get_cooperative_or_fail(coopname);
  auto draft = get_scoped_draft_by_registry_or_fail(_draft, draft_id);
  
  programs_index programs(_soviet, coopname.value);
  
  auto program_id = get_global_id_in_scope(_soviet, coopname, "programs"_n);

  eosio::check(calculation_type == "free"_n || calculation_type == "absolute"_n || calculation_type == "relative"_n, "Неизвестный тип расчёта");
  
  if (calculation_type == "absolute"_n) {
    eosio::check(membership_percent_fee == 0, "Процент членского взноса для независимого расчёта должен равняться нулю");
  } else if ( calculation_type == "relative"_n){
    eosio::check(membership_percent_fee > 0, "Процент членского взноса для относительного расчёта должен не равняться нулю");
    eosio::check(fixed_membership_contribution.amount == 0, "Величина членского взноса при относительном расчёте должна равняться нулю");
  } else {
    eosio::check(membership_percent_fee == 0, "Процент членского взноса должен равняться нулю для безоплатных ЦПП");
    eosio::check(fixed_membership_contribution.amount == 0, "Величина членского взноса должна равняться нулю при безоплатных ЦПП");
  }

  cooperative.check_symbol_or_fail(fixed_membership_contribution);
  
  programs.emplace(username, [&](auto &pr) {
    pr.id = program_id;
    pr.is_active = true;
    pr.draft_id = draft_id;
    pr.coopname = coopname;
    pr.title = title;
    pr.announce = announce;
    pr.description = description;
    pr.preview = preview;
    pr.images = images;
    pr.calculation_type = calculation_type;
    pr.fixed_membership_contribution = fixed_membership_contribution;  
    pr.membership_percent_fee = membership_percent_fee;
    pr.is_can_coop_spend_share_contributions = false;
    pr.share_contributions = eosio::asset(0, _root_govern_symbol);
    pr.membership_contributions = eosio::asset(0, _root_govern_symbol);
    
    pr.available = eosio::asset(0, _root_govern_symbol);
    pr.spended = eosio::asset(0, _root_govern_symbol);
    pr.blocked = eosio::asset(0, _root_govern_symbol);
    
    pr.meta = meta;
  });

  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "newprogram"_n,
      std::make_tuple(coopname, program_id)
  ).send();

};


/**
\ingroup public_actions
\brief Метод редактирования целевой программы
*
* Этот метод позволяет модифицировать существующую программу.
*
* @param coopname Имя кооператива
* @param id Идентификатор программы
* @param name Имя программы
* @param announce Объявление о программе
* @param description Описание программы
* @param preview Предварительный просмотр
* @param images Изображения для программы
* @param website Веб-сайт программы
* @param initial Вступительный взнос
* @param minimum Минимальный взнос
* @param membership Членский взнос
* @param period Периодичность
* @param category Категория
*
* @note Авторизация требуется от аккаунта: @p coopname
*/
void soviet::editprog(eosio::name coopname, eosio::name username, uint64_t program_id, uint64_t draft_id, std::string title, std::string announce, std::string description, std::string preview, std::string images, std::string meta) {
  check_auth_or_fail(_soviet, coopname, username, "editprog"_n);
  
  programs_index programs(_soviet, coopname.value);
  auto existing_program = programs.find(program_id);
  eosio::check(existing_program != programs.end(), "Программа не найдена.");

  programs.modify(existing_program, coopname, [&](auto& pr) {
    pr.draft_id = draft_id;
    pr.title = title;
    pr.announce = announce;
    pr.description = description;
    pr.preview = preview;
    pr.images = images;
    pr.meta = meta;
  });

}


/**
\ingroup public_actions
\brief Метод отключения целевой программы
*
* Этот метод позволяет отключить существующую программу, устанавливая поле `is_active` в `false`.
*
* @param coopname Имя кооператива
* @param id Идентификатор программы
*
* @note Авторизация требуется от аккаунта: @p coopname
*/
void soviet::disableprog(eosio::name coopname, uint64_t program_id) {
  require_auth(coopname);

  programs_index programs(_soviet, coopname.value);
  auto existing_program = programs.find(program_id);
  eosio::check(existing_program != programs.end(), "Программа не найдена.");

  programs.modify(existing_program, coopname, [&](auto& pr) {
    pr.is_active = false;
  });

}

