using namespace eosio;

/**
\ingroup public_actions
\brief Модерация товара на маркетплейсе.
*
* Данный метод предназначен для модерации товара перед его публикацией на маркетплейсе. 
* Метод может быть вызван только администратором маркетплейса.
*
* @param username Имя пользователя-администратора, который вызывает данный метод.
* @param exchange_id Уникальный идентификатор товара, который нужно опубликовать после модерации.
*
* @note Авторизация требуется от аккаунта: @p username
*/
[[eosio::action]] void marketplace::moderate(eosio::name username, uint64_t exchange_id) { 
  require_auth(username);
  
  exchange_index exchange(_marketplace, _marketplace.value);
  
  auto change = exchange.find(exchange_id);
  eosio::check(change != exchange.end(), "Ордер не найден");

  if (change -> status == "moderation"_n) {
    staff_index staff(_soviet, change -> coop_username.value);
    auto persona = staff.find(username.value);
    eosio::check(persona -> has_right(_marketplace, "moderate"_n), "Недостаточно прав доступа");

    exchange.modify(change, username, [&](auto &o){
      o.status = "published"_n;
    });
  }
};


/**
\ingroup public_actions
\brief Отказ в прохождении модерации заявки на товар.
*
* Этот метод предназначен для администраторов маркетплейса, чтобы отказать в публикации товара после его модерации. 
* При отказе администратор указывает причину отказа в параметре `meta`.
*
* @param username Имя пользователя-администратора, который вызывает данный метод.
* @param exchange_id Уникальный идентификатор товара, публикацию которого нужно запретить.
* @param meta Строковое описание или причина, по которой товар не прошел модерацию.
*
* @note Авторизация требуется от аккаунта: @p username
*/
[[eosio::action]] void marketplace::prohibit(eosio::name username, uint64_t exchange_id, std::string meta) { 
  require_auth(username);
  
  exchange_index exchange(_marketplace, _marketplace.value);
  auto change = exchange.find(exchange_id);
  eosio::check(change != exchange.end(), "Ордер не найден");

  staff_index staff(_soviet, change -> coop_username.value);
  auto persona = staff.find(username.value);
  eosio::check(persona -> has_right(_marketplace, "prohibit"_n), "Недостаточно прав доступа");

  exchange.modify(change, username, [&](auto &o){
    o.status = "prohibit"_n;
    o.meta = meta;
  });
  
};


/**
\ingroup public_actions
\brief Снять товар с публикации на маркетплейсе.
*
* Этот метод предназначен для снятия товара с публикации. Только владелец товара может снять его с публикации.
*
* @param username Имя пользователя, являющегося владельцем заявки.
* @param exchange_id Идентификатор заявки, которую следует снять с публикации.
*
* @note Авторизация требуется от аккаунта: @p username
*/
[[eosio::action]] void marketplace::unpublish(eosio::name username, uint64_t exchange_id) { 
  require_auth(username);
  
  exchange_index exchange(_marketplace, _marketplace.value);
  auto change = exchange.find(exchange_id);
  eosio::check(change != exchange.end(), "Ордер не найден");
  eosio::check(change->username == username, "У вас нет права на снятие данной заявки");

  exchange.modify(change, username, [&](auto &o){
    o.status = "unpublished"_n;
  });
};


/**
\ingroup public_actions
\brief Опубликовать товар на маркетплейсе.
*
* Этот метод позволяет владельцу товара опубликовать его на маркетплейсе. Для публикации товар должен находиться в статусе "unpublished".
*
* @param username Имя пользователя, являющегося владельцем заявки.
* @param exchange_id Идентификатор заявки, которую следует опубликовать.
*
* @note Авторизация требуется от аккаунта: @p username
*/
[[eosio::action]] void marketplace::publish(eosio::name username, uint64_t exchange_id) { 
  require_auth(username);
  
  exchange_index exchange(_marketplace, _marketplace.value);
  auto change = exchange.find(exchange_id);
  eosio::check(change != exchange.end(), "Ордер не найден");
  eosio::check(change->username == username, "У вас нет права на публикацию данной заявки");
  eosio::check(change->status == "unpublished"_n, "Неверный статус для публикации");

  exchange.modify(change, username, [&](auto &o){
    o.status = "published"_n;
  });
}