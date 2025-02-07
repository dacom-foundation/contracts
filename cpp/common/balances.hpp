
/**
 * @ingroup public_tables
 * @brief Таблица `deposits` отслеживает депозиты в контракте GATEWAY.
 */

struct [[eosio::table, eosio::contract(GATEWAY)]] onedeposit {
    uint64_t id; /*!< Уникальный идентификатор записи депозита */
    eosio::name username; /*!< Имя аккаунта пользователя, совершившего депозит */
    eosio::name coopname; /*!< Имя аккаунта кооператива, в контексте которого совершается депозит */
    eosio::name type; /*!< Тип взноса (deposit | initial) */
    
    eosio::name token_contract; /*!< Имя аккаунта контракта токена для депозита */
    eosio::asset quantity; /*!< Количество средств во внутренней валюте */

    eosio::name status; /*!< Статус депозита */
    std::string link; /*!< Ссылка на дополнительную информацию или внешние данные */
    std::string memo; /*!< Примечание к депозиту */
    eosio::time_point_sec expired_at; ///< Время истечения срока давности

    uint64_t primary_key() const { return id; } /*!< Возвращает id как первичный ключ */
    uint64_t by_username() const { return username.value; } /*!< Индекс по имени пользователя */
    uint64_t by_status() const { return status.value; } /*!< Индекс по статусу депозита */
    uint64_t by_expired() const { return expired_at.sec_since_epoch(); } /*!< Индекс по статусу истечения */
};

typedef eosio::multi_index<
    "deposits"_n, onedeposit,
    eosio::indexed_by<"byusername"_n, eosio::const_mem_fun<onedeposit, uint64_t, &onedeposit::by_username>>,
    eosio::indexed_by<"bystatus"_n, eosio::const_mem_fun<onedeposit, uint64_t, &onedeposit::by_status>>,
    eosio::indexed_by<"byexpired"_n, eosio::const_mem_fun<onedeposit, uint64_t, &onedeposit::by_expired>>
> deposits_index; /*!< Мультииндекс для доступа и манипуляции данными таблицы `deposits` */



/**
 * @ingroup public_tables
 * @brief Таблица `withdraws` отслеживает операции вывода средств в контракте GATEWAY.
 */
struct [[eosio::table, eosio::contract(GATEWAY)]] onewithdraw {
    uint64_t id; /*!< Уникальный идентификатор записи вывода */
    eosio::name username; /*!< Имя пользователя, осуществляющего вывод средств */
    eosio::name coopname; /*!< Имя аккаунта кооператива, в рамках которого осуществляется вывод */
    std::string bank_data_id; /*!< Строковый идентификатор банковских реквизитов в приватной базе данных кооператива */
    eosio::name token_contract; /*!< Имя контракта контракта токена для вывода */
    eosio::asset quantity; /*!< Количество средств для вывода во внутренней валюте */
    eosio::name status; /*!< Статус операции вывода */
    document document; /*!< Заявление на возврат */
    std::string memo; /*!< Примечание к операции вывода */
    eosio::time_point_sec created_at; ///< Время истечения срока давности

    uint64_t primary_key() const { return id; } /*!< Возвращает id как первичный ключ */
    uint64_t by_coopname() const { return coopname.value; } /*!< Индекс по имени кооператива */
    uint64_t by_username() const { return username.value; } /*!< Индекс по имени пользователя */
    uint64_t by_status() const { return status.value; } /*!< Индекс по статусу операции вывода */
    uint64_t by_created() const { return created_at.sec_since_epoch(); } /*!< Индекс по статусу истечения */
};

  typedef eosio::multi_index<
    "withdraws"_n, onewithdraw,
    eosio::indexed_by<"byusername"_n, eosio::const_mem_fun<onewithdraw, uint64_t, &onewithdraw::by_username>>,
    eosio::indexed_by<"bycoopname"_n, eosio::const_mem_fun<onewithdraw, uint64_t, &onewithdraw::by_coopname>>,
    eosio::indexed_by<"bystatus"_n, eosio::const_mem_fun<onewithdraw, uint64_t, &onewithdraw::by_status>>,
    eosio::indexed_by<"bycreated"_n, eosio::const_mem_fun<onewithdraw, uint64_t, &onewithdraw::by_created>>
    > withdraws_index; /*!< Мультииндекс для доступа и манипуляции данными таблицы `withdraws` */



/**
\ingroup public_tables
\brief Структура таблицы баланса.

@details Структура, представляющая записи баланса в контрактах. Используется при отправке токенов в контракты _marketplace или _registrator.

@note Таблица хранится в области памяти пользователя (_registrator | _marketplace, username)
*/
struct balances_base {
  uint64_t id;          /*!< идентификатор баланса */
  eosio::name contract; /*!< имя контракта токена */
  eosio::asset quantity; /*!< количество токенов на балансе */

  uint64_t primary_key() const { return id; } /*!< return id - primary_key */

  uint128_t byconsym() const {
    return combine_ids(contract.value, quantity.symbol.code().raw());
  } /*!< возвращает уникальный индекс, сформированный из значения contract и символа токена */
};

typedef eosio::multi_index<"balances"_n, balances_base, eosio::indexed_by<"byconsym"_n, eosio::const_mem_fun<balances_base, uint128_t, &balances_base::byconsym>>> balances_index; /*!< Тип мультииндекса для таблицы балансов */


// -----------------------------------------------------------------
// Таблица кошельков
// -----------------------------------------------------------------
struct [[eosio::table, eosio::contract(SOVIET)]] onewallet {
  eosio::name username;
  eosio::name coopname;
  eosio::asset available;
  eosio::asset blocked;
  eosio::asset minimum;
  eosio::binary_extension<eosio::asset> initial;
  
  uint64_t primary_key() const { return username.value; } /*!< return username - primary_key */
};

typedef eosio::multi_index<"wallets"_n, onewallet> wallets_index;



// -----------------------------------------------------------------
// Таблица кошельков конкретных программ
// -----------------------------------------------------------------
struct [[eosio::table, eosio::contract(SOVIET)]] progwallet {
  uint64_t id;
  eosio::name coopname;
  uint64_t program_id;
  uint64_t agreement_id;
  eosio::name username;
  eosio::asset available;

  uint64_t primary_key() const { return id; } /*!< return id - primary_key */
  uint64_t by_username() const { return username.value; } /*!< username - secondary_key */
  uint64_t by_program() const { return program_id; } /*!< program_id - secondary_key */
  uint64_t by_agreement() const { return agreement_id; } /*!< agreement_id - secondary_key */

  uint128_t by_username_and_program() const {
    return combine_ids(username.value, program_id);
  } /*!< возвращает уникальный индекс, сформированный из username и program_id */
};

typedef eosio::multi_index<
  "progwallets"_n, 
  progwallet,
  eosio::indexed_by<"byusername"_n, eosio::const_mem_fun<progwallet, uint64_t, &progwallet::by_username>>,
  eosio::indexed_by<"byprogram"_n, eosio::const_mem_fun<progwallet, uint64_t, &progwallet::by_program>>,
  eosio::indexed_by<"byuserprog"_n, eosio::const_mem_fun<progwallet, uint128_t, &progwallet::by_username_and_program>>,
  eosio::indexed_by<"byagreement"_n, eosio::const_mem_fun<progwallet, uint64_t, &progwallet::by_agreement>>
> progwallets_index; /*!< Тип мультииндекса для таблицы кошелька программ */
