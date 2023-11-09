/**
\ingroup public_actions
\brief Авторизация обмена советом.

@details Метод используется для подтверждения согласия совета на заявленный обмен. 
Обычно этот метод вызывается после прохождения определенного процесса голосования или принятия решения советом.
Авторизованный обмен считается утвержденным и может быть выполнен.

@param exchange_id Идентификатор заявки на обмен, которую следует авторизовать.

@note Авторизация требуется от аккаунта: @p _soviet
*/
[[eosio::action]] void marketplace::authorize(eosio::name coopname, uint64_t exchange_id) {
  require_auth(_soviet);

  exchange_index exchange(_marketplace, coopname.value);
  auto change = exchange.find(exchange_id);

  eosio::check(change != exchange.end(), "Ордер не найден");
  
  exchange.modify(change, _soviet, [&](auto &o) { 
    o.status = "authorized"_n; 
  });

};