using namespace eosio;

/**
 * @ingroup public_actions
 * @brief  Добавление пайщика по API без процедуры подписания заявления на вступление.
 * Действие добавляет действующего пайщика в систему, пропуская этап подписи заявления на вступление и оплату вступительного и минимального паевого взносов.
 * Система позволяет установить дату регистрации участника, которая будет соответствовать дате уплаты им минимального и вступительного взносов.
 * Если spread_initial установлен в false, то сумма вступительного взноса не распределяется среди фондов. Тогда система считает, что учет распределения вступительного взноса произошел за её пределами. 
 * Если spread_initial установлен в true, то сумма вступительного взноса распределяется по фондам согласно правилам распределения кооператива в контракте fund.
 * Минимальный паевый взнос всегда добавляется в кошелёк пайщика и отмечается в статистике оборотного фонда кооператива.
 *
 * @param[in]  coopname        The coopname
 * @param[in]  username        The username
 * @param[in]  type            The type
 * @param[in]  created_at      The created at
 * @param[in]  initial         The initial
 * @param[in]  minimum         The minimum
 * @param[in]  spread_initial  The spread initial
 *
 */
void soviet::adduser(eosio::name coopname, eosio::name username, eosio::name type, eosio::time_point_sec created_at, eosio::asset initial, eosio::asset minimum, bool spread_initial) {
  
  check_auth_and_get_payer_or_fail({_registrator});
  
  auto cooperative = get_cooperative_or_fail(coopname);

  accounts_index accounts(_registrator, _registrator.value);
  auto account = accounts.find(username.value);

  eosio::check(account != accounts.end(), "Аккаунт не найден");
  
  participants_index participants(_soviet, coopname.value);
  
  participants.emplace(_soviet, [&](auto &m){
    m.username = username;
    m.created_at = created_at;
    m.last_update = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    m.last_min_pay = created_at;
    m.status = "accepted"_n;
    m.is_initial = true;
    m.is_minimum = true;
    m.has_vote = true;    
    m.type = type;
  });

  wallets_index wallets(_soviet, coopname.value);

  wallets.emplace(_soviet, [&](auto &w){
    w.username = username;
    w.coopname = coopname;
    w.available = asset(0, cooperative.initial.symbol);
    w.blocked = asset(0, cooperative.initial.symbol);
    w.minimum = minimum; 
  });
  
  /**
   * Добавляем в оборотный фонд минимальный паевый взнос и распределяем по всем прочим фондам значение вступительного взноса.
   */
  action(
    permission_level{ _soviet, "active"_n},
    _gateway,
    "adduser"_n,
    std::make_tuple(coopname, username, initial, minimum, created_at, spread_initial)
  ).send();

}

/**
\ingroup public_actions
\brief Подтверждение оплаты регистрационного взноса
*
* Этот метод вызывается контрактом gateway при поступлении оплаты регистрационного взноса, что изменяет состояние объекта joincoop и сообщает администраторам, что оплата взноса произведена.
*
* @param coopname Имя кооператива
* @param username Имя пользователя
* @param position_title Заголовок должности
* @param position Должность
* 
* @note Авторизация требуется от аккаунта: @p _registrator
*/
void soviet::regpaid(eosio::name coopname, eosio::name username){
  require_auth(_gateway);

  joincoops_index joincoops(_soviet, coopname.value);
  auto joincoops_by_username_index = joincoops.template get_index<"byusername"_n>();

  auto joincoop = joincoops_by_username_index.find(username.value);

  eosio::check(joincoop != joincoops_by_username_index.end(), "Информация о регистрируемом пользователе не найдена" );  
  eosio::check(joincoop -> is_paid == false, "Регистрационный взнос уже оплачен");

  joincoops_by_username_index.modify(joincoop, _gateway, [&](auto &j){
    j.is_paid = true;
  });
};


/**
\ingroup public_actions
\brief Заявка на вступление в кооператив
*
* Этот метод вызывается контрактом registrator при поступлении заявления на вступление в кооператив. При успешном вызове формируется повестка на голосование членам совета. Повестка по вопросам регистрации аккаунтов может быть автоматизирована.
*
* @param coopname Имя кооператива
* @param username Имя пользователя
* @param position_title Заголовок должности
* @param position Должность
* 
* @note Авторизация требуется от аккаунта: @p _registrator
*/
void soviet::joincoop(eosio::name coopname, eosio::name braname, eosio::name username, document document) { 
  require_auth(_registrator);
  
  joincoops_index joincoops(_soviet, coopname.value); 
  
  auto batch_id = get_global_id(_soviet, "joincoops"_n);

  joincoops.emplace(_registrator, [&](auto &a){
    a.id = batch_id;
    a.username = username;
    a.braname = braname;
    a.is_paid = false;
  });
  
  decisions_index decisions(_soviet, coopname.value);
  auto decision_id = get_id(_soviet, coopname, "decisions"_n);
  
  decisions.emplace(_registrator, [&](auto &d){
    d.id = decision_id;
    d.coopname = coopname;
    d.username = username;
    d.type = _regaccount_action;
    d.batch_id = batch_id;
    d.statement = document;
    d.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
  });
  
  // сохраняем документ во входящем реестре
  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "newsubmitted"_n,
    std::make_tuple(coopname, username, "joincoop"_n, decision_id, document)
  ).send();
  
};

void soviet::cancelreg(eosio::name coopname, eosio::name username, std::string message){
  require_auth(_gateway);

  auto cooperative = get_cooperative_or_fail(coopname);
  
  participants_index participants(_soviet, coopname.value);
  
  auto participant = participants.find(username.value);
  
  /** 
    Если отмена регистрации происходит до принятия решения советом - том отменять нам здесь нечего, 
    и мы просто игнорируем код ниже. 
  */
  if (participant != participants.end()) {
    
    wallets_index wallets(_soviet, coopname.value);
    auto wallet = wallets.find(username.value);
    
    //обнуляем кошелёк
    wallets.modify(wallet, _soviet, [&](auto &w){
      w.minimum = asset(0, wallet -> minimum.symbol);
      w.initial = asset(0, wallet -> minimum.symbol);
    });
    
    //TODO send block action here
    action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "block"_n,
      std::make_tuple(coopname, coopname, username, message)
    ).send();
  }
};

void soviet::joincoop_effect(eosio::name executer, eosio::name coopname, uint64_t decision_id, uint64_t batch_id) { 

  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);

  joincoops_index joincoops(_soviet, coopname.value); 
  auto joincoop_action = joincoops.find(decision -> batch_id);

  participants_index participants(_soviet, coopname.value);
  auto cooperative = get_cooperative_or_fail(coopname);

  accounts_index accounts(_registrator, _registrator.value);
  auto account = accounts.find(joincoop_action -> username.value);
  eosio::check(account != accounts.end(), "Аккаунт не найден");
  
  auto participant = participants.find(joincoop_action ->username.value);

  participants.emplace(_soviet, [&](auto &m){
      m.username = joincoop_action -> username;
      m.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
      m.last_min_pay = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
      m.status = "accepted"_n;
      m.is_initial = true;
      m.is_minimum = true;
      m.has_vote = true;    
      m.type = account -> type;
      m.braname = joincoop_action -> braname.has_value() ? joincoop_action -> braname.value() : ""_n;
    });  
  
  wallets_index wallets(_soviet, coopname.value);
  eosio::asset minimum = account -> type == "organization"_n ? cooperative.org_minimum.value() : cooperative.minimum; 
  eosio::asset initial = account -> type == "organization"_n ? cooperative.org_initial.value() : cooperative.initial;
  
  auto wallet = wallets.find(joincoop_action -> username.value);
  
  wallets.emplace(_soviet, [&](auto &w){
    w.username = joincoop_action -> username;
    w.coopname = coopname;
    w.available = asset(0, cooperative.initial.symbol);
    w.blocked = asset(0, cooperative.initial.symbol);
    w.minimum = minimum;
    w.initial = initial;
  });
  
  action(
      permission_level{ _soviet, "active"_n},
      _registrator,
      "confirmreg"_n,
      std::make_tuple(coopname, joincoop_action -> username)
  ).send();

  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "newresolved"_n,
      std::make_tuple(coopname, joincoop_action -> username, _regaccount_action, decision_id, decision -> statement)
  ).send();
  
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "newdecision"_n,
      std::make_tuple(coopname, joincoop_action -> username, _regaccount_action, decision_id, decision -> authorization)
  ).send();

  decisions.erase(decision);
  joincoops.erase(joincoop_action);

};
