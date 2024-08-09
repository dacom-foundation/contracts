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
* - unit_cost: Цена за единицу (штуку) товара или услуги, выраженная в определенном формате asset.
* - data: Содержит дополнительные данные, специфичные для заявки (например, условия обмена).
* - meta: Метаданные, предоставляющие дополнительную информацию о заявке (например, описание товара или условия обмена).
*/
struct exchange_params {
  eosio::name username; /*!< Имя пользователя, инициирующего или обновляющего заявку */
  uint64_t parent_id; /*!< Идентификатор родительской заявки */
  uint64_t program_id; /*!< Идентификатор программы */
  eosio::name coopname; /*!< Имя кооператива */
  uint64_t pieces; /*!< Количество частей (штук) товара или услуги */
  eosio::asset unit_cost; /*!< Цена за единицу (штуку) товара или услуги */
  uint64_t product_lifecycle_secs; /*!< Время жизни продукта, заявляемое поставщиком */
  std::optional<document> document; /*!< Сопутствующий подписанный документ на взнос или возврат взноса */
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
 * @param unit_cost Цена за единицу товара в заявке
 * @param remain_units Оставшееся количество товара
 * @param blocked_units Заблокированное количество товара
 * @param delivered_units Количество доставленного товара
 * @param data Дополнительные данные, связанные с заявкой
 * @param meta Метаданные заявки
 *
 * Дополнительные индексы по status, type и parent_id позволяют искать заявки по этим полям.
 *
 * Пример использования:
 * @code
 * requests_index exchange(_me, coopname.value);
 * auto exchange_order = exchange.find(id);
 * @endcode
 */

struct [[eosio::table, eosio::contract(MARKETPLACE)]] request {
  uint64_t id;                 /*!< идентификатор обмена */
  uint64_t parent_id;          /*!< идентификатор родительской заявки */
  uint64_t program_id;         /*!< идентификатор программы */
  eosio::name coopname;        /*!< имя аккаунта кооператива */
  eosio::name type;            /*!< тип обмена */
  eosio::name status;          /*!< статус обмена */
  eosio::name username;        /*!< имя аккаунта владельца заявки */
  eosio::name parent_username; /*!< имя аккаунта владельца объявления */
  eosio::name token_contract;  /*!< имя контракта токена */
  
  eosio::asset unit_cost;/*!< себестоимость единицы товара от поставщика */
  eosio::asset supplier_amount; /*!< сумма взноса поставщика */
  eosio::asset total_cost;    /*!< сумма взноса заказчика */
  eosio::asset membership_fee; /*!< членский взнос заказчика */

  uint64_t remain_units;      /*!< оставшееся количество товара */
  uint64_t blocked_units;     /*!< заблокированное количество товара */
  uint64_t delivered_units;   /*!< количество доставленного товара */
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
  uint64_t cancellation_fee; //up to 100
  eosio::asset cancellation_fee_amount; 

  eosio::time_point_sec created_at;
  eosio::time_point_sec accepted_at;
  eosio::time_point_sec supplied_at;
  eosio::time_point_sec delivered_at;
  eosio::time_point_sec recieved_at;
  eosio::time_point_sec completed_at;
  eosio::time_point_sec declined_at;
  eosio::time_point_sec disputed_at;
  eosio::time_point_sec canceled_at;
  // eosio::time_point_sec expired_at;

  eosio::time_point_sec warranty_delay_until;
  eosio::time_point_sec deadline_for_receipt;

  bool is_warranty_return = false;
  uint64_t warranty_return_id;

  uint64_t primary_key() const { return id; } /*!< return id - primary_key */
  uint64_t by_coop() const {return coopname.value;} /*!< кооператив */
  uint64_t by_status() const { return status.value; } /*!< индекс по статусу */
  uint64_t by_program() const { return program_id; } /*!< индекс по программе */
  uint64_t by_type() const { return type.value; } /*!< индекс по типу */
  uint64_t by_parent() const { return parent_id; } /*!< индекс по родительскому ID */
  uint64_t by_username() const { return username.value;} /*!< индекс по имени аккаунта */
  uint64_t by_parent_username() const { return parent_username.value;} /*!< индекс по имени аккаунта владельца объявления */

  uint64_t by_created() const { return created_at.sec_since_epoch();}
  uint64_t by_completed() const { return completed_at.sec_since_epoch();}
  uint64_t by_declined() const { return declined_at.sec_since_epoch();}
  uint64_t by_canceled() const { return canceled_at.sec_since_epoch();}
  uint64_t by_warranty_id() const { return warranty_return_id;}
  // uint64_t by_expired() const { return expired_at.sec_since_epoch();}
};

typedef eosio::multi_index<
    "requests"_n, request,
    eosio::indexed_by<"bycoop"_n, eosio::const_mem_fun<request, uint64_t, &request::by_coop>>,
    eosio::indexed_by<"bystatus"_n, eosio::const_mem_fun<request, uint64_t, &request::by_status>>,
    eosio::indexed_by<"bytype"_n, eosio::const_mem_fun<request, uint64_t, &request::by_type>>,
    eosio::indexed_by<"byprogram"_n, eosio::const_mem_fun<request, uint64_t, &request::by_program>>,
    eosio::indexed_by<"byparent"_n, eosio::const_mem_fun<request, uint64_t, &request::by_parent>>,
    eosio::indexed_by<"byusername"_n, eosio::const_mem_fun<request, uint64_t, &request::by_username>>,
    eosio::indexed_by<"bypausername"_n, eosio::const_mem_fun<request, uint64_t, &request::by_parent_username>>,
    eosio::indexed_by<"bycreated"_n, eosio::const_mem_fun<request, uint64_t, &request::by_created>>,
    eosio::indexed_by<"bycompleted"_n, eosio::const_mem_fun<request, uint64_t, &request::by_completed>>,
    eosio::indexed_by<"bydeclined"_n, eosio::const_mem_fun<request, uint64_t, &request::by_declined>>,
    eosio::indexed_by<"bycanceled"_n, eosio::const_mem_fun<request, uint64_t, &request::by_canceled>>,
    eosio::indexed_by<"bywarrantyid"_n, eosio::const_mem_fun<request, uint64_t, &request::by_warranty_id>>
    // eosio::indexed_by<"byexpired"_n, eosio::const_mem_fun<request, uint64_t, &request::by_expired>>
  >
    requests_index; /*!< Тип мультииндекса для таблицы обменов */


