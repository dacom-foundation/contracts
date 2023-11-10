#include <eosio/asset.hpp>
#include <eosio/contract.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>

#include "../common/consts.hpp"
#include "../common/utils.hpp"
#include "../common/coops.hpp"
#include "../common/counts.hpp"
#include "../common/permissions.hpp"
#include "../common/balances.hpp"
#include "../common/accounts.hpp"
#include "../common/programs.hpp"

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

  [[eosio::action]] void newid(eosio::name username, uint64_t id);

  void apply(uint64_t receiver, uint64_t code, uint64_t action);

  [[eosio::action]] void dpcreate(eosio::name username, eosio::name coopname, uint64_t program_id, eosio::name purpose, uint64_t secondary_id, eosio::asset internal_quantity, eosio::asset external_quantity, std::string memo);
  [[eosio::action]] void dpinit(uint64_t deposit_id, std::string link, std::string memo);
  [[eosio::action]] void dpupdate(uint64_t deposit_id, std::string memo);
  [[eosio::action]] void dpcomplete(uint64_t deposit_id, std::string memo);
  [[eosio::action]] void dpfail(uint64_t deposit_id, std::string memo);

  [[eosio::action]] void wthdcreate(eosio::name username, eosio::name coopname, eosio::asset internal_quantity, eosio::asset external_quantity, std::string memo);
  [[eosio::action]] void wthdupdate(uint64_t withdraw_id, std::string memo);
  [[eosio::action]] void wthdcomplete(uint64_t withdraw_id, std::string memo);
  [[eosio::action]] void wthdfail(uint64_t withdraw_id, std::string memo);
  
  [[eosio::action]] void back(eosio::name username, eosio::name token_contract, eosio::asset quantity);


/**
 * @ingroup public_tables
 * @brief Таблица `deposits` отслеживает депозиты в контракте GATEWAY.
 */

struct [[eosio::table, eosio::contract("GATEWAY")]] deposits {
    uint64_t id; /*!< Уникальный идентификатор записи депозита */
    eosio::name username; /*!< Имя аккаунта пользователя, совершившего депозит */
    eosio::name coopname; /*!< Имя аккаунта кооператива, в контексте которого совершается депозит */
    uint64_t program_id; /*!< Идентификатор целевой программы  */
    eosio::name purpose; /*!< Цель депозита, например 'changeone' или 'initial' */
    uint64_t secondary_id; /*!< Вторичный идентификатор, связанный с депозитом и целью */
    eosio::name token_contract; /*!< Имя аккаунта контракта токена для депозита */
    eosio::asset internal_quantity; /*!< Количество средств во внутренней валюте */
    eosio::asset external_quantity; /*!< Количество средств во внешней валюте */
    eosio::name status; /*!< Статус депозита */
    std::string link; /*!< Ссылка на дополнительную информацию или внешние данные */
    std::string memo; /*!< Примечание к депозиту */

    uint64_t primary_key() const { return id; } /*!< Возвращает id как первичный ключ */
    uint64_t by_coopname() const { return coopname.value; } /*!< Индекс по названию кооператива */
    uint64_t by_username() const { return username.value; } /*!< Индекс по имени пользователя */
    uint64_t by_status() const { return status.value; } /*!< Индекс по статусу депозита */
};

typedef eosio::multi_index<
    "deposits"_n, deposits,
    eosio::indexed_by<"byusername"_n, eosio::const_mem_fun<deposits, uint64_t, &deposits::by_username>>,
    eosio::indexed_by<"bycoopname"_n, eosio::const_mem_fun<deposits, uint64_t, &deposits::by_coopname>>,
    eosio::indexed_by<"bystatus"_n, eosio::const_mem_fun<deposits, uint64_t, &deposits::by_status>>
> deposits_index; /*!< Мультииндекс для доступа и манипуляции данными таблицы `deposits` */


/**
 * @ingroup public_tables
 * @brief Таблица `withdraws` отслеживает операции вывода средств в контракте GATEWAY.
 */
struct [[eosio::table, eosio::contract("GATEWAY")]] withdraws {
    uint64_t id; /*!< Уникальный идентификатор записи вывода */
    eosio::name username; /*!< Имя пользователя, осуществляющего вывод средств */
    eosio::name coopname; /*!< Имя аккаунта кооператива, в рамках которого осуществляется вывод */
    eosio::name token_contract; /*!< Имя контракта контракта токена для вывода */
    eosio::asset internal_quantity; /*!< Количество средств для вывода во внутренней валюте */
    eosio::asset external_quantity; /*!< Количество средств для вывода во внешней валюте */
    eosio::name status; /*!< Статус операции вывода */
    std::string memo; /*!< Примечание к операции вывода */

    uint64_t primary_key() const { return id; } /*!< Возвращает id как первичный ключ */
    uint64_t by_coopname() const { return coopname.value; } /*!< Индекс по имени кооператива */
    uint64_t by_username() const { return username.value; } /*!< Индекс по имени пользователя */
    uint64_t by_status() const { return status.value; } /*!< Индекс по статусу операции вывода */
};

  typedef eosio::multi_index<
    "withdraws"_n, withdraws,
    eosio::indexed_by<"byusername"_n, eosio::const_mem_fun<withdraws, uint64_t, &withdraws::by_username>>,
    eosio::indexed_by<"bycoopname"_n, eosio::const_mem_fun<withdraws, uint64_t, &withdraws::by_coopname>>,
    eosio::indexed_by<"bystatus"_n, eosio::const_mem_fun<withdraws, uint64_t, &withdraws::by_status>>
    > withdraws_index; /*!< Мультииндекс для доступа и манипуляции данными таблицы `withdraws` */


  struct [[eosio::table, eosio::contract(ANO)]] counts : counts_base {};


  /**
   * @ingroup public_tables
   * @brief Таблица `balances` отслеживает внутренние балансы пользователей в контракте GATEWAY.
   */
  struct [[eosio::table, eosio::contract(ANO)]] balances : balances_base {};
};
