using namespace eosio;

//заявление на взнос продуктом
void soviet::pgivestate (eosio::name coopname, eosio::name username, uint64_t decision_id, document product_give_statement) {
  require_auth(_soviet);

  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  eosio::check(decision != decisions.end(), "Решение не найдено");
  eosio::check(decision -> type == "productcontr"_n || decision -> type == "moneycontr"_n, "Неверный тип черновика решения совета");
  
  changes_index changes(_soviet, coopname.value);
  auto change = changes.find(decision -> batch_id);
  
  eosio::check(change != changes.end(), "Объект обмена не найден");
  eosio::check(change -> product_contributor == username, "Пользователь не является тем, кто вносит продукт");

  changes.modify(change, _soviet, [&](auto &c){
    c.product_give_statement = product_give_statement;
  });

};

//заявление на возврат продуктом
void soviet::pgetstate (eosio::name coopname, eosio::name username, uint64_t decision_id, document product_get_statement) {
  require_auth(_soviet); 

  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  eosio::check(decision != decisions.end(), "Решение не найдено");
  eosio::check(decision -> type == "productcontr"_n || decision -> type == "moneycontr"_n, "Неверный тип черновика решения совета");
  
  changes_index changes(_soviet, coopname.value);
  auto change = changes.find(decision -> batch_id);
  
  eosio::check(change != changes.end(), "Объект обмена не найден");
  eosio::check(change -> money_contributor == username, "Пользователь не является тем, кто получает продукт");

  changes.modify(change, _soviet, [&](auto &c) {
    c.product_get_statement = product_get_statement;
  });

};

//акт приёма-передачи продукта кооперативу
void soviet::pgiveact (eosio::name coopname, eosio::name username, uint64_t decision_id, document product_give_act) {
  require_auth(_marketplace);  

  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  eosio::check(decision != decisions.end(), "Решение не найдено");
  eosio::check(decision -> type == "productcontr"_n || decision -> type == "moneycontr"_n, "Неверный тип черновика решения совета");
  
  changes_index changes(_soviet, coopname.value);
  auto change = changes.find(decision -> batch_id);
  
  eosio::check(change != changes.end(), "Объект обмена не найден");
  
  eosio::check(change -> product_contributor == username, "Пользователь не является поставщиком продукта");

  changes.modify(change, _marketplace, [&](auto &c) {
    c.product_give_act = product_give_act;
  });  

};

//удостоверить акт приёма-передачи представителем
void soviet::valpgiveact (eosio::name coopname, eosio::name username, uint64_t decision_id, document product_give_validated_act) {
  require_auth(_marketplace);

  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  eosio::check(decision != decisions.end(), "Решение не найдено");
  eosio::check(decision -> type == "productcontr"_n || decision -> type == "moneycontr"_n, "Неверный тип черновика решения совета");
  
  changes_index changes(_soviet, coopname.value);
  auto change = changes.find(decision -> batch_id);
  
  eosio::check(change != changes.end(), "Объект обмена не найден");
  
  auto soviet = get_board_by_type_or_fail(coopname, "soviet"_n);
  auto chairman = soviet.get_chairman();
  
  eosio::check(chairman == username, "Пользователь не является упомолномоченным для валидации акта");

  changes.modify(change, _marketplace, [&](auto &c) {
    c.product_give_validated_act = product_give_validated_act;
  });

};

//акт приёма-передачи пайщику от кооператива
void soviet::pgetact(eosio::name coopname, eosio::name username, uint64_t decision_id, document product_get_act) {
  require_auth(_marketplace);  

  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  eosio::check(decision != decisions.end(), "Решение не найдено");
  eosio::check(decision -> type == "productcontr"_n || decision -> type == "moneycontr"_n, "Неверный тип черновика решения совета");
  
  changes_index changes(_soviet, coopname.value);
  auto change = changes.find(decision -> batch_id);
  
  eosio::check(change != changes.end(), "Объект обмена не найден");
  
  eosio::check(change -> money_contributor == username, "Пользователь не является заказчиком продукта");

  changes.modify(change, _marketplace, [&](auto &c) {
    c.product_get_act = product_get_act;
  });  


};

//удостоверить акт приёма-передачи от кооператива
void soviet::valpgetact (eosio::name coopname, eosio::name username, uint64_t decision_id, document product_get_validated_act) {
  require_auth(_marketplace);

  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  eosio::check(decision != decisions.end(), "Решение не найдено");
  eosio::check(decision -> type == "productcontr"_n || decision -> type == "moneycontr"_n, "Неверный тип черновика решения совета");
  
  changes_index changes(_soviet, coopname.value);
  auto change = changes.find(decision -> batch_id);
  
  eosio::check(change != changes.end(), "Объект обмена не найден");
  
  auto soviet = get_board_by_type_or_fail(coopname, "soviet"_n);
  auto chairman = soviet.get_chairman();

  eosio::check(chairman == username, "Пользователь не является упомолномоченным для валидации акта");

  changes.modify(change, _marketplace, [&](auto &c) {
    c.product_get_validated_act = product_get_validated_act;
  });  

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
void soviet::change(eosio::name coopname, eosio::name parent_username, eosio::name username, uint64_t exchange_id, uint64_t program_id, eosio::name type) { 
  require_auth(_marketplace);  

  decisions_index decisions(_soviet, coopname.value);
  auto decision_id_1 = get_global_id(_soviet, "decisions"_n);
  auto decision_id_2 = get_global_id(_soviet, "decisions"_n);

  changes_index changes(_soviet, coopname.value);
  auto batch_id = get_global_id(_soviet, "change"_n);

  eosio::name money_contributor = type == "order"_n ? username : parent_username;
  eosio::name product_contributor = type == "order"_n ? parent_username : username;
  
  changes.emplace(_marketplace, [&](auto &c) {
    c.id = batch_id;
    c.program_id = program_id;
    c.exchange_id = exchange_id;
    // 1 часть пачки (решение №1)
    c.money_contributor = money_contributor; //заказчик
    // c.product_get_statement =   //заявление на возврат продуктом
    // c.product_get_act =  //подпись заказчика на акте приёма-передачи продукта от кооператива
    // c.product_get_validated_act = //подпись администратора на акте приёма-передачи от кооператива
    
    // 2 часть пачки (решение №2)
    c.product_contributor = product_contributor;//поставщик
    // c.product_give_statement = //заявление на взнос продуктом
    // c.product_give_act =  //подпись поставщика на акте приёма-передачи продукта кооперативу
    // c.product_give_validated_act = //подпись администратора на акте приёма-передачи кооперативу
  });

  decisions.emplace(_soviet, [&](auto &d){
    d.id = decision_id_1;
    d.type = "productcontr"_n;
    d.batch_id = batch_id;
    d.coopname = coopname;
    d.username = product_contributor;  
  });

  decisions.emplace(_soviet, [&](auto &d){
    d.id = decision_id_2;
    d.type = "moneycontr"_n;
    d.batch_id = batch_id;
    d.coopname = coopname;
    d.username = money_contributor;  
  });

  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "setdecision"_n,
    std::make_tuple(coopname, exchange_id, decision_id_1)
  ).send();

  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "draft"_n,
    std::make_tuple(coopname, product_contributor, decision_id_1, batch_id)
  ).send();
  
  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "draft"_n,
    std::make_tuple(coopname, money_contributor, decision_id_2, batch_id)
  ).send();


  exchange_index exchange(_marketplace, coopname.value);
  auto change = exchange.find(exchange_id);
  eosio::check(change != exchange.end(), "Заявка не обнаружена");

  //переносим заявления в совет для утверждения
  if (type == "order"_n) {

    action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "pgetstate"_n,
      std::make_tuple(change -> coopname, username, decision_id_2, change -> return_product_statement)
    ).send();

    action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "pgivestate"_n,
      std::make_tuple(change -> coopname, username, decision_id_1, change -> contribute_product_statement)
    ).send();

  } else {

    eosio::check(false, "Обратные заказы временно не обслуживаются");

  }
  

};

// ЭТО ДОЛЖНО СРАБОТАТЬ ПРИ ЗАВЕРШЕНИИ ОБМЕНА
// void soviet complete_change(){

  // action(
  //     permission_level{ _soviet, "active"_n},
  //     _soviet,
  //     "statement"_n,
  //     std::make_tuple(coopname, change -> money_contributor, _change_action, decision_id, decision->batch_id, change -> product_get_statement)
  // ).send();
  
  // action(
  //     permission_level{ _soviet, "active"_n},
  //     _soviet,
  //     "statement"_n,
  //     std::make_tuple(coopname, change -> product_contributor, _change_action, decision_id, decision->batch_id, change -> product_contribution_statement)
  // ).send();
  
  // action(
  //     permission_level{ _soviet, "active"_n},
  //     _soviet,
  //     "act"_n,
  //     std::make_tuple(coopname, change -> money_contributor, _change_action, decision_id, decision->batch_id, change -> product_receipt_transfer_act_from_cooperative)
  // ).send();
  
  // action(
  //     permission_level{ _soviet, "active"_n},
  //     _soviet,
  //     "act"_n,
  //     std::make_tuple(coopname, change -> product_contributor, _change_action, decision_id, decision->batch_id, change -> product_receipt_transfer_act_to_cooperative)
  // ).send();
  
  // action(
  //     permission_level{ _soviet, "active"_n},
  //     _soviet,
  //     "batch"_n,
  //     std::make_tuple(coopname, _change_action, decision->batch_id)
  // ).send();


// }


void soviet::change_effect(eosio::name executer, eosio::name coopname, uint64_t decision_id, uint64_t batch_id) { 
  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  
  // TODO обмен должен завершаться в момент подписи крайнего акта
  // т.е. здесь надо просто удалять, а завершать надо отдельным вызовом параллельно с complete в маркетплейсе

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

  decisions.erase(decision);
  changes.erase(change);

};


void soviet::cancelorder(eosio::name coopname, eosio::name username, uint64_t program_id, uint64_t exchange_id, uint64_t contribution_id, eosio::asset quantity) { 
  require_auth(_marketplace);

  //TODO delete decisions and change

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

