// /**
//  * @mainpage Описание системы заявок
//  *
//  * В системе имеются два типа заявок: order (заказ) и offer (предложение), а также два уровня заявок: родительская (base) и встречная (quote).
//  * Используется одна таблица заявок: exchange(_marketplace, _marketplace). Встречная заявка всегда должна быть противополжного типа к родительской.
//  *
//  * <b>Группы заявок:</b>
//  * 
//  * <b>1. Группа предложений от родителя:</b>
//  * - Parent: type == 'offer' && (parent_id == 0) => имущественный паевый взнос
//  * - Child: type == 'order' && (parent_id > 0) => денежный паевый взнос
//  *
//  * <b>2. Группа заказа от родителя:</b>
//  * - Parent: type == "order" && (parent_id == 0) => денежный паевый взнос
//  * - Child: type == "offer" && (parent_id > 0) => имущественный паевый взнос
//  *
//  * <b>Обозначения:</b>
//  * - order => денежный паевый взнос
//  * - offer => имущественный паевый взнос
//  */


/**
\ingroup public_actions
\brief Создать заявку на денежный паевой взнос.
*
* Данный метод позволяет пользователю создать заявку на денежный паевой взнос в системе.
*
* @param params Параметры для создания заявки на денежный паевой взнос.
*
* @note Авторизация требуется от аккаунта: @p params.username
*/
[[eosio::action]] void marketplace::order (const exchange_params& params) {
  require_auth(params.username);

  marketplace::create("order"_n, params);

};

/**
\ingroup public_actions
\brief Создать заявку на имущественный паевой взнос.
*
* Данный метод позволяет пользователю создать заявку на имущественный паевой взнос в системе.
*
* @param params Параметры для создания заявки на имущественный паевой взнос.
*
* @note Авторизация требуется от аккаунта: @p params.username
*/
[[eosio::action]] void marketplace::offer (const exchange_params& params) {
  require_auth(params.username);
  
  marketplace::create("offer"_n, params);
  
};



/**
 * @brief Создание заявки на обмен
 * 
 * @param type Тип заявки
 * @param params Параметры заявки
 * 
 * Общая функция для создания как родительских, так и дочерних заявок.
 */
void marketplace::create (eosio::name type, const exchange_params& params) {
  orgs_index orgs(_registrator, _registrator.value);
  auto coop = orgs.find(params.coopname.value);
  eosio::check(coop != orgs.end() && coop -> is_coop(), "Кооператив не найден");
  eosio::check(params.unit_cost.symbol == coop -> initial.symbol, "Неверный символ токен");
  eosio::check(params.pieces > 0, "Количество единиц в заявке должно быть больше нуля");
  eosio::check(params.unit_cost.amount >= 0, "Цена не может быть отрицательной");

  if (params.parent_id == 0) {
    marketplace::create_parent(type, params);
  } else {
    marketplace::create_child(type, params);
  };
};


/**
 * @brief Создание родительской заявки
 * 
 * @param type Тип заявки
 * @param params Параметры заявки
 * 
 * Специализированная функция для создания родительской заявки.
 */
void marketplace::create_parent(eosio::name type, const exchange_params& params) {
  eosio::check(type == "offer"_n, "В родительском заявке может быть только предложение");

  exchange_index exchange(_marketplace, params.coopname.value);
  uint64_t id = get_global_id(_marketplace, "exchange"_n);
  
  eosio::check(params.parent_id == 0, "Родительская заявка создаётся без указания родителя");

  orgs_index orgs(_registrator, _registrator.value);
  auto coop = orgs.find(params.coopname.value);
  eosio::check(coop != orgs.end(), "Кооператив не найден");
  eosio::check(coop -> is_coop() == true, "Организация - не кооператив");
    
  participants_index participants(_soviet, params.coopname.value);
  auto participant = participants.find(params.username.value);      
  eosio::check(participant != participants.end(), "Вы не являетесь членом указанного кооператива");

  auto program = get_program_or_fail(params.coopname, params.program_id);

  //Специальные проверки
  if (type == "offer"_n) {
    //срок жизни продукта должен быть установлен
    eosio::check(params.product_lifecycle_secs > 0, "Гарантийный срок возврата для имущества должен быть установлен");

  } 

  else if(type == "order"_n) {
    
    eosio::asset quantity = params.unit_cost * params.pieces;

    action(
      permission_level{ _marketplace, "active"_n},
      _soviet,
      "blockbal"_n,
      std::make_tuple(params.coopname, params.username, quantity)
    ).send();

    action(
      permission_level{ _marketplace, "active"_n},
      _soviet,
      "addprogbal"_n,
      std::make_tuple(params.coopname, params.username, params.program_id, quantity)
    ).send();

  };

  exchange.emplace(params.username, [&](auto &i) {
    i.id = id;
    i.type = type;
    i.program_id = params.program_id; 
    i.username = params.username;
    i.coopname = params.coopname;
    i.status = "moderation"_n;
    i.token_contract = coop -> token_contract;
    i.remain_units = params.pieces;
    i.unit_cost = params.unit_cost;
    i.supplier_amount = params.unit_cost * params.pieces;

    i.membership_fee = asset(0, coop -> initial.symbol);
    i.total_cost = asset(0, coop -> initial.symbol);
    
    i.product_lifecycle_secs = params.product_lifecycle_secs;
    i.data = params.data;
    i.meta = params.meta;
    i.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    i.cancellation_fee_amount = asset(0, params.unit_cost.symbol);
  });
  
  action(
    permission_level{ _marketplace, "active"_n},
    _marketplace,
    "newid"_n,
    std::make_tuple(id, type)
  ).send();
  
};



/**
 * @brief Создание дочерней заявки
 * 
 * @param type Тип заявки
 * @param params Параметры заявки
 * 
 * Специализированная функция для создания дочерних заявок, связанных с родительской заявкой.
 */
void marketplace::create_child(eosio::name type, const exchange_params& params) {
  eosio::check(type == "order"_n, "В дочерней заявки может быть только заказ");

  exchange_index exchange(_marketplace, params.coopname.value);
  auto parent_change = exchange.find(params.parent_id);
  eosio::check(parent_change != exchange.end(), "Заявка не обнаружена");
  eosio::check(parent_change -> status == "published"_n, "Заявка не опубликована или не прошла модерацию");

  orgs_index orgs(_registrator, _registrator.value);
  auto coop = orgs.find(params.coopname.value);
  eosio::check(coop != orgs.end(), "Кооператив не найден");
  eosio::check(coop -> is_coop() == true, "Организация - не кооператив");
  eosio::check(parent_change -> token_contract == coop -> token_contract, "Неверный контракт");

  eosio::check(parent_change -> unit_cost.amount == params.unit_cost.amount, "Торги запрещены");
  
  eosio::check(params.parent_id > 0, "Встречная заявка создаётся с указанием родителя");
  
  uint64_t id = get_global_id(_marketplace, "exchange"_n);
  
  auto program = get_program_or_fail(params.coopname, params.program_id);

  eosio::check(parent_change -> program_id == params.program_id, "Целевые программы должны совпадать");

  participants_index participants(_soviet, params.coopname.value);
  auto participant = participants.find(params.username.value);      
  eosio::check(participant != participants.end(), "Вы не являетесь членом указанного кооператива");
  
  uint64_t product_lifecycle_secs = 0;

  eosio::asset membership_fee;
  eosio::asset supplier_amount = params.unit_cost * params.pieces;

  if (program.calculation_type == "absolute"_n) {
    membership_fee = program.fixed_membership_contribution;
  } else {
    membership_fee = supplier_amount * HUNDR_PERCENTS / program.membership_percent_fee;
  };
  
  eosio::asset total_cost = params.unit_cost * params.pieces + membership_fee;
  
  //Специальные проверки
  if (type == "offer"_n) {
    //родительская заявка должна быть противоположного типа
    eosio::check(parent_change -> type == "order"_n, "Неверный тип родительской заявки");

  } else if(type == "order"_n) {
    //родительская заявка должна быть противоположного типа
    eosio::check(parent_change -> type == "offer"_n, "Неверный тип родительской заявки");
    
    action(
      permission_level{ _marketplace, "active"_n},
      _soviet,
      "blockbal"_n,
      std::make_tuple(params.coopname, params.username, total_cost)
    ).send();
    
    action(
      permission_level{ _marketplace, "active"_n},
      _soviet,
      "addprogbal"_n,
      std::make_tuple(params.coopname, params.username, params.program_id, total_cost)
    ).send();

  }
  
  exchange.emplace(params.username, [&](auto &i) {
    i.id = id;
    i.parent_id = params.parent_id;
    i.parent_username = parent_change -> username;
    i.type = type; 
    i.program_id = parent_change -> program_id;
    i.coopname = params.coopname;
    i.username = params.username;
    i.status = "published"_n;
    i.token_contract = coop -> token_contract;
    i.remain_units = params.pieces;
    i.unit_cost = params.unit_cost;
    i.membership_fee = membership_fee;
    
    i.supplier_amount = supplier_amount;
    i.total_cost = total_cost;
    
    // не дублируем информацию
    // i.data = params.data;
    // i.meta = params.meta;

    i.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    i.cancellation_fee_amount = asset(0, params.unit_cost.symbol);


    if (type == "order"_n) {
      print("on create child order");
      i.return_product_statement = params.document;
      i.money_contributor = params.username;
      i.product_contributor = parent_change -> username;
      i.product_lifecycle_secs = parent_change -> product_lifecycle_secs;
    } else if (type == "offer"_n) {
      print("on create child offer");
      i.contribute_product_statement = params.document;
      i.money_contributor = parent_change -> username;
      i.product_contributor = params.username;  
      i.product_lifecycle_secs = params.product_lifecycle_secs;
    };

  });

  action(
    permission_level{ _marketplace, "active"_n},
    _marketplace,
    "newid"_n,
    std::make_tuple(id, type)
  ).send();

};



/**
\ingroup public_actions
\brief Подтверждение готовности выполнить заявку.

@details Данный метод позволяет пользователю, который получил предложение по своей заявке, подтвердить свою готовность его принять и выполнить. При этом формируется пакет документов, который отправляется в совет на утверждение. 

@param username Имя пользователя, подтверждающего готовность выполнить предложение.
@param exchange_id ID предложения, которое следует подтвердить.
 
@note Авторизация требуется от аккаунта: @p username
*/
[[eosio::action]] void marketplace::accept(eosio::name coopname, eosio::name username, uint64_t exchange_id, document document) { 
  require_auth(username);
 
  exchange_index exchange(_marketplace, coopname.value);
  auto change = exchange.find(exchange_id);
  eosio::check(change != exchange.end(), "Заявка не найдена");
  eosio::check(change -> status == "published"_n, "Только заявка в статусе ожидания может быть принята");

  auto parent_change = exchange.find(change -> parent_id);
  eosio::check(parent_change != exchange.end(), "Родительская заявка не найдена");
  eosio::check(parent_change -> username == username, "Недостаточно прав доступа");
  eosio::check(parent_change -> remain_units >= change -> remain_units, "Недостаточно объектов для поставки");
  
  exchange.modify(parent_change, username, [&](auto &i) {
    i.remain_units -= change -> remain_units;
    i.supplier_amount = (parent_change -> remain_units - change -> remain_units ) * parent_change -> unit_cost;
    i.blocked_units += change -> remain_units;
  });

  exchange.modify(change, username, [&](auto &o){
    o.status = "accepted"_n;
    o.blocked_units += change -> remain_units;
    o.remain_units = 0;
    o.accepted_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());

    if (change -> type == "order"_n) {
      o.contribute_product_statement = document;
    } else if (change -> type == "offer"_n) {
      o.return_product_statement = document;
    };

  });

  action(
    permission_level{ _marketplace, "active"_n},
    _soviet,
    _change_action,
    std::make_tuple(change -> coopname, username, change -> username, exchange_id, change -> money_contributor, change -> product_contributor)
  ).send();
}



[[eosio::action]] void marketplace::supply(eosio::name coopname, eosio::name username, uint64_t exchange_id, document document) {
  require_auth(username);

  exchange_index exchange(_marketplace, coopname.value);
  auto change = exchange.find(exchange_id);
  eosio::check(change != exchange.end(), "Заявка не найдена");
  eosio::check(change -> parent_id > 0, "Только продукт по встречной заявке может быть поставлен");

  eosio::check(change -> status == "authorized"_n, "Продукт может быть поставлен только по заявке в статусе authorized");

  auto soviet = get_board_by_type_or_fail(coopname, "soviet"_n);
  auto chairman = soviet.get_chairman();
  
  eosio::check(username == chairman, "Недостаточно прав доступа для подтверждения поставки");

  exchange.modify(change, username, [&](auto &ch) {
    ch.supplied_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    ch.status = "supplied1"_n;
    ch.product_contribution_act_validation = document;
  });

}


/**
\ingroup public_actions
\brief Отказ от предложения.
**/

[[eosio::action]] void marketplace::supplycnfrm(eosio::name coopname, eosio::name username, uint64_t exchange_id, document document) { 
  require_auth(username);

  exchange_index exchange(_marketplace, coopname.value);
  auto change = exchange.find(exchange_id);
  eosio::check(change != exchange.end(), "Заявка не найдена");
  eosio::check(change -> parent_id > 0, "Только продукт по встречной заявке может быть поставлен");

  eosio::check(change -> status == "supplied1"_n, "Продукт может быть поставлен только по заявке в статусе supplied1");

  auto soviet = get_board_by_type_or_fail(coopname, "soviet"_n);
  auto chairman = soviet.get_chairman();
    
  eosio::check(change -> product_contributor == username, "Недостачно прав доступа для совершения поставки");

  //подписываем акт приёма-передачи кооперативу пайщиком
  exchange.modify(change, username, [&](auto &ch) {
    ch.status = "supplied2"_n;
    ch.product_contribution_act = document;
  });

  action(
    permission_level{ _marketplace, "active"_n},
    _soviet,
    "addbalance"_n,
    std::make_tuple(coopname, change -> product_contributor, change -> supplier_amount)
  ).send();


  action(
    permission_level{ _marketplace, "active"_n},
    _soviet,
    "blockbal"_n,
    std::make_tuple(coopname, change -> product_contributor, change -> supplier_amount)
  ).send();


  action(
    permission_level{ _marketplace, "active"_n},
    _soviet,
    "addprogbal"_n,
    std::make_tuple(coopname, change -> product_contributor, change -> program_id, change -> supplier_amount)
  ).send();

}





[[eosio::action]] void marketplace::delivered(eosio::name coopname, eosio::name username, uint64_t exchange_id) {
  require_auth(username);

  exchange_index exchange(_marketplace, coopname.value);
  auto change = exchange.find(exchange_id);
  eosio::check(change != exchange.end(), "Заявка не найдена");
  eosio::check(change -> parent_id > 0, "Только продукт по встречной заявке может быть доставлен");

  eosio::check(change -> status == "supplied2"_n, "Продукт может быть поставлен только по заявке в статусе supplied2");

  auto soviet = get_board_by_type_or_fail(coopname, "soviet"_n);
  auto chairman = soviet.get_chairman();

  eosio::check(username == chairman, "Недостаточно прав доступа для подтверждения доставки");

  auto program = get_program_or_fail(coopname, change -> program_id);

  exchange.modify(change, username, [&](auto &ch) {
    ch.status = "delivered"_n;
    ch.delivered_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    ch.deadline_for_receipt = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch() + change -> product_lifecycle_secs / 4);
  });
}






/**
\ingroup public_actions
\brief Подпись акта получения имущества пайщиком
**/

[[eosio::action]] void marketplace::recieve(eosio::name coopname, eosio::name username, uint64_t exchange_id, document document) { 
  require_auth(username);

  exchange_index exchange(_marketplace, coopname.value);
  auto change = exchange.find(exchange_id);
  eosio::check(change != exchange.end(), "Заявка не найдена");
  eosio::check(change -> parent_id > 0, "Только продукт по встречной заявке может быть выдан");

  eosio::check(change -> status == "delivered"_n, "Продукт может быть выдан только по заявке в статусе ожидания получения");

  eosio::check(change -> deadline_for_receipt.sec_since_epoch() >= eosio::current_time_point().sec_since_epoch(), "Время на выдачу имущества истекло");
  
  auto soviet = get_board_by_type_or_fail(coopname, "soviet"_n);
  auto chairman = soviet.get_chairman();
  
  if (change -> type == "order"_n) { //если указанная заявка - это заказ продукта
    //то получение продукта может осуществить только пользователь из username
    eosio::check(change -> username == username, "Недостачно прав доступа для получения имущества");
  } else { //если указанная заявка - это поставка продукта

    //то поставку может осуществить только пользователь username в этой заявке
    eosio::check(change -> parent_username == username, "Недостаточно прав доступа для получения имущества");
  };

    //подписываем акт приёма-передачи кооперативу пайщиком
  exchange.modify(change, username, [&](auto &ch){
    ch.status = "recieved1"_n;
    ch.recieved_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    ch.warranty_delay_until = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch() + change -> product_lifecycle_secs / 4);
    ch.product_recieve_act = document;
  });

}



[[eosio::action]] void marketplace::recievecnfrm(eosio::name coopname, eosio::name username, uint64_t exchange_id, document document) {
  
  exchange_index exchange(_marketplace, coopname.value);
  auto change = exchange.find(exchange_id);
  eosio::check(change != exchange.end(), "Заявка не найдена");
  eosio::check(change -> parent_id > 0, "Только продукт по встречной заявке может быть поставлен");

  eosio::check(change -> status == "recieved1"_n, "Продукт может быть поставлен только по заявке в статусе recieved1");

  auto soviet = get_board_by_type_or_fail(coopname, "soviet"_n);
  auto chairman = soviet.get_chairman();
  
  eosio::check(username == chairman, "Недостачно прав доступа для подтверждения выдачи имущества");

  exchange.modify(change, username, [&](auto &ch) {
    ch.status = "recieved2"_n;
    ch.product_recieve_act_validation = document;
  });

  action(
    permission_level{ _marketplace, "active"_n},
    _soviet,
    "recieved"_n,
    std::make_tuple(coopname, exchange_id)
  ).send();

}




[[eosio::action]] void marketplace::dispute(eosio::name coopname, eosio::name username, uint64_t exchange_id, document document){

  require_auth(username);
  
  exchange_index exchange(_marketplace, coopname.value);
  auto change = exchange.find(exchange_id);
  
  eosio::check(change -> type == "order"_n, "Спор может быть открыт только по заявке на поставку");
  
  eosio::check(change != exchange.end(), "Заявка не найдена");
  eosio::check(change -> username == username, "Только заказчик может открыть спор");
  
  eosio::check(change -> is_warranty_return == false, "Нельзя открыть спор на спор");

  auto parent_change = exchange.find(change -> parent_id);
  eosio::check(parent_change != exchange.end(), "Родительская заявка не найдена");

  eosio::check(change -> status == "recieved2"_n, "Неверный статус для открытия спора");
  
  exchange.modify(change, username, [&](auto &e){
    e.status = "disputed"_n;
    e.disputed_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    e.warranty_return_id = change -> id;
  });

  uint64_t new_id = get_global_id(_marketplace, "exchange"_n);
  uint64_t new_parent_id = get_global_id(_marketplace, "exchange"_n);  

  auto cooperative = get_cooperative_or_fail(coopname);
  
  //открытая заявка от заказчика на поставку имущества поставщику
  exchange.emplace(username, [&](auto &i) {
    i.id = new_parent_id;
    i.type = "offer"_n;
    i.program_id = change -> program_id; 
    i.username = username;
    i.parent_username = ""_n;
    i.parent_id = 0;
    i.coopname = coopname;
    i.status = "published"_n;
    i.token_contract = change -> token_contract;
    i.remain_units = change -> remain_units;
    i.unit_cost = change -> unit_cost;

    i.supplier_amount = i.unit_cost * i.remain_units;
    i.total_cost = i.supplier_amount;

    i.membership_fee = asset(0, cooperative.initial.symbol);
    i.cancellation_fee_amount = asset(0, cooperative.initial.symbol);
    
    i.product_lifecycle_secs = change -> product_lifecycle_secs;
    i.data = change -> data;
    i.meta = change -> meta;
    i.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());

    i.is_warranty_return = true;
    i.warranty_return_id = change -> id;
  });

  action(
    permission_level{ _marketplace, "active"_n},
    _marketplace,
    "newid"_n,
    std::make_tuple(new_parent_id, "offer")
  ).send();

  //новая встречная заявка для будущего возврата поставщику
  exchange.emplace(username, [&](auto &i) {
    i.id = new_id;
    i.parent_id = new_parent_id;
    i.type = "order"_n;

    i.program_id = parent_change -> program_id;
    i.coopname = coopname;
    
    i.username = change -> parent_username;
    i.parent_username = change -> username;
    
    i.status = "published"_n;
    i.token_contract = cooperative.token_contract;
    
    i.remain_units = change -> remain_units;
    i.unit_cost = change -> unit_cost;
    
    i.supplier_amount = change -> supplier_amount;
    i.total_cost = change -> supplier_amount;;
    
    i.membership_fee = asset(0, cooperative.initial.symbol); //размер членского взноса должен установить председатель в методе resolve
    i.cancellation_fee_amount = asset(0, cooperative.initial.symbol);

    i.data = change -> data;
    i.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    
    i.is_warranty_return = true;
    i.warranty_return_id = exchange_id;

    i.contribute_product_statement = document;
    i.money_contributor = parent_change -> username;
    i.product_contributor = change -> username;  
    i.product_lifecycle_secs = 0; //потому что гарантийного срока в споре нет
    
  });


  action(
    permission_level{ _marketplace, "active"_n},
    _marketplace,
    "newid"_n,
    std::make_tuple(new_id, "order"_n)
  ).send();
  
};

  
  // I принимаем брак
  // создать открытую заявку от заказчика на поставку
  // создать встречную заявку от поставщика на заказ своего имущества
    // без заявления
    // это отправит запрос на взнос в совет
    //  т.е. отправляется только одна часть: за внос, вторая часть: за возврат будет отправлена принята позднее
    //  т.е. делим стандартный процесс обмена на 2 независимые части, всё остальное как обычно
    //  accept должен обработать это
    //  cancel должен обработать это
    // после решения совета - немедленно начислить баланс в кошелёк
  

  // II отправляем брак назад
  // теперь необходимо получить согласие или отказ от изначального поставщика забрать свой продукт
    // метод accept2 принимает согласие получить возврат и запускает в ход второе решение
    // метод cancel2 принимает отказ получить возврат и завершает ход

  // методы передачи  supply, supplycnfrm, delivery, ... мы можем использовать повторно
  

// обратный процесс доставки в том же ордере (указатель на объект диспута)
// поля для оценок / отзывов в том же ордере
// moneyblock для поставщика по кошельку (простая проверка при попытке вывода на запрет по диспуту)
// dsptact
// dsptactcnfrm
// moneyback



// [[eosio::action]] marketplace::resolve()



/**
\ingroup public_actions
\brief Подписание акта о приёме-передаче имущества.

* @details После успешного получения товара, получатель подписывает акт о приёме-передаче, что свидетельствует о юридическом завершении сделки. Этот акт делает пакет документов по данной сделке полным. После проведения ряда проверок, обновляются статусы и количество объектов в основной заявке и предложении. Если все объекты основной заявки обработаны, заявка удаляется из публикации. В зависимости от типа предложения, может осуществляться перевод токенов.

* @param username Имя пользователя-получателя товара.
* @param exchange_id ID предложения, под которым следует подписать акт.

* @note Авторизация требуется от аккаунта: @p username
*/
[[eosio::action]] void marketplace::complete(eosio::name coopname, eosio::name username, uint64_t exchange_id) { 
  require_auth(username);
  
  exchange_index exchange(_marketplace, coopname.value);
  auto change = exchange.find(exchange_id);
  eosio::check(change != exchange.end(), "Заявка не найдена");
  
  eosio::check(change -> parent_id > 0, "У указанной заявки нет встречной заявки");

  auto parent_change = exchange.find(change -> parent_id);
  eosio::check(parent_change != exchange.end(), "Родительская заявка не найдена");
  
  // eosio::check(change -> username == username, "Вы не можете подтвердить исполнение заявки");
  eosio::check(change -> status == "recieved2"_n, "Заявка находится в неверном статусе для утверждения обмена");
  
  eosio::check(change -> warranty_delay_until.sec_since_epoch() < eosio::current_time_point().sec_since_epoch(), "Время гарантийной задержки еще не истекло");

  exchange.modify(parent_change, username, [&](auto &i) {
    i.delivered_units += change -> blocked_units;
    i.blocked_units -= change -> blocked_units; 
    
    if (i.blocked_units + parent_change -> remain_units == 0) {
      i.status = "unpublished"_n; //снимаем родительскую заявку с публикации, если она исполнена
    }; 
  });

  exchange.modify(change, username, [&](auto &o) {
    o.status = "completed"_n;
    o.delivered_units += change -> blocked_units;
    o.blocked_units = 0;
    o.completed_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
  });

  auto program = get_program_or_fail(coopname, change -> program_id);
  
  //Заказчику уменьшаем баланс ЦПП не кошелька, разблокируем баланс ЦПП кошелька и списываем его  
  action(
    permission_level{ _marketplace, "active"_n},
    _soviet,
    "subprogbal"_n,
    std::make_tuple(coopname, change -> money_contributor, change -> program_id, change -> total_cost)
  ).send();
  
  action(
    permission_level{ _marketplace, "active"_n},
    _soviet,
    "unblockbal"_n,
    std::make_tuple(coopname, change -> money_contributor, change -> total_cost)
  ).send();

  action(
    permission_level{ _marketplace, "active"_n},
    _soviet,
    "subbalance"_n,
    std::make_tuple(coopname, change -> money_contributor, change -> total_cost)
  ).send();


  //Поставщику уменьшаем баланс ЦПП не кошелька и разблокируем баланс ЦПП кошелька
  action(
    permission_level{ _marketplace, "active"_n},
    _soviet,
    "subprogbal"_n,
    std::make_tuple(coopname, change -> product_contributor, change -> program_id, change -> supplier_amount)
  ).send();

  action(
    permission_level{ _marketplace, "active"_n},
    _soviet,
    "unblockbal"_n,
    std::make_tuple(coopname, change -> product_contributor, change -> supplier_amount)
  ).send();


  if (change -> membership_fee.amount > 0){
    action(
      permission_level{ _marketplace, "active"_n},
      _fund,
      "spreadamount"_n,
      std::make_tuple(coopname, change -> membership_fee)
    ).send();
  }

}




/**
\ingroup public_actions
\brief Отказ от предложения.

* @details Этот метод позволяет пользователю отклонить предложение, представленное к его заявке.
* Выполняются следующие проверки:
* - Существование предложения с указанным ID.
* - Существование основной заявки.
* - Предложение находится в статусе "ожидание".
* 
* Если отклонено предложение к заявке типа "order", осуществляется возврат токенов пользователю, которому были заблокированы токены при создании предложения.
* 
* @param username Имя пользователя, отклоняющего предложение.
* @param exchange_id ID предложения, которое следует отклонить.
* @param meta Дополнительные метаданные, связанные с отказом.
* 
* @note Авторизация требуется от аккаунта: @p username
*/
[[eosio::action]] void marketplace::decline(eosio::name coopname, eosio::name username, uint64_t exchange_id, std::string meta) { 
  require_auth(username);

  exchange_index exchange(_marketplace, coopname.value);
  auto change = exchange.find(exchange_id);
  auto parent_change = exchange.find(change -> parent_id);

  eosio::check(change != exchange.end(), "Заявка не найдена");
  eosio::check(parent_change != exchange.end(), "Родительская заявка не найдена");
  eosio::check(change -> status == "published"_n, "Только заявка в статусе ожидания может быть отклонена");


  exchange.modify(change, username, [&](auto &o){
    o.status = "declined"_n;
    o.declined_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    o.meta = meta;
  });

  if (change -> type == "order"_n) {

    action(
      permission_level{ _marketplace, "active"_n},
      _soviet,
      "subprogbal"_n,
      std::make_tuple(coopname, change -> money_contributor, change -> program_id, change -> total_cost)
    ).send();

    action(
      permission_level{ _marketplace, "active"_n},
      _soviet,
      "unblockbal"_n,
      std::make_tuple(coopname, change -> money_contributor, change -> total_cost)
    ).send();


  }; 
  
}




/**
\ingroup public_actions
\brief Отмена заявки и возврат токенов.

@details Позволяет пользователю отменить родительскую или дочернюю заявку, а также обеспечивает возврат токенов владельцу (если применимо). При отмене проверяется наличие заявки и её текущий статус. 

@param username Имя пользователя, инициировавшего отмену.
@param exchange_id Идентификатор заявки для отмены.

@note Авторизация требуется от аккаунта: @p username
*/
[[eosio::action]] void marketplace::cancel(eosio::name coopname, eosio::name username, uint64_t exchange_id) { 
  require_auth(username);

  exchange_index exchange(_marketplace, coopname.value);
  auto change = exchange.find(exchange_id);
  eosio::check(change != exchange.end(), "Заявка не найдена");
  
  //TODO перенести под проверку пользователя и вообще сценарий отмены проверить полностью
  eosio::check(change -> status != "accepted"_n, "Заявка не может быть отменена сейчас");

  if (change -> parent_id == 0) {
    marketplace::cancel_parent(coopname, username, exchange_id);
  } else {
    marketplace::cancel_child(coopname, username, exchange_id);
  };
}


/**
 * @brief Отмена родительской заявки.
 *
 * Вызывается из `cancel`, если заявка является родительской.
 * Выполняется проверка, что заявка не имеет заблокированных единиц товара, и удаляется из хранилища.
 *
 * @param username Имя пользователя, осуществляющего отмену заявки.
 * @param exchange_id ID родительской заявки, которую нужно отменить.
 */
void marketplace::cancel_parent(eosio::name coopname, eosio::name username, uint64_t exchange_id) {
  exchange_index exchange(_marketplace, coopname.value);
  auto change = exchange.find(exchange_id);

  //Удаление, если заблокированных объектов на поставке - нет.   
  eosio::check(change -> remain_units + change -> blocked_units == 0, "Заявка не может быть отменена из-за наличия заблокированных единиц товара");
  exchange.erase(change);
};

/**
 * @brief Отмена дочерней заявки.
 *
 * Вызывается из `cancel`, если заявка является дочерней.
 * Обновляет количество оставшихся и заблокированных единиц товара в родительской заявке и удаляет дочернюю заявку из хранилища.
 * В зависимости от статуса и типа заявки возможен возврат токенов "покупателю".
 *
 * @param username Имя пользователя, осуществляющего отмену заявки.
 * @param exchange_id ID дочерней заявки, которую нужно отменить.
 */
void marketplace::cancel_child(eosio::name coopname, eosio::name username, uint64_t exchange_id) {
  exchange_index exchange(_marketplace, coopname.value);
  auto change = exchange.find(exchange_id);
  auto parent_change = exchange.find(change -> parent_id);
  eosio::asset quantity = change -> unit_cost * change -> blocked_units;

  
  // оповещаем совет об отмене
  if (change -> type == "order"_n) {
    action(
      permission_level{ _marketplace, "active"_n},
      _soviet,
      "subprogbal"_n,
      std::make_tuple(coopname, change -> money_contributor, change -> program_id, change -> total_cost)
    ).send();

    action(
      permission_level{ _marketplace, "active"_n},
      _soviet,
      "unblockbal"_n,
      std::make_tuple(coopname, change -> money_contributor, change -> total_cost)
    ).send();

  };  

  if (change -> status == "authorized"_n) {
    //возвращаем единицы товара в родительскую заявку
    exchange.modify(parent_change, username, [&](auto &e) {
      e.remain_units += change -> blocked_units;
      e.blocked_units -= change -> blocked_units;
      e.supplier_amount += change -> supplier_amount;
    });

    exchange.modify(change, username, [&](auto &c){
      c.status = "canceled"_n;
      c.canceled_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    });

    //удаляем дочернюю заявку
    // exchange.erase(change);
  } else if (change -> status == "published"_n) {

    exchange.modify(change, username, [&](auto &c){
      c.status = "canceled"_n;
      c.canceled_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    });

    //удаляем дочернюю заявку
    // exchange.erase(change);
  } else {
    //TODO здесь должно быть допустимо, но для каждого статуса по-своему
    eosio::check(false, "Заявка находится в недопустимом статусе для отмены");
  }

}


/**
\ingroup public_actions
\brief Отмена заявки и возврат токенов.

@details Позволяет пользователю отменить родительскую или дочернюю заявку, а также обеспечивает возврат токенов владельцу (если применимо). При отмене проверяется наличие заявки и её текущий статус. Если заявка является родительской, вызывается метод `cancel_parent`, иначе — `cancel_child`.

@param username Имя пользователя, инициировавшего отмену.
@param exchange_id Идентификатор заявки для отмены.

@note Авторизация требуется от аккаунта: @p username
*/
[[eosio::action]] void marketplace::update(eosio::name coopname, eosio::name username, uint64_t exchange_id, uint64_t remain_units, eosio::asset unit_cost, std::string data, std::string meta) {
  require_auth(username);

  exchange_index exchange(_marketplace, coopname.value);
  auto change = exchange.find(exchange_id);
  eosio::check(change != exchange.end(), "Заявка на обмен не найдена");
  eosio::check(unit_cost.symbol == change -> unit_cost.symbol, "Неверный символ токен");
  eosio::check(change -> parent_id == 0, "Встречные заявки можно только отменять");
  eosio::check(change -> username == username, "У вас нет права на отмену данной заявки");
  eosio::check(remain_units >= 0, "Количество единиц товара должно быть положительным");

  eosio::name status = "moderation"_n;

  if (change -> data == data && change -> meta == meta) 
    status = change -> status;

  exchange.modify(change, username, [&](auto &i) {
    i.status = status;
    i.unit_cost = unit_cost;
    i.remain_units = remain_units;
    i.data = data;
    i.meta = meta;
    i.supplier_amount = remain_units * unit_cost;
  });
}

/**
\ingroup public_actions
\brief Добавление единиц товара к заявке.

@details Метод позволяет владельцу заявки дополнительно увеличить количество товара, доступное для обмена в рамках указанной заявки. 
Используется, когда у продавца появляется дополнительное количество товара, которое он хочет добавить к существующей заявке.

@param username Имя пользователя, инициировавшего добавление.
@param exchange_id Идентификатор заявки, к которой добавляются единицы товара.
@param new_pieces Количество новых единиц товара, которые следует добавить к заявке.

@note Авторизация требуется от аккаунта: @p username
*/
[[eosio::action]] void marketplace::addpieces(eosio::name coopname, eosio::name username, uint64_t exchange_id, uint64_t new_pieces) {
  require_auth(username);
    
  exchange_index exchange(_marketplace, coopname.value);
  auto change = exchange.find(exchange_id);
  
  eosio::check(change -> username == username, "У вас нет прав на редактирование данной заявки");
  eosio::check(change -> parent_id == 0, "Нельзя отредактировать количество единиц во встречной заявке. Отмените её и пересоздайте");

  exchange.modify(change, username, [&](auto &c){
    c.remain_units += new_pieces;
    c.supplier_amount = (change -> remain_units + new_pieces) * change -> unit_cost;
  });
  
};




