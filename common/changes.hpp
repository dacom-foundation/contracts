#pragma once



/**
\ingroup public_tables
\brief Параметры заявки на обмен.
*
* Эта структура предоставляет набор данных, которые требуются для создания или обновления заявки на обмен в контракте "marketplace".
*
* @details Следующие параметры определяют заявку:
* - username: Имя пользователя, инициирующего или обновляющего заявку.
* - parent_id: Идентификатор родительской заявки (если есть).
* - coopname: Имя кооператива.
* - pieces: Количество частей (штук) товара или услуги для обмена.
* - price_for_piece: Цена за единицу (штуку) товара или услуги, выраженная в определенном формате asset.
* - data: Содержит дополнительные данные, специфичные для заявки (например, условия обмена).
* - meta: Метаданные, предоставляющие дополнительную информацию о заявке (например, описание товара или условия обмена).
*/
struct exchange_params {
  eosio::name username; /*!< Имя пользователя, инициирующего или обновляющего заявку */
  uint64_t parent_id; /*!< Идентификатор родительской заявки */
  uint64_t program_id; /*!< Идентификатор программы */
  eosio::name coopname; /*!< Имя кооператива */
  uint64_t pieces; /*!< Количество частей (штук) товара или услуги */
  eosio::asset price_for_piece; /*!< Цена за единицу (штуку) товара или услуги */
  uint64_t product_lifecycle_secs; /*!< Время жизни продукта, заявляемое поставщиком */
  document document; /*!< Сопутствующий подписанный документ на взнос или возврат взноса */
  std::string data; /*!< Дополнительные данные, специфичные для заявки */
  std::string meta; /*!< Метаданные о заявке */
};


/**
 * @brief Таблица обменов для контракта "marketplace"
 * @ingroup public_tables
 * Эта таблица используется для хранения информации об обменных заявках в системе.
 *
 * @param id Идентификатор обмена, используемый как первичный ключ
 * @param parent_id Идентификатор родительской заявки (если есть)
 * @param type Тип обмена (покупка, продажа и т.д.)
 * @param status Статус обмена (например, "опубликовано", "на модерации" и т.д.)
 * @param username Имя пользователя, создавшего заявку
 * @param token_contract Имя контракта токена
 * @param price_for_piece Цена за единицу товара в заявке
 * @param remain_pieces Оставшееся количество товара
 * @param blocked_pieces Заблокированное количество товара
 * @param delivered_pieces Количество доставленного товара
 * @param data Дополнительные данные, связанные с заявкой
 * @param meta Метаданные заявки
 *
 * Дополнительные индексы по status, type и parent_id позволяют искать заявки по этим полям.
 *
 * Пример использования:
 * @code
 * exchange_index exchange(_me, coopname.value);
 * auto exchange_order = exchange.find(id);
 * @endcode
 */

struct [[eosio::table, eosio::contract(MARKETPLACE)]] exchange {
  uint64_t id;                 /*!< идентификатор обмена */
  uint64_t parent_id;          /*!< идентификатор родительской заявки */
  uint64_t program_id;         /*!< идентификатор программы */
  uint64_t contribution_id;    /*!< идентификатор взноса */
  uint64_t decision_id;        /*!< идентификатор решения */
  eosio::name coopname;        /*!< имя аккаунта кооператива */
  eosio::name type;            /*!< тип обмена */
  eosio::name status;          /*!< статус обмена */
  eosio::name username;        /*!< имя аккаунта владельца заявки */
  eosio::name parent_username; /*!< имя аккаунта владельца объявления */
  eosio::name token_contract;  /*!< имя контракта токена */
  eosio::asset price_for_piece;/*!< цена за единицу товара */
  eosio::asset amount;         /*!< цена за всю поставку */
  uint64_t remain_pieces;      /*!< оставшееся количество товара */
  uint64_t blocked_pieces;     /*!< заблокированное количество товара */
  uint64_t delivered_pieces;   /*!< количество доставленного товара */
  std::string data;            /*!< дополнительные данные */
  std::string meta;            /*!< метаданные заявки */

  eosio::name money_contributor;
  eosio::name product_contributor;

  document contribute_product_statement;
  document return_product_statement;
  
  uint64_t contribution_product_decision_id;
  document contribution_product_authorization;
  
  uint64_t return_product_decision_id;
  document return_product_authorization;
  
  document product_contribution_act_validation;
  document product_contribution_act;  

  document product_recieve_act;  
  document product_recieve_act_validation;
  
  uint64_t product_lifecycle_secs;
  
  eosio::time_point_sec warranty_delay_until;
  eosio::time_point_sec deadline_for_receipt;

  uint64_t primary_key() const { return id; } /*!< return id - primary_key */
  uint64_t by_coop() const {return coopname.value;} /*!< кооператив */
  uint64_t by_status() const { return status.value; } /*!< индекс по статусу */
  uint64_t by_program() const { return program_id; } /*!< индекс по программе */
  uint64_t by_decision() const { return decision_id; } /*!< индекс по решению */
  uint64_t by_type() const { return type.value; } /*!< индекс по типу */
  uint64_t by_parent() const { return parent_id; } /*!< индекс по родительскому ID */
  uint64_t by_username() const { return username.value;} /*!< индекс по имени аккаунта */
  uint64_t by_parent_username() const { return parent_username.value;} /*!< индекс по имени аккаунта владельца объявления */
};

typedef eosio::multi_index<
    "exchange"_n, exchange,
    eosio::indexed_by<"bycoop"_n, eosio::const_mem_fun<exchange, uint64_t, &exchange::by_coop>>,
    eosio::indexed_by<"bystatus"_n, eosio::const_mem_fun<exchange, uint64_t, &exchange::by_status>>,
    eosio::indexed_by<"bytype"_n, eosio::const_mem_fun<exchange, uint64_t, &exchange::by_type>>,
    eosio::indexed_by<"byprogram"_n, eosio::const_mem_fun<exchange, uint64_t, &exchange::by_program>>,
    eosio::indexed_by<"bydecision"_n, eosio::const_mem_fun<exchange, uint64_t, &exchange::by_decision>>,
    eosio::indexed_by<"byparent"_n, eosio::const_mem_fun<exchange, uint64_t, &exchange::by_parent>>,
    eosio::indexed_by<"byusername"_n, eosio::const_mem_fun<exchange, uint64_t, &exchange::by_username>>,
    eosio::indexed_by<"bypausername"_n, eosio::const_mem_fun<exchange, uint64_t, &exchange::by_parent_username>>
    >
    exchange_index; /*!< Тип мультииндекса для таблицы обменов */


