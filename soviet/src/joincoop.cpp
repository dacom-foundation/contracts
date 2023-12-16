using namespace eosio;

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
void soviet::joincoop(eosio::name coopname, eosio::name username, document document) { 
  require_auth(_registrator);
  
  joincoops_index joincoops(_soviet, coopname.value); 
  auto batch_id = get_global_id(_soviet, "joincoops"_n);

  joincoops.emplace(_registrator, [&](auto &a){
    a.id = batch_id;
    a.username = username;
    a.is_paid = false;
    a.statement = document;
  });
  
  decisions_index decisions(_soviet, coopname.value);
  auto decision_id = get_global_id(_soviet, "decisions"_n);
  
  decisions.emplace(_registrator, [&](auto &d){
    d.id = decision_id;
    d.coopname = coopname;
    d.username = username;
    d.type = _regaccount_action;
    d.batch_id = batch_id;
    d.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
  });
  
  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "draft"_n,
    std::make_tuple(coopname, username, decision_id, batch_id)
  ).send();
  
};


void soviet::joincoop_effect(eosio::name executer, eosio::name coopname, uint64_t decision_id, uint64_t batch_id) { 

  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);

  joincoops_index joincoops(_soviet, coopname.value); 
  auto joincoop_action = joincoops.find(decision->batch_id);

  participants_index participants(_soviet, coopname.value);
  auto cooperative = get_cooperative_or_fail(coopname);

  participants.emplace(_soviet, [&](auto &m){
    m.username = joincoop_action -> username;
    m.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    m.last_update = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    m.last_min_pay = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    m.status = "accepted"_n;
    m.is_initial = true;
    m.is_minimum = true;
    m.has_vote = true;    
    m.available = asset(0, cooperative.initial.symbol);
    m.blocked = asset(0, cooperative.initial.symbol);
    m.minimum = cooperative.minimum; //TODO add minimum amount here
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
      "statement"_n,
      std::make_tuple(coopname, joincoop_action -> username, _regaccount_action, decision_id, decision->batch_id, joincoop_action -> statement)
  ).send();
  
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "decision"_n,
      std::make_tuple(coopname, joincoop_action -> username, _regaccount_action, decision_id, decision->batch_id, joincoop_action -> authorization)
  ).send();

  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "batch"_n,
      std::make_tuple(coopname, _regaccount_action, decision -> batch_id)
  ).send();

  decisions.erase(decision);
  joincoops.erase(joincoop_action);

};
