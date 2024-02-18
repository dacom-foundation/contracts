using namespace eosio;


//это должно срабатывать при завершении цикла обмена в маркетплейсе (когда срок гарантии вышел и время разморозить / списать балансы)
void soviet::completed(eosio::name coopname, uint64_t exchange_id) {
  // require_auth(_marketplace);
  // print("coopname: ", coopname);
  // print("exchange_id: ", exchange_id);
  
  // exchange_index exchange(_marketplace, coopname.value);
  // auto request = exchange.find(exchange_id);
  // eosio::check(request != exchange.end(), "Заявка не обнаружена");

  
}


// удостоверить акт приёма-передачи от кооператива
// это должно срабатывать на выдаче имущества заказчику для формирования закрывающего списка документов
void soviet::recieved (eosio::name coopname, uint64_t exchange_id) {
  require_auth(_marketplace);

  //TODO CHANGE DECISION_ID PROBLEM

  exchange_index exchange(_marketplace, coopname.value);
  auto request = exchange.find(exchange_id);
  eosio::check(request != exchange.end(), "Заявка не обнаружена");
  eosio::check(request -> parent_id > 0, "Только встречная заявка может быть обработана");

  //TODO control decision params
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "decision"_n,
      std::make_tuple(coopname, request -> money_contributor, _product_return_action, request -> return_product_decision_id, request -> return_product_authorization)
  ).send();

  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "statement"_n,
      std::make_tuple(coopname, request -> money_contributor, _product_return_action, request -> return_product_decision_id, request -> return_product_statement)
  ).send();
  
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "act"_n,
      std::make_tuple(coopname, request -> money_contributor, _product_return_action, request -> return_product_decision_id, request -> product_recieve_act_validation)
  ).send();
  

  
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "decision"_n,
      std::make_tuple(coopname, request -> product_contributor, _product_contribution_action, request -> contribution_product_decision_id, request -> contribution_product_authorization)
  ).send();

  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "statement"_n,
      std::make_tuple(coopname, request -> product_contributor, _product_contribution_action, request -> contribution_product_decision_id, request -> contribute_product_statement)
  ).send();
  
  
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "act"_n,
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
  auto decision_id_1 = get_global_id(_soviet, "decisions"_n);
  auto decision_id_2 = get_global_id(_soviet, "decisions"_n);

  changes_index changes(_soviet, coopname.value);
  auto batch_id = get_global_id(_soviet, "change"_n);

  exchange_index exchange(_marketplace, coopname.value);
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
    "draft"_n,
    std::make_tuple(coopname, product_contributor, decision_id_1)
  ).send();
  
  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "draft"_n,
    std::make_tuple(coopname, money_contributor, decision_id_2)
  ).send();
  

  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "batch"_n,
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







void soviet::cancelorder(eosio::name coopname, eosio::name username, uint64_t exchange_id) { 
  require_auth(_marketplace);
  //username здесь - это аккаунт, который инициировал отмену (ему и штраф платить)

  // exchange_index exchange(_marketplace, coopname.value);
  // auto request = exchange.find(exchange_id);
  // eosio::check(request != exchange.end(), "Заявка не обнаружена");
  // eosio::check(request -> parent_id == 0, "Только встречная заявка требует оповещения совета");


  // if (request -> type == "order"_n) {
    
  //   action(
  //     permission_level{ _soviet, "active"_n},
  //     _soviet,
  //     "subprogbal"_n,
  //     std::make_tuple(coopname, username, request-> program_id, request-> amount)
  //   ).send();

  //   action(
  //     permission_level{ _soviet, "active"_n},
  //     _soviet,
  //     "unblockbal"_n,
  //     std::make_tuple(coopname, username, request-> amount)
  //   ).send();

  // }


}


void soviet::mcontribute(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::name type, uint64_t secondary_id) { 
  // eosio::check(has_auth(_marketplace) || has_auth(username), "Недостаточно прав доступа");
  // eosio::name payer = has_auth(_marketplace) ? _marketplace : username;
  
  // programs_index programs(_soviet, coopname.value);
  // auto existing_program = programs.find(program_id);

  // exchange_index exchange(_marketplace, coopname.value);
  // auto request = exchange.find(secondary_id);
  // eosio::check(request != exchange.end(), "Заявка не обнаружена");

  // eosio::check(existing_program != programs.end(), "Программа не найдена.");
  // eosio::check(type == "change"_n, "Неподдерживаемый тип операции");

  // participants_index participants(_soviet, coopname.value);
  // auto participant = participants.find(username.value);
  
  // eosio::check(participant != participants.end(), "Вы не являетесь пайщиком указанного кооператива");
  // eosio::check(participant -> is_active(), "Ваш аккаунт не активен в указанном кооперативе");
  
  // action(
  //   permission_level{ _soviet, "active"_n},
  //   _soviet,
  //   "blockbal"_n,
  //   std::make_tuple(coopname, username, request -> amount)
  // ).send();

  // action(
  //   permission_level{ _soviet, "active"_n},
  //   _soviet,
  //   "addprogbal"_n,
  //   std::make_tuple(coopname, username, program_id, request -> amount)
  // ).send();

};


//Вызвать при совершении взноса имуществом поставщиком из кооплейса для выдачи заблокированного баланса в ЦПП кошелька и добавление баланса к ЦПП не кошелька
void soviet::pcontribute(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::name type, uint64_t secondary_id) {
  require_auth(_marketplace);

  // print("on soviet contribute");
  // programs_index programs(_soviet, coopname.value);
  // auto existing_program = programs.find(program_id);

  // eosio::check(existing_program != programs.end(), "Программа не найдена.");
  // eosio::check(type == "change"_n, "Неподдерживаемый тип операции");

  // exchange_index exchange(_marketplace, coopname.value);
  // auto request = exchange.find(secondary_id);
  // eosio::check(request != exchange.end(), "Заявка не обнаружена");

  // participants_index participants(_soviet, coopname.value);
  // auto participant = participants.find(username.value);

  // eosio::check(participant != participants.end(), "Вы не являетесь пайщиком указанного кооператива");
  // eosio::check(participant -> is_active(), "Ваш аккаунт не активен в указанном кооперативе");
  

  // action(
  //   permission_level{ _soviet, "active"_n},
  //   _soviet,
  //   "addbalance"_n,
  //   std::make_tuple(coopname, username, request -> amount)
  // ).send();


  // action(
  //   permission_level{ _soviet, "active"_n},
  //   _soviet,
  //   "blockbal"_n,
  //   std::make_tuple(coopname, username, request -> amount)
  // ).send();


  // action(
  //   permission_level{ _soviet, "active"_n},
  //   _soviet,
  //   "addprogbal"_n,
  //   std::make_tuple(coopname, username, program_id, request -> amount)
  // ).send();

}
