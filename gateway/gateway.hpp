#include <eosio/asset.hpp>
#include <eosio/contract.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>

#include "../common/consts.hpp"
#include "../common/utils.hpp"
#include "../common/drafts.hpp"
#include "../common/coops.hpp"
#include "../common/counts.hpp"
#include "../common/permissions.hpp"
#include "../common/balances.hpp"
#include "../common/accounts.hpp"
#include "../common/programs.hpp"
#include "../common/admins.hpp"

/**
 *  \ingroup public_contracts
 *
 *  @brief Класс `gateway` обеспечивает процессинг банковских и криптовалютных платежей в кооперативах.
 *  @details
 *  Контракт `Gateway` выполняет ключевую роль в обработке платежных операций в блокчейн-системе кооперативов. Он позволяет управлять депозитами и выводом средств, обеспечивая эффективное и безопасное взаимодействие между участниками системы.
 *
 *  Для процессинга депозитов контракт предлагает следующие методы:
 *  - `dpcreate`: Создание инвойса для депозита.
 *  - `dpinit`: Инициализация обработки инвойса.
 *  - `dpupdate`: Обновление статуса инвойса.
 *  - `dpcomplete`: Отметка инвойса как исполненного.
 *  - `dpfail`: Отметка инвойса как провалившегося.
 *
 *  Для вывода средств предусмотрены следующие шаги:
 *  - `Transfer`: Перевод на контракт шлюза, который пополняет внутренний баланс.
 *  - `wthdcreate`: Создание поручения на вывод средств и блокировка внутреннего баланса.
 *  - `wthdupdate`: Обновление статуса поручения на вывод.
 *  - `wthdcomplete`: Отметка поручения на вывод как исполненного.
 *  - `wthdfail`: Отметка поручения на вывод как проваленного.
 *
 *  Для возврата средств с внутреннего баланса, полученных путем перевода на аккаунт контракта, применяется метод `back`.
 *
 */
class [[eosio::contract(GATEWAY)]] gateway : public eosio::contract {

public:
  gateway(eosio::name receiver, eosio::name code,
      eosio::datastream<const char *> ds)
      : eosio::contract(receiver, code, ds) {}

  [[eosio::action]] void newdepositid(eosio::name username, uint64_t id);
  [[eosio::action]] void newwithdrid(eosio::name username, uint64_t id);

  void apply(uint64_t receiver, uint64_t code, uint64_t action);

  [[eosio::action]] void deposit(eosio::name coopname, eosio::name username, eosio::name type, eosio::asset quantity);
  [[eosio::action]] void dpcomplete(eosio::name coopname, eosio::name admin, uint64_t deposit_id, std::string memo);
  [[eosio::action]] void dpfail(eosio::name coopname, eosio::name admin, uint64_t deposit_id, std::string memo);

  [[eosio::action]] void withdrawauth(eosio::name coopname, uint64_t withdraw_id);

  [[eosio::action]] void withdraw(eosio::name coopname, eosio::name username, eosio::asset quantity, document document, std::string bank_data_id, std::string memo);
  [[eosio::action]] void wthdcomplete(eosio::name coopname, eosio::name admin, uint64_t withdraw_id, std::string memo);
  [[eosio::action]] void wthdfail(eosio::name coopname, eosio::name admin, uint64_t withdraw_id, std::string memo);
  
  // [[eosio::action]] void back(eosio::name username, eosio::name token_contract, eosio::asset quantity);



  struct [[eosio::table, eosio::contract(GATEWAY)]] counts : counts_base {};


  /**
   * @ingroup public_tables
   * @brief Таблица `balances` отслеживает внутренние балансы пользователей в контракте GATEWAY.
   */
  struct [[eosio::table, eosio::contract(GATEWAY)]] balances : balances_base {};
};
