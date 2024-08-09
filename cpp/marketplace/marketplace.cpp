#include "marketplace.hpp"
#include <eosio/transaction.hpp>
#include "src/soviet.cpp"
#include "src/admins.cpp"
#include "src/change.cpp"

/**
 * @brief Пустой метод регистрации нового идентификатора
 * @ingroup public_actions
 * Этот метод используется для возврата информации из контракта.
 * @param id идентификатор
 * @param type тип идентификатора
 */
[[eosio::action]] void marketplace::newid(uint64_t id, eosio::name type) {
  require_auth(_marketplace);
};
