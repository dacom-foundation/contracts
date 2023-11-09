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
void soviet::joincoop(eosio::name coopname, eosio::name username, signed_doc signed_doc) { 
  require_auth(_registrator);

  joincoops_index joincoops(_soviet, coopname.value); 
  auto secondary_id = get_global_id(_soviet, "joincoops"_n);

  joincoops.emplace(_registrator, [&](auto &a){
    a.id = secondary_id;
    a.username = username;
    a.is_paid = false;
    a.signed_doc = signed_doc;
  });

  decisions_index decisions(_soviet, coopname.value);
  auto id = get_global_id(_soviet, "decisions"_n);
  decisions.emplace(_registrator, [&](auto &d){
    d.id = id;
    d.coopname = coopname;
    d.type = _regaccount_action;
    d.secondary_id = secondary_id;
  });

  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "newid"_n,
    std::make_tuple(id)
  ).send();
};


void soviet::joincoop_effect(eosio::name executer, eosio::name coopname, uint64_t decision_id, uint64_t secondary_id) { 

  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);

  joincoops_index joincoops(_soviet, coopname.value); 
  auto joincoop_action = joincoops.find(decision->secondary_id);

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
      "registrator"_n,
      "confirmreg"_n,
      std::make_tuple(coopname, joincoop_action -> username)
  ).send();

  decisions.modify(decision, executer, [&](auto &d){
    d.executed = true;
  });

  joincoops.erase(joincoop_action);

};
