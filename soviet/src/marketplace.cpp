using namespace eosio;

/**
\ingroup public_actions
\brief Обмен в кооперативе
*
* Этот метод вызывается контрактом marketplace при создании запроса на обмен в кооперативе. При успешном вызове формируется повестка со всеми сопровождающими документами, которая отправляется на голосование членам совета.
*
* @param coopname Имя кооператива
* @param exchange_id Идентификатор обмена
* @param orderer Участник, который вносит денежный паевый взнос
* @param offerer Участник, который вносит имущественный паевый взнос
* @param quantity Сумма взаимного зачёта
* 
* @note Авторизация требуется от аккаунта: @p _marketplace
*/
void soviet::change(eosio::name coopname, uint64_t program_id, uint64_t exchange_id) { 
  require_auth(_marketplace);  

  decisions_index decisions(_soviet, coopname.value);
  auto id = get_global_id(_soviet, "decisions"_n);
 
  changes_index changes(_soviet, coopname.value);
  auto change_id = get_global_id(_soviet, "change"_n);

  changes.emplace(_marketplace, [&](auto &c){
    c.id = change_id;
    c.program_id = program_id;
    c.exchange_id = exchange_id;
    
    //TODO add signature information
  });

  decisions.emplace(_soviet, [&](auto &d){
    d.id = id;
    d.type = _change_action;
    d.secondary_id = change_id;
  });

  uint64_t seed = generate();

  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "newid"_n,
    std::make_tuple(id, seed)
  ).send();
  
};


void soviet::change_effect(eosio::name executer, eosio::name coopname, uint64_t decision_id, uint64_t secondary_id) { 
  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  
  eosio::check(decision != decisions.end(), "Решение не найдено");

  changes_index changes(_soviet, coopname.value);
  auto change = changes.find(secondary_id);
  eosio::check(change != changes.end(), "Объект обмена не найден");

  action(
      permission_level{ _soviet, "active"_n},
      "marketplace"_n,
      "authorize"_n,
      std::make_tuple(coopname, change -> exchange_id)
  ).send();
  
  decisions.modify(decision, executer, [&](auto &d){
    d.executed = true;
  });

};


void soviet::cancelorder(eosio::name coopname, eosio::name username, uint64_t program_id, uint64_t exchange_id, uint64_t contribution_id, eosio::asset quantity) { 
  require_auth(_marketplace);

  cntrbutions_index contributions(_soviet, coopname.value);  
  auto contribution = contributions.find(contribution_id);
  contributions.erase(contribution);

  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "unblprogbal"_n,
    std::make_tuple(coopname, username, program_id, quantity)
  ).send();


  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "subbalfrprog"_n,
    std::make_tuple(coopname, username, program_id, quantity)
  ).send();


  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "addcoopbal"_n,
    std::make_tuple(coopname, username, quantity)
  ).send();


  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "withdraw"_n,
    std::make_tuple(coopname, username, quantity)
  ).send();


}

