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
void soviet::createprog(eosio::name coopname, eosio::name chairman, std::string title, std::string announce, std::string description, std::string preview, std::string images, eosio::asset initial, eosio::asset minimum, eosio::asset maximum, eosio::asset share_contribution, eosio::asset membership_contribution, eosio::name period, eosio::name category, eosio::name calculation_type, uint64_t membership_percent_fee) { 
  require_auth(chairman);

  auto board = get_board_by_type_or_fail(coopname, "soviet"_n);
  
  eosio::check(board.is_valid_chairman(chairman), "Только председатель совета может создать программу");
  auto cooperative = get_cooperative_or_fail(coopname);

  programs_index programs(_soviet, coopname.value);
  
  auto id = get_global_id(_soviet, "programs"_n);

  eosio::check(period == "percase"_n, "Только не периодические взносы от суммы взноса сейчас доступны");
  
  eosio::check(calculation_type == "absolute"_n || calculation_type == "relative"_n, "Неизвестный тип расчёта");

  if (calculation_type == "absolute"_n) {
    eosio::check(membership_percent_fee == 0, "Процент членского взноса для независимого расчёта должен равняться нулю");
  } else {
    eosio::check(membership_percent_fee > 0, "Процент членского взноса для относительного расчёта должен не равняться нулю");
    eosio::check(membership_contribution.amount == 0, "Величина членского взноса при относительном расчёте должна равняться нулю");
  }

  cooperative.check_symbol_or_fail(initial);
  cooperative.check_symbol_or_fail(minimum);
  cooperative.check_symbol_or_fail(maximum);
  cooperative.check_symbol_or_fail(share_contribution);
  cooperative.check_symbol_or_fail(membership_contribution);

  //TODO check minimum and maximum symbols

  programs.emplace(chairman, [&](auto &pr) {
    pr.id = id;
    pr.coopname = coopname;
    pr.title = title;
    pr.announce = announce;
    pr.description = description;
    pr.preview = preview;
    pr.images = images;
    pr.is_active = true;
    pr.category = category;

    pr.calculation_type = calculation_type;
    pr.share_contribution = share_contribution;
    pr.membership_contribution = membership_contribution;  /*!< Членский взнос */
    pr.membership_percent_fee = membership_percent_fee;
    
    pr.period = period;     /*!< Периодичность *///  (percase, daily, weekly, monthly, quarterly, halfayear, annually)
    pr.minimum = minimum;
    pr.maximum = maximum;
  });


  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "newid"_n,
    std::make_tuple(id)
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
void soviet::editprog(eosio::name coopname, uint64_t id, std::string title, std::string announce, std::string description, std::string preview, std::string images, eosio::name category) {
  require_auth(coopname);

  programs_index programs(_soviet, coopname.value);
  auto existing_program = programs.find(id);
  eosio::check(existing_program != programs.end(), "Программа не найдена.");

  programs.modify(existing_program, coopname, [&](auto& pr) {
    pr.title = title;
    pr.announce = announce;
    pr.description = description;
    pr.preview = preview;
    pr.images = images;
    pr.category = category;
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
void soviet::disableprog(eosio::name coopname, uint64_t id) {
  require_auth(coopname);

  programs_index programs(_soviet, coopname.value);
  auto existing_program = programs.find(id);
  eosio::check(existing_program != programs.end(), "Программа не найдена.");

  programs.modify(existing_program, coopname, [&](auto& pr) {
    pr.is_active = false;
  });
}

