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


extern "C" {

/// Метод apply реализует диспетчеризацию событий для этого контракта
void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  if (code == _marketplace.value) {
    switch (action) {
      EOSIO_DISPATCH_HELPER(marketplace, (newid)
        //change
        (offer)(order)(complete)(decline)(accept)(setcontrib)(update)
        //admin
        (moderate)(prohibit)(unpublish)(publish)
        //soviet
        (authorize)

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
      // Если перевод направлен на аккаунт контракта, обновляем баланс
      // if (op.to == _marketplace) {
        eosio::check(false, "Контракт не принимает входящие переводы");
      // }
    }
  }
};
};
