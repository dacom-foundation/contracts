using namespace eosio;


// удостоверить акт приёма-передачи от кооператива
// это должно срабатывать на выдаче имущества заказчику для формирования закрывающего списка документов
void soviet::recieved (eosio::name coopname, uint64_t exchange_id) {
  require_auth(_marketplace);

  requests_index exchange(_marketplace, coopname.value);
  auto request = exchange.find(exchange_id);
  eosio::check(request != exchange.end(), "Заявка не обнаружена");
  eosio::check(request -> parent_id > 0, "Только встречная заявка может быть обработана");

  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "newdecision"_n,
      std::make_tuple(coopname, request -> money_contributor, _product_return_action, request -> return_product_decision_id, request -> return_product_authorization)
  ).send();

  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "newresolved"_n,
      std::make_tuple(coopname, request -> money_contributor, _product_return_action, request -> return_product_decision_id, request -> return_product_statement)
  ).send();
  
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "newact"_n,
      std::make_tuple(coopname, request -> money_contributor, _product_return_action, request -> return_product_decision_id, request -> product_recieve_act_validation)
  ).send();
  
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "newdecision"_n,
      std::make_tuple(coopname, request -> product_contributor, _product_contribution_action, request -> contribution_product_decision_id, request -> contribution_product_authorization)
  ).send();

  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "newresolved"_n,
      std::make_tuple(coopname, request -> product_contributor, _product_contribution_action, request -> contribution_product_decision_id, request -> contribute_product_statement)
  ).send();
  
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "newact"_n,
      std::make_tuple(coopname, request -> product_contributor, _product_contribution_action, request -> contribution_product_decision_id, request -> product_contribution_act_validation)
  ).send();

};



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
void soviet::change(eosio::name coopname, eosio::name parent_username, eosio::name username, uint64_t exchange_id, eosio::name money_contributor, eosio::name product_contributor) { 
  require_auth(_marketplace);  

  decisions_index decisions(_soviet, coopname.value);
  auto decision_id_1 = get_id(_soviet, coopname, "decisions"_n);
  auto decision_id_2 = get_id(_soviet, coopname, "decisions"_n);
    
  changes_index changes(_soviet, coopname.value);
  auto batch_id = get_global_id(_soviet, "change"_n);

  requests_index exchange(_marketplace, coopname.value);
  auto change = exchange.find(exchange_id);
  eosio::check(change != exchange.end(), "Заявка не обнаружена");

  changes.emplace(_marketplace, [&](auto &c) {
    c.id = batch_id;
    c.exchange_id = exchange_id;
    c.contribution_product_decision_id = decision_id_1;
    c.return_product_decision_id = decision_id_2;
  });

  
  decisions.emplace(_soviet, [&](auto &d) {
    d.id = decision_id_1;
    d.type = _change_action;
    d.batch_id = batch_id;
    d.coopname = coopname;
    d.username = product_contributor;  
  });
  
  decisions.emplace(_soviet, [&](auto &d){
    d.id = decision_id_2;
    d.type = _change_action;
    d.batch_id = batch_id;
    d.coopname = coopname;
    d.username = money_contributor;
  });

  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "newsubmitted"_n,
    std::make_tuple(coopname, product_contributor, _product_contribution_action, decision_id_1, change -> contribute_product_statement)
  ).send();
  
  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "newsubmitted"_n,
    std::make_tuple(coopname, money_contributor, _product_return_action, decision_id_2, change -> return_product_statement)
  ).send();
  

  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "newbatch"_n,
    std::make_tuple(coopname, _change_action, batch_id)
  ).send();

};



void soviet::change_effect(eosio::name executer, eosio::name coopname, uint64_t decision_id, uint64_t batch_id) { 
  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  
  eosio::check(decision != decisions.end(), "Решение не найдено");

  changes_index changes(_soviet, coopname.value);
  auto change = changes.find(batch_id);
  eosio::check(change != changes.end(), "Объект обмена не найден");
  
  auto contribution_product_decision = decisions.find(change -> contribution_product_decision_id);
  auto return_product_decision = decisions.find(change -> return_product_decision_id);

  eosio::check(contribution_product_decision != decisions.end(), "Черновик решения о приёме имущества не найден");
  eosio::check(return_product_decision != decisions.end(), "Черновик решения о возврате взноса не найден");

  if (contribution_product_decision -> authorized == true && return_product_decision -> authorized == true){
    // если принимается второе решение, то эффект должен оповестить контракт кооплейса
    
    action(
      permission_level{ _soviet, "active"_n},
      _marketplace,
      "authorize"_n,
      std::make_tuple(coopname, change -> exchange_id, contribution_product_decision -> id, contribution_product_decision -> authorization, return_product_decision -> id, return_product_decision -> authorization)
    ).send();

    decisions.erase(contribution_product_decision);
    decisions.erase(return_product_decision);
    changes.erase(change);

  } else {
    print("Ожидаем получение второго решения");
    // если принимается первое решение, то эффекта при обмене не должно быть
    // просто ждём второе решение
  }



};

