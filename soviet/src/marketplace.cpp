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
void soviet::change(eosio::name coopname, eosio::name username, uint64_t program_id, uint64_t exchange_id) { 
  require_auth(_marketplace);  

  decisions_index decisions(_soviet, coopname.value);
  auto decision_id = get_global_id(_soviet, "decisions"_n);
 
  changes_index changes(_soviet, coopname.value);
  auto batch_id = get_global_id(_soviet, "change"_n);

  changes.emplace(_marketplace, [&](auto &c) {
    c.id = batch_id;
    c.program_id = program_id;
    c.exchange_id = exchange_id;
    
    //add type on the top
    //calculate who is money or property giver by type
    
    // 1 часть пачки (решение №1)
    // c.money_contributor = //заказчик
    // c.product_return_statement =   //заявление на возврат продуктом
    // c.product_receipt_transfer_act_from_cooperative =  //подпись заказчика на акте приёма-передачи продукта от кооператива
    // c.product_receipt_transfer_act_validation_from_cooperative = //подпись администратора на акте приёма-передачи от кооператива
    
    // 2 часть пачки (решение №2)
    // c.product_contributor = //поставщик
    // c.product_contribution_statement = //заявление на взнос продуктом
    // c.product_receipt_transfer_act_to_cooperative =  //подпись поставщика на акте приёма-передачи продукта кооперативу
    // c.product_receipt_transfer_act_validation_to_cooperative = //подпись администратора на акте приёма-передачи кооперативу
    
    
  });

  decisions.emplace(_soviet, [&](auto &d){
    d.id = decision_id;
    d.type = _change_action;
    d.batch_id = batch_id;
  });

  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "draft"_n,
    std::make_tuple(coopname, username, decision_id, batch_id)
  ).send();
  
};


void soviet::change_effect(eosio::name executer, eosio::name coopname, uint64_t decision_id, uint64_t batch_id) { 
  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  
  eosio::check(decision != decisions.end(), "Решение не найдено");

  changes_index changes(_soviet, coopname.value);
  auto change = changes.find(batch_id);
  eosio::check(change != changes.end(), "Объект обмена не найден");

  action(
      permission_level{ _soviet, "active"_n},
      "marketplace"_n,
      "authorize"_n,
      std::make_tuple(coopname, change -> exchange_id)
  ).send();

  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "statement"_n,
      std::make_tuple(coopname, change -> money_contributor, _change_action, decision_id, decision->batch_id, change -> product_return_statement)
  ).send();
  
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "statement"_n,
      std::make_tuple(coopname, change -> product_contributor, _change_action, decision_id, decision->batch_id, change -> product_contribution_statement)
  ).send();
  
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "act"_n,
      std::make_tuple(coopname, change -> money_contributor, _change_action, decision_id, decision->batch_id, change -> product_receipt_transfer_act_from_cooperative)
  ).send();
  
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "act"_n,
      std::make_tuple(coopname, change -> product_contributor, _change_action, decision_id, decision->batch_id, change -> product_receipt_transfer_act_to_cooperative)
  ).send();
  
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "batch"_n,
      std::make_tuple(coopname, _change_action, decision->batch_id)
  ).send();


  decisions.erase(decision);
  changes.erase(change);

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

