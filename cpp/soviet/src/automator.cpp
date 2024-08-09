using namespace eosio;

void soviet::is_valid_action_for_automation(eosio::name action_type) {
  std::vector<eosio::name> valid_types = {"regaccount"_n, "authorize"_n};

  eosio::check(std::find(valid_types.begin(), valid_types.end(), action_type) != valid_types.end(), "Действие не найдено среди допустимых типов.");
  
}

/**
\ingroup public_actions
\brief Автоматизация подписи на решениях
*
* Этот метод позволяет члену совета автоматизировать подпись на решениях по определенным типам вопросов. 
* Член совета может настроить автоматическую подпись для указанных типов действий.
*
* @param coopname Имя кооператива
* @param board_id ID совета кооператива
* @param member Имя члена совета, который настраивает автоматизацию
* @param action_type Тип действия для автоматизации
* @param permission_name Имя разрешения для использования в автоматизированном действии
* @param encrypted_private_key Зашифрованный приватный ключ для автоматизации на аккаунт oracul по алгоритму Д-Х
* 
* @note Авторизация требуется от аккаунта: @p member
*/
void soviet::automate(eosio::name coopname, uint64_t board_id, eosio::name member, eosio::name action_type, eosio::name permission_name, std::string encrypted_private_key) {

  require_auth(member);
 
  boards_index boards(_soviet, coopname.value);
  auto board = boards.find(board_id);
  eosio::check(board != boards.end(), "Совет не найден");

  if (action_type == "authorize"_n){
    board -> is_valid_chairman(member);  
  } else {
    soviet::is_valid_action_for_automation(action_type);  
  };

  automator_index automator(_soviet, coopname.value);
  auto by_member_action_index = automator.template get_index<"bymembaction"_n>();
  auto idx = combine_ids(member.value, action_type.value);
  auto autom = by_member_action_index.find(idx);
  
  eosio::check(autom == by_member_action_index.end(), "Автоматизация по данному действию уже установлена");

  automator.emplace(member, [&](auto &a){
    a.id = automator.available_primary_key();
    a.coopname = coopname;
    a.board_id = board_id;
    a.member = member;
    a.action_type = action_type;
    a.permission_name = permission_name;
    a.encrypted_private_key = encrypted_private_key;
  });

}

/**
\ingroup public_actions
\brief Удаление настройки автоматизации подписи
*
* Этот метод позволяет члену совета удалить настройку автоматизации подписи на решениях по определенным типам вопросов. 
*
* @param coopname Имя кооператива
* @param board_id ID совета кооператива
* @param member Имя члена совета, который удаляет автоматизацию
* @param automation_id Идентификатор автоматизации для удаления
* 
* @note Авторизация требуется от аккаунта: @p member
*/
void soviet::disautomate(eosio::name coopname, uint64_t board_id, eosio::name member, uint64_t automation_id ) {
  require_auth(member);

  automator_index automator(_soviet, coopname.value);
  auto autom = automator.find(automation_id);
  eosio::check(autom -> board_id == board_id, "Указан неверный идентификатор совета");
  eosio::check(autom -> member == member, "Это не ваша автоматизация для удаления");

  automator.erase(autom);
}

