#include <eosio/asset.hpp>
#include <eosio/contract.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>


#include "../common/consts.hpp"
#include "../common/coops.hpp"
#include "../common/permissions.hpp"
#include "../common/rammarket.hpp"
#include "../common/utils.hpp"
#include "../common/balances.hpp"
#include "../common/users.hpp"


/**
 *  \ingroup public_contracts
 *
 *  @brief  Класс `registrator` служит воротами в блокчейн-систему для новых участников, позволяя регистрировать аккаунты,
 *  а также карточки физических и юридических лиц. Через данный контракт участники могут посылать заявления в
 *  совет кооперативов на вступление и получать подтверждения о принятии их заявлений.
 *
 *  Основные функции класса:
 *  - Регистрация новых аккаунтов с различными параметрами (CPU, NET, RAM и т.д.).
 *  - Регистрация карточек физических и юридических лиц.
 *  - Обновление метаданных пользователей.
 *  - Отправка заявлений на вступление в любой кооператив.
 *  - Создание и изменение ключей доступа к аккаунтам.
 *  
 *  \note Этот класс служит основой для регистрации и идентификации участников в блокчейн-среде.
 */
class [[eosio::contract(REGISTRATOR)]] registrator : public eosio::contract {

public:
  registrator(eosio::name receiver, eosio::name code,
              eosio::datastream<const char *> ds)
      : eosio::contract(receiver, code, ds) {}

  [[eosio::action]] void update(eosio::name username, std::string meta);
  [[eosio::action]] void confirmreg(eosio::name coop_username, eosio::name member, std::string position_title, eosio::name position);

  [[eosio::action]] void reguser(
     eosio::name registrator,
     eosio::name username,
     storage storage
  );

  [[eosio::action]] void regorg(eosio::name registrator, eosio::name username, org_data new_org);

  [[eosio::action]] void joincoop(eosio::name coop_username, eosio::name username, std::string position_title, eosio::name position, std::string ricardian_data, std::string statement_hash);

  [[eosio::action]] void verificate(eosio::name username);

  [[eosio::action]] void newaccount(
    eosio::name payer, eosio::name referer,
    eosio::name username, eosio::public_key public_key,
    std::string meta);

  [[eosio::action]] void changekey(eosio::name username,
                                   eosio::public_key public_key);

  void apply(uint64_t receiver, uint64_t code, uint64_t action);
  
  /**
     * @brief      Таблица хранения глобальной сети партнёров
     * @ingroup public_tables
     * @contract _me
     * @scope _me
     * @table partners
     * @details Таблица хранит реферальные связи партнёров, их профили и глобальные статусы.
  */
  struct [[eosio::table, eosio::contract(REGISTRATOR)]] balances : balances_base {};
  
};
