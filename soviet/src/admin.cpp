using namespace eosio;

/**
\ingroup public_actions
\brief Добавление персонала кооператива
*
* Данный метод позволяет добавить в кооператив нового члена персонала с указанием определенных прав на выполнение методов действий вместо совета.
* Авторизация для выполнения этого метода требуется только от председателя совета кооператива.
*
* @param coopname Имя кооператива
* @param board_id ID совета кооператива
* @param chairman Имя председателя совета
* @param username Имя нового члена персонала
* @param rights Список прав, разрешенных для выполнения указанным членом персонала
* @param position_title Название должности нового члена персонала
* 
* @note Авторизация требуется от аккаунта: @p chairman
*/
void soviet::addstaff(eosio::name coopname, eosio::name chairman, eosio::name username, std::vector<right> rights, std::string position_title) {
  require_auth(chairman);

  staff_index staff(_soviet, coopname.value);
  
  auto board = get_board_by_type_or_fail(coopname, "soviet"_n);
  eosio::check(board.is_valid_chairman(chairman), "Только председатель кооператива может добавлять персонал");

  auto persona = staff.find(username.value);
  accounts_index accounts(_registrator, _registrator.value);
  auto account = accounts.find(username.value);
  eosio::check(account != accounts.end(), "Сотрудник не найден в картотеке аккаунтов");

  eosio::check(persona == staff.end(), "Сотрудник уже добавлен. Отредактируйте его права или переназначьте на другую должность");

  staff.emplace(chairman, [&](auto &a){
    a.username = username;
    a.position_title = position_title;
    a.rights = rights;
    a.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    a.updated_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
  });

};

/**
\ingroup public_actions
\brief Удаление персонала из кооператива
*
* Данный метод позволяет удалить члена персонала из кооператива. 
* Авторизация для выполнения этого метода требуется только от председателя совета кооператива.
*
* @param coopname Имя кооператива
* @param board_id ID совета кооператива
* @param chairman Имя председателя совета
* @param username Имя удаляемого члена персонала
* 
* @note Авторизация требуется от аккаунта: @p chairman
*/
void soviet::rmstaff(eosio::name coopname, eosio::name chairman, eosio::name username) {
  require_auth(chairman);

  staff_index staff(_soviet, coopname.value);
  
  auto board = get_board_by_type_or_fail(coopname, "soviet"_n);
  eosio::check(board.is_valid_chairman(chairman), "Только председатель кооператива может удалять персонал");

  auto persona = staff.find(username.value);
  eosio::check(persona != staff.end(), "Персона не найдена");

  staff.erase(persona);

};

/**
\ingroup public_actions
\brief Установка прав для персонала кооператива
*
* Этот метод позволяет установить конкретные права для члена персонала в кооперативе.
* Авторизация для выполнения этого метода требуется только от председателя совета кооператива.
*
* @param coopname Имя кооператива
* @param board_id ID совета кооператива
* @param chairman Имя председателя совета
* @param username Имя члена персонала, для которого устанавливаются права
* @param rights Вектор прав, которые будут установлены для указанного члена персонала
* 
* @note Авторизация требуется от аккаунта: @p chairman
*/
void soviet::setrights(eosio::name coopname, eosio::name chairman, eosio::name username, std::vector<right> rights) {
  require_auth(chairman);

  staff_index staff(_soviet, coopname.value);
  boards_index boards(_soviet, coopname.value);

  auto board = get_board_by_type_or_fail(coopname, "soviet"_n);
  eosio::check(board.is_valid_chairman(chairman), "Только председатель кооператива может устанавливать права персонала");

  auto persona = staff.find(username.value);
  eosio::check(persona != staff.end(), "Персона не найдена");

  staff.modify(persona, chairman, [&](auto &a){
    a.rights = rights;
    a.updated_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
  });  
};
  

/**
\ingroup public_actions
\brief Предварительная валидация решения совета персоналом
*
* Этот метод позволяет персоналу подтвердить поступление оплаты или провести другую форму валидации перед принятием советом какого-либо решения. 
*
* @param coopname Имя кооператива
* @param board_id ID совета кооператива
* @param username Имя члена персонала, проводящего валидацию
* @param decision_id ID решения, которое подлежит валидации
* 
* @note Авторизация требуется от аккаунта: @p username
*/
void soviet::validate(eosio::name coopname, eosio::name username, uint64_t decision_id) { 
  require_auth(username);

  boards_index boards(_soviet, coopname.value);
  auto board = get_board_by_type_or_fail(coopname, "soviet"_n);
  
  staff_index staff(_soviet, coopname.value);
  auto persona = staff.find(username.value);

  eosio::check(persona != staff.end(), "Указанный аккаунт не является сотрудником");
  eosio::check(persona -> has_right(_soviet, "validate"_n), "Недостаточно прав доступа");

  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  eosio::check(decision != decisions.end(), "Документ не найден");

  decisions.modify(decision, username, [&](auto &d){
    d.validated = !decision -> validated;
  });

  autosigner_index autosigner(_soviet, coopname.value);  
  auto signer = autosigner.find(decision -> id);

  if (signer == autosigner.end())
    autosigner.emplace(_soviet, [&](auto &o) {
      o.decision_id = decision -> id;
    });
}
