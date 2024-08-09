#include <eosio/asset.hpp>
#include <eosio/contract.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>

#include "../common/utils.hpp"
#include "../common/consts.hpp"
#include "../common/accounts.hpp"
#include "../common/drafts.hpp"
#include "../common/coops.hpp"
#include "../common/permissions.hpp"
#include "../common/rammarket.hpp"
#include "../common/balances.hpp"
#include "../common/admins.hpp"

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
class [[eosio::contract(REGISTRATOR)]] registrator : public eosio::contract
{

public:
  registrator(eosio::name receiver, eosio::name code,
              eosio::datastream<const char *> ds)
      : eosio::contract(receiver, code, ds) {}

  [[eosio::action]] void init();
  [[eosio::action]] void fix();
  [[eosio::action]] void test();
  
  [[eosio::action]] void updateaccnt(eosio::name username, eosio::name account_to_change, std::string meta);
  [[eosio::action]] void updatecoop(eosio::name coopname, eosio::name username, eosio::asset initial, eosio::asset minimum, eosio::asset org_initial, eosio::asset org_minimum, std::string announce, std::string description);

  [[eosio::action]] void confirmreg(eosio::name coopname, eosio::name username);

  [[eosio::action]] void reguser(
      eosio::name registrator,
      eosio::name coopname,
      eosio::name username,
      eosio::name type
      );

  [[eosio::action]] void regcoop(eosio::name registrator, eosio::name coopname, eosio::name username, org_data params);
  
  [[eosio::action]] void joincoop(eosio::name registrator, eosio::name coopname, eosio::name username, document document);

  [[eosio::action]] void verificate(eosio::name username, eosio::name procedure);

  [[eosio::action]] void newaccount(
      eosio::name registrator, eosio::name coopname, eosio::name referer,
      eosio::name username, eosio::public_key public_key, std::string meta);

  [[eosio::action]] void adduser(
    eosio::name registrator, eosio::name coopname, eosio::name referer,
    eosio::name username, eosio::name type, std::string meta);

  [[eosio::action]] void changekey(eosio::name coopname, eosio::name changer, eosio::name username, eosio::public_key public_key);

  // [[eosio::action]] void check(checksum256 hash, public_key public_key, signature signature);

  struct [[eosio::table, eosio::contract(REGISTRATOR)]] balances : balances_base
  {
  };
};
