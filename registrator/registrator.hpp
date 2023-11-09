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
#include "../common/accounts.hpp"
#include "../common/drafts.hpp"

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
  [[eosio::action]] void confirmreg(eosio::name coopname, eosio::name member);

  [[eosio::action]] void reguser(
     eosio::name username,
     storage storage
  );

  [[eosio::action]] void regorg(eosio::name registrator, eosio::name username, org_data params);

  [[eosio::action]] void joincoop(eosio::name coopname, eosio::name username, signed_doc signed_doc);

  [[eosio::action]] void verificate(eosio::name username, eosio::name procedure);

  [[eosio::action]] void newaccount(
    eosio::name registrator, eosio::name referer,
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
  

  static void add_balance(eosio::name source, eosio::name username, eosio::asset quantity,
                                    eosio::name contract) {
    // Если баланс не найден, создаем новую запись.
    // В противном случае, увеличиваем существующий баланс.

    
    eosio::check(username != ""_n, "В поле memo должен быть указан получатель баланса");
    balances_index balances(source, username.value);

    auto balances_by_contract_and_symbol =
        balances.template get_index<"byconsym"_n>();
    auto contract_and_symbol_index =
        combine_ids(contract.value, quantity.symbol.code().raw());

    auto balance =
        balances_by_contract_and_symbol.find(contract_and_symbol_index);

    if (balance == balances_by_contract_and_symbol.end()) {
      balances.emplace(source, [&](auto &b) {
        b.id = balances.available_primary_key();
        b.contract = contract;
        b.quantity = quantity;
      });
    } else {
      balances_by_contract_and_symbol.modify(
          balance, source, [&](auto &b) { b.quantity += quantity; });
    };
  }


  static void sub_balance(eosio::name source, eosio::name username, eosio::asset quantity,
                                eosio::name contract) {
    // Если после вычитания баланс равен нулю, удаляем запись.
    // В противном случае, уменьшаем существующий баланс.
    
    balances_index balances(source, username.value);

    auto balances_by_contract_and_symbol =
        balances.template get_index<"byconsym"_n>();
    auto contract_and_symbol_index =
        combine_ids(contract.value, quantity.symbol.code().raw());

    auto balance =
        balances_by_contract_and_symbol.find(contract_and_symbol_index);

    eosio::check(balance != balances_by_contract_and_symbol.end(),
                 "Баланс не найден");

    eosio::check(balance->quantity >= quantity, "Недостаточный баланс");

    if (balance->quantity == quantity) {

      balances_by_contract_and_symbol.erase(balance);

    } else {

      balances_by_contract_and_symbol.modify(
          balance, source, [&](auto &b) { b.quantity -= quantity; });
    }
  }
};
