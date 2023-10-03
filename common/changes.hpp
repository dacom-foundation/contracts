#pragma once
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
 * exchange_index exchange(_me, _me.value);
 * auto exchange_order = exchange.find(id);
 * @endcode
 */
struct [[eosio::table, eosio::contract(MARKETPLACE)]] exchange {
  uint64_t id;                 /*!< идентификатор обмена */
  uint64_t parent_id;          /*!< идентификатор родительской заявки */
  eosio::name coop_username;      /*!< имя аккаунта кооператива */
  eosio::name type;            /*!< тип обмена */
  eosio::name status;          /*!< статус обмена */
  eosio::name username;        /*!< имя пользователя */
  eosio::name token_contract;        /*!< имя контракта токена */
  eosio::asset price_for_piece;/*!< цена за единицу товара */
  uint64_t remain_pieces;      /*!< оставшееся количество товара */
  uint64_t blocked_pieces;     /*!< заблокированное количество товара */
  uint64_t delivered_pieces;   /*!< количество доставленного товара */
  std::string data;            /*!< дополнительные данные */
  std::string meta;            /*!< метаданные заявки */

  uint64_t primary_key() const { return id; } /*!< return id - primary_key */
  uint64_t by_coop() const {return coop_username.value;} /*!< кооператив */
  uint64_t by_status() const { return status.value; } /*!< индекс по статусу */
  uint64_t by_type() const { return type.value; } /*!< индекс по типу */
  uint64_t by_parent() const { return parent_id; } /*!< индекс по родительскому ID */
};

typedef eosio::multi_index<
    "exchange"_n, exchange,
    eosio::indexed_by<"bycoop"_n, eosio::const_mem_fun<exchange, uint64_t, &exchange::by_coop>>,
    eosio::indexed_by<"bystatus"_n, eosio::const_mem_fun<exchange, uint64_t, &exchange::by_status>>,
    eosio::indexed_by<"bytype"_n, eosio::const_mem_fun<exchange, uint64_t, &exchange::by_type>>,
    eosio::indexed_by<"byparent"_n, eosio::const_mem_fun<exchange, uint64_t, &exchange::by_parent>>>
    exchange_index; /*!< Тип мультииндекса для таблицы обменов */


