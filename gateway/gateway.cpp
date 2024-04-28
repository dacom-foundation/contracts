#include "gateway.hpp"
#include <ctime>
#include <eosio/transaction.hpp>

using namespace eosio;


/**
 * @brief Пустой метод регистрации нового идентификатора
 * @ingroup public_actions
 * Этот метод используется для возврата информации из контракта.
 * @param id идентификатор
 * @param type тип идентификатора
 */
void gateway::newdepositid(eosio::name username, uint64_t id) {
  require_auth(_gateway);
};

void gateway::newwithdrid(eosio::name username, uint64_t id) {
  require_auth(_gateway);
};


/**
\ingroup public_actions
 * @brief Создает новую запись депозита в контракте `gateway`.
 * @details Действие `dpcreate` позволяет пользователю `username` создать запись о депозите 
 * в указанном кооперативе по имени аккаунта `coopname` с определенными параметрами. 
 * 
 * @note Требуется авторизация пользователя `username`.
 *
 * @param username Имя пользователя, создающего запись.
 * @param coopname Имя аккаунта кооператива, в рамках которого создается депозит.
 * @param program_id Идентификатор программы, с которой связан депозит.
 * @param purpose Назначение платежа ('registration' или 'deposit').
 * @param batch_id Вторичный идентификатор, связанный с депозитом.
 * @param internal_quantity Количество во внутреннем формате.
 * @param external_quantity Количество во внешнем формате.
 * @param memo Примечание к депозиту.
 *
 * Пример создания новой записи депозита через cleos:
 * 
 * cleos push action gateway dpcreate '["username", "coopaccount", 123, "registration", 456, "10.0000 SYS", "10.0000 EXT", "Депозит для программы X"]' -p username@active
 */

[[eosio::action]] void gateway::deposit(eosio::name coopname, eosio::name username, eosio::name type, eosio::asset quantity) {
  eosio::name payer = has_auth(coopname) ? coopname : username;
  print("on here");
  eosio::check(has_auth(payer), "Недостаточно прав доступа");

  deposits_index deposits(_gateway, coopname.value);

  uint64_t id = get_global_id(_gateway, "deposits"_n);

  auto cooperative = get_cooperative_or_fail(coopname);
  
  eosio::check(type == "registration"_n || type == "deposit"_n, "Неверный тип заявки");
  eosio::check(quantity.amount > 0, "Сумма ввода должна быть положительной");

  if (type == "deposit"_n) {
    participants_index participants(_soviet, coopname.value);
    auto participant = participants.find(username.value);
    eosio::check(participant != participants.end(), "Вы не являетесь пайщиком указанного кооператива");
    eosio::check(participant -> is_active(), "Ваш аккаунт не активен в указанном кооперативе");
  } else {
    eosio::check(quantity == cooperative.registration, "Сумма минимального взноса не соответствует установленной в кооперативе");
  }
  deposits.emplace(payer, [&](auto &d) {
    d.id = id;
    d.type = type;
    d.username = username;
    d.coopname = coopname;
    d.token_contract = cooperative.token_contract;
    d.quantity = quantity;
    d.status = "pending"_n;
    d.expired_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch() + _deposit_expiration_seconds);
  });

  action(
    permission_level{ _gateway, "active"_n},
    _gateway,
    "newdepositid"_n,
    std::make_tuple(username, id)
  ).send();

}

/**
 * @brief Завершает обработку депозита в контракте `gateway`.
 *
 * @details Действие `dpcomplete` используется для установки статуса депозита в 'completed' и обновления его заметки.
 * Это действие также инициирует выпуск токенов соответствующему пользователю, основываясь на данных депозита.
 *
 * @note Требуется авторизация аккаунта контракта `gateway`.
 * @ingroup public_actions
 *
 * @param deposit_id Идентификатор депозита, который завершается.
 * @param memo Новая заметка, связанная с депозитом.
 *
 * Завершение обработки депозита через cleos
 *
 * cleos push action gateway dpcomplete '[123, "Заметка к завершенному депозиту"]' -p gateway@active
 */
void gateway::dpcomplete(eosio::name coopname, eosio::name admin, uint64_t deposit_id, std::string memo) {

  require_auth(admin);

  if (coopname != admin){
    check_auth_or_fail(coopname, admin, "dpcomplete"_n);
  };

  deposits_index deposits(_gateway, coopname.value);
  auto deposit = deposits.find(deposit_id);

  eosio::check(deposit != deposits.end(), "Объект процессинга не найден");
  eosio::check(deposit -> coopname == coopname, "Указан не верный кооператив");
  eosio::check(deposit -> status == "pending"_n, "Статус депозита должен быть pending");

  auto cooperative = get_cooperative_or_fail(coopname);  
  cooperative.check_symbol_or_fail(deposit -> quantity);

  deposits.modify(deposit, admin, [&](auto &d){
    d.status = "completed"_n;
    d.expired_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch() + _deposit_expiration_seconds);
  });
  
  if (deposit -> type == "deposit"_n) {
  
    action(
      permission_level{ _gateway, "active"_n},
      _soviet,
      "addbalance"_n,
      std::make_tuple(coopname, deposit -> username, deposit -> quantity)
    ).send();
    
    action(
      permission_level{ _gateway, "active"_n},
      _fund,
      "addcirculate"_n,
      std::make_tuple(coopname, deposit -> quantity)
    ).send();
  
  } else {
    //TODO spread to funds

    action(
      permission_level{ _gateway, "active"_n},
      _fund,
      "addcirculate"_n,
      std::make_tuple(coopname, cooperative.minimum)
    ).send();
    
    action(
      permission_level{ _gateway, "active"_n},
      _fund,
      "spreadamount"_n,
      std::make_tuple(coopname, cooperative.initial)
    ).send();

    action(
      permission_level{ _gateway, "active"_n},
      _soviet,
      "regpaid"_n,
      std::make_tuple(coopname, deposit -> username)
    ).send();
    
  }
  
}

/**
 * @brief Устанавливает статус депозита как "failed" в контракте `gateway`.
 *
 * @details Действие `dpfail` используется для обозначения неудачи в обработке депозита, устанавливая его статус в 'failed' и обновляя заметку.
 *
 * @note Требуется авторизация аккаунта контракта `gateway`.
 * @ingroup public_actions
 *
 * @param deposit_id Идентификатор депозита, статус которого обновляется.
 * @param memo Новая заметка, связанная с депозитом.
 *
 * Установка статуса депозита как 'failed' через cleos
 *
 * cleos push action gateway dpfail '[123, "Заметка к неудачному депозиту"]' -p gateway@active
 */
void gateway::dpfail(eosio::name coopname, eosio::name admin, uint64_t deposit_id, std::string memo) {
  
  require_auth(admin);

  if (coopname != admin){
    check_auth_or_fail(coopname, admin, "dpcomplete"_n);
  };
  
  deposits_index deposits(_gateway, coopname.value);
  auto deposit = deposits.find(deposit_id);
  eosio::check(deposit -> coopname == coopname, "Указан не верный кооператив");
 
  eosio::check(deposit != deposits.end(), "Объект процессинга не найден");
  
  deposits.modify(deposit, admin, [&](auto &d){
    d.status = "fail"_n;
    d.expired_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch() + _deposit_expiration_seconds);
  }); 

}


/**
 * @brief Создаёт запрос на вывод средств в контракте `gateway`.
 *
 * @details Действие `wthdcreate` используется для создания запроса на вывод средств, включая внутреннее и внешнее количество токенов и связанную заметку.
 *
 * @note Требуется авторизация аккаунта пользователя.
 * @ingroup public_actions
 *
 * @param username Имя пользователя, инициирующего вывод средств.
 * @param coopname Имя кооператива, через который происходит вывод.
 * @param internal_quantity Количество токенов для вывода во внутреннем представлении.
 * @param external_quantity Количество токенов для вывода во внешнем представлении.
 * @param memo Заметка, связанная с операцией вывода средств.
 *
 * Пример
 * 
 * cleos push action gateway wthdcreate '["username", "coopname", "10.0000 SYS", "10.00 USD", "Заметка к выводу"]' -p username@active
 */
void gateway::withdraw(eosio::name coopname, eosio::name username, eosio::asset quantity, document document, std::string bank_data_id, std::string memo){

  require_auth(username);

  withdraws_index withdraws(_gateway, coopname.value);

  uint64_t id = get_global_id(_gateway, "withdraws"_n);

  auto cooperative = get_cooperative_or_fail(coopname);
  cooperative.check_symbol_or_fail(quantity);

  eosio::check(quantity.amount > 0, "Сумма вывода должна быть положительной");
  
  withdraws.emplace(username, [&](auto &d){
    d.id = id;
    d.username = username;
    d.coopname = coopname;
    d.bank_data_id = bank_data_id;
    d.token_contract = cooperative.token_contract;
    d.document = document;
    d.quantity = quantity;
    d.status = "pending"_n;    
    d.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
  });


  //TODO здесь необходимо запросить авторизацию совета и там и заблокировать баланс кошелька
  action(
    permission_level{ _gateway, "active"_n},
    _soviet,
    "withdraw"_n,
    std::make_tuple(coopname, username, id)
  ).send();

  action(
    permission_level{ _gateway, "active"_n},
    _soviet,
    "blockbal"_n,
    std::make_tuple(coopname, username, quantity)
  ).send();


  action(
    permission_level{ _gateway, "active"_n},
    _gateway,
    "newwithdrid"_n,
    std::make_tuple(username, id)
  ).send();

};



void gateway::withdrawauth(eosio::name coopname, uint64_t withdraw_id) {

  require_auth(_soviet);

  withdraws_index withdraws(_gateway, coopname.value);
  
  auto withdraw = withdraws.find(withdraw_id);
  eosio::check(withdraw != withdraws.end(), "Объект процессинга не найден");
  
  withdraws.modify(withdraw, _soviet, [&](auto &d){
    d.status = "authorized"_n;
  });

}


/**
 * @brief Завершает процесс вывода средств в контракте `gateway`.
 *
 * @details Действие `wthdcomplete` используется для обозначения успешного завершения запроса на вывод средств. Оно обновляет статус запроса на "completed" и обновляет заметку.
 *
 * @note Требуется авторизация аккаунта контракта `gateway`.
 * @ingroup public_actions
 *
 * @param withdraw_id Уникальный идентификатор запроса на вывод средств.
 * @param memo Обновлённая заметка, связанная с запросом на вывод.
 *
 * cleos push action gateway wthdcomplete '["12345", "Успешное завершение"]' -p gateway@active
 */
void gateway::wthdcomplete(eosio::name coopname, eosio::name admin, uint64_t withdraw_id, std::string memo){

  require_auth(admin);

  check_auth_or_fail(coopname, admin, "wthdcomplete"_n);
  
  withdraws_index withdraws(_gateway, coopname.value);
  
  auto withdraw = withdraws.find(withdraw_id);
  eosio::check(withdraw != withdraws.end(), "Объект процессинга не найден");
  eosio::check(withdraw -> status == "authorized"_n, "Только принятые заявления на вывод могут быть обработаны");

  withdraws.modify(withdraw, admin, [&](auto &d){
    d.status = "completed"_n;
    d.memo = memo;
  });

  action(
    permission_level{ _gateway, "active"_n},
    _soviet,
    "unblockbal"_n,
    std::make_tuple(coopname, withdraw -> username, withdraw -> quantity)
  ).send();


  action(
    permission_level{ _gateway, "active"_n},
    _soviet,
    "subbalance"_n,
    std::make_tuple(coopname, withdraw -> username, withdraw -> quantity)
  ).send();

  action(
    permission_level{ _gateway, "active"_n},
    _fund,
    "subcirculate"_n,
    std::make_tuple(coopname, withdraw -> quantity)
  ).send();

}


/**
 * @brief Отменяет процесс вывода средств в контракте `gateway`.
 *
 * @details Действие `wthdfail` используется для обозначения неудачного завершения запроса на вывод средств. Оно обновляет статус запроса на "failed", обновляет заметку и возвращает средства пользователю.
 *
 * @note Требуется авторизация аккаунта контракта `gateway`.
 * @ingroup public_actions
 *
 * @param withdraw_id Уникальный идентификатор запроса на вывод средств.
 * @param memo Обновлённая заметка, связанная с запросом на вывод.
 *
 * cleos push action gateway wthdfail '["12345", "Отмена из-за ошибки"]' -p gateway@active
 */
void gateway::wthdfail(eosio::name coopname, eosio::name admin, uint64_t withdraw_id, std::string memo) {

  require_auth(admin);

  check_auth_or_fail(coopname, admin, "wthdfail"_n);

  withdraws_index withdraws(_gateway, coopname.value);
  auto withdraw = withdraws.find(withdraw_id);
  eosio::check(withdraw != withdraws.end(), "Объект процессинга не найден");
  eosio::check(withdraw -> status == "pending"_n, "Неверный статус для провала");

  withdraws.modify(withdraw, _gateway, [&](auto &d){
    d.status = "failed"_n;
    d.memo = memo;
  });

  action(
    permission_level{ _gateway, "active"_n},
    _soviet,
    "unblockbal"_n,
    std::make_tuple(coopname, withdraw -> username, withdraw -> quantity)
  ).send();

}





extern "C" {

/// The apply method implements the dispatch of events to this contract
void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  if (code == _gateway.value) {
    switch (action) {
      EOSIO_DISPATCH_HELPER(
          gateway, (newdepositid)(newwithdrid)

          (deposit)(dpcomplete)(dpfail)
          (withdraw)(withdrawauth)(wthdcomplete)(wthdfail)
      )
    }
  } else {
    if (action == "transfer"_n.value) {
      struct transfer {
        eosio::name from;
        eosio::name to;
        eosio::asset quantity;
        std::string memo;
      };

      auto op = eosio::unpack_action_data<transfer>();

      if (op.to == _gateway) {
        // вызов функции-обработчика пополнения баланса
        struct transfer {
          eosio::name from;
          eosio::name to;
          eosio::asset quantity;
          std::string memo;
        };

        auto op = eosio::unpack_action_data<transfer>();
        
        if (op.to == _gateway) {

          //nothing

        }
      }
    }
  }
};
};
