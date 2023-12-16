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
void gateway::newid(eosio::name username, uint64_t id) {
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
 * @param purpose Назначение платежа ('initial' или 'changeone').
 * @param batch_id Вторичный идентификатор, связанный с депозитом.
 * @param internal_quantity Количество во внутреннем формате.
 * @param external_quantity Количество во внешнем формате.
 * @param memo Примечание к депозиту.
 *
 * Пример создания новой записи депозита через cleos:
 * 
 * cleos push action gateway dpcreate '["username", "coopaccount", 123, "initial", 456, "10.0000 SYS", "10.0000 EXT", "Депозит для программы X"]' -p username@active
 */
[[eosio::action]] void gateway::dpcreate(eosio::name creator, eosio::name username, eosio::name coopname, uint64_t program_id, eosio::name type, uint64_t batch_id, eosio::asset internal_quantity, eosio::asset external_quantity, std::string link, std::string memo){

  require_auth(creator);

  deposits_index deposits(_gateway, _gateway.value);

  uint64_t id = get_global_id(_gateway, "deposits"_n);

  auto cooperative = get_cooperative_or_fail(coopname);
  
  if (program_id > 0)
    auto program = get_program_or_fail(coopname, program_id);
  
  eosio::check(type == "change"_n || type == "initial"_n, "Доступные назначения платежа - 'initial' | 'changeone'");
  
  eosio::check(cooperative.initial.symbol == internal_quantity.symbol, "Неверный cимвол токена");
  eosio::check(internal_quantity.amount > 0, "Сумма ввода должна быть положительной");
  eosio::check(external_quantity.amount > 0, "Сумма вывода должна быть положительной");

  deposits.emplace(creator, [&](auto &d) {
    d.id = id;
    d.creator = creator;
    d.username = username;
    d.coopname = coopname;
    d.program_id = program_id;
    d.type = type; 
    d.batch_id = batch_id;
    d.token_contract = cooperative.token_contract;
    d.internal_quantity = internal_quantity;
    d.external_quantity = external_quantity;
    d.status = "waiting"_n;
    d.memo = memo;
    d.link = link;
    d.expired_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch() + 3600);
  });

  action(
    permission_level{ _gateway, "active"_n},
    _gateway,
    "newid"_n,
    std::make_tuple(username, id)
  ).send();

}

/**
 * @brief Завершает обработку депозита в контракте `gateway`.
 *
 * @details Действие `dpcomplete` используется для установки статуса депозита в 'successed' и обновления его заметки.
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
void gateway::dpcomplete(uint64_t deposit_id, std::string memo) {

  deposits_index deposits(_gateway, _gateway.value);
  auto deposit = deposits.find(deposit_id);
  
  eosio::check(deposit != deposits.end(), "Объект процессинга не найден");
  
  require_auth(deposit -> creator);

  // action(
  //     permission_level{ _gateway, "active"_n },
  //     deposit -> token_contract, "issue"_n,
  //     std::make_tuple( deposit -> username, deposit -> internal_quantity, std::string("id: ") + std::to_string(deposit->id)) 
  // ).send();

  deposits.erase(deposit);
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
void gateway::dpfail(uint64_t deposit_id, std::string memo) {

  deposits_index deposits(_gateway, _gateway.value);
  auto deposit = deposits.find(deposit_id);

  require_auth(deposit -> creator);
  eosio::check(deposit != deposits.end(), "Объект процессинга не найден");
  
  deposits.erase(deposit);

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
void gateway::wthdcreate(eosio::name username, eosio::name coopname, eosio::asset internal_quantity, eosio::asset external_quantity, std::string memo){

  require_auth(username);

  withdrawals_index withdrawals(_gateway, _gateway.value);

  uint64_t id = get_global_id(_gateway, "withdrawals"_n);

  auto cooperative = get_cooperative_or_fail(coopname);
  
  eosio::check(cooperative.initial.symbol == internal_quantity.symbol, "Неверный символ токена");
  eosio::check(internal_quantity.amount > 0, "Сумма ввода должна быть положительной");
  eosio::check(external_quantity.amount > 0, "Сумма вывода должна быть положительной");

  sub_balance(_gateway, username, internal_quantity, cooperative.token_contract);

  withdrawals.emplace(username, [&](auto &d){
    d.id = id;
    d.username = username;
    d.coopname = coopname;
    d.token_contract = cooperative.token_contract;
    d.internal_quantity = internal_quantity;
    d.external_quantity = external_quantity;
    d.status = ""_n;
    d.memo = memo;
  });

  action(
    permission_level{ _gateway, "active"_n},
    _gateway,
    "newid"_n,
    std::make_tuple(username, id)
  ).send();

};


/**
 * @brief Обновляет статус запроса на вывод средств в контракте `gateway`.
 *
 * @details Действие `wthdupdate` используется для обновления статуса существующего запроса на вывод средств, устанавливая его в "pending" и обновляя связанную заметку.
 *
 * @note Требуется авторизация аккаунта контракта `gateway`.
 * @ingroup public_actions
 *
 * @param withdraw_id Уникальный идентификатор запроса на вывод средств.
 * @param memo Обновлённая заметка, связанная с запросом на вывод.
 *
 * cleos push action gateway wthdupdate '["12345", "Обновлённая заметка"]' -p gateway@active
 */
void gateway::wthdupdate(uint64_t withdraw_id, std::string memo) {

  require_auth(_gateway);

  withdrawals_index withdrawals(_gateway, _gateway.value);
  
  auto withdraw = withdrawals.find(withdraw_id);
  eosio::check(withdraw != withdrawals.end(), "Объект процессинга не найден");
  
  withdrawals.modify(withdraw, _gateway, [&](auto &d){
    d.status = "pending"_n;
    d.memo = memo;
  });
}


/**
 * @brief Завершает процесс вывода средств в контракте `gateway`.
 *
 * @details Действие `wthdcomplete` используется для обозначения успешного завершения запроса на вывод средств. Оно обновляет статус запроса на "successed" и обновляет заметку.
 *
 * @note Требуется авторизация аккаунта контракта `gateway`.
 * @ingroup public_actions
 *
 * @param withdraw_id Уникальный идентификатор запроса на вывод средств.
 * @param memo Обновлённая заметка, связанная с запросом на вывод.
 *
 * cleos push action gateway wthdcomplete '["12345", "Успешное завершение"]' -p gateway@active
 */
void gateway::wthdcomplete(uint64_t withdraw_id, std::string memo){

  require_auth(_gateway);

  withdrawals_index withdrawals(_gateway, _gateway.value);
  
  auto withdraw = withdrawals.find(withdraw_id);
  eosio::check(withdraw != withdrawals.end(), "Объект процессинга не найден");
  
  withdrawals.modify(withdraw, _gateway, [&](auto &d){
    d.status = "successed"_n;
    d.memo = memo;
  });

  action(
      permission_level{ _gateway, "active"_n },
      withdraw -> token_contract, "retire"_n,
      std::make_tuple( withdraw -> internal_quantity, std::string("id: ") + std::to_string(withdraw_id)) 
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
void gateway::wthdfail(uint64_t withdraw_id, std::string memo) {

  require_auth(_gateway);

  withdrawals_index withdrawals(_gateway, _gateway.value);
  
  auto withdraw = withdrawals.find(withdraw_id);
  eosio::check(withdraw != withdrawals.end(), "Объект процессинга не найден");
  eosio::check(withdraw -> status == "pending"_n, "Неверный статус для провала");

  withdrawals.modify(withdraw, _gateway, [&](auto &d){
    d.status = "failed"_n;
    d.memo = memo;
  });

  action(
      permission_level{ _gateway, "active"_n },
      withdraw -> token_contract, "transfer"_n,
      std::make_tuple( _gateway, withdraw -> username, withdraw -> internal_quantity, std::string("id: ") + std::to_string(withdraw->id)) 
  ).send();
}


/**
 * @brief Возвращает средства пользователю в контракте `gateway`.
 *
 * @details Действие `back` используется для возврата средств пользователю. Оно уменьшает баланс пользователя в контракте и переводит указанную сумму средств обратно на его счет.
 *
 * @note Требуется авторизация со стороны пользователя.
 * @ingroup public_actions
 *
 * @param username Имя пользователя, который запрашивает возврат средств.
 * @param token_contract Контракт токена, с которого будут возвращены средства.
 * @param quantity Количество средств для возврата.
 *
 * cleos push action gateway back '["useraccount", "eosiotoken", "10.0000 EOS"]' -p useraccount@active
 */
void gateway::back(eosio::name username, eosio::name token_contract, eosio::asset quantity) {

  require_auth(username);

  sub_balance(_gateway, username, quantity, token_contract);

  action(
      permission_level{ _gateway, "active"_n },
      token_contract, "transfer"_n,
      std::make_tuple( _gateway, username, quantity, std::string("back")) 
  ).send();
}


extern "C" {

/// The apply method implements the dispatch of events to this contract
void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  if (code == _gateway.value) {
    switch (action) {
      EOSIO_DISPATCH_HELPER(
          gateway, (newid)
          (dpcreate)(dpcomplete)(dpfail)
          (wthdcreate)(wthdupdate)(wthdcomplete)(wthdfail)
          (back)
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

          eosio::name username = eosio::name(op.memo.c_str());
          add_balance(_gateway, username, op.quantity, eosio::name(code));
        }
      }
    }
  }
};
};
