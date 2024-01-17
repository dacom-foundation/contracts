#include <eosio/asset.hpp>
#include <eosio/contract.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>

#include "../common/consts.hpp"
#include "../common/utils.hpp"
#include "../common/drafts.hpp"
#include "../common/accounts.hpp"
#include "../common/counts.hpp"
#include "../common/balances.hpp"
#include "../common/changes.hpp"
#include "../common/coops.hpp"
#include "../common/programs.hpp"

/**
 *  \ingroup public_contracts
 *  @brief Класс `marketplace` предоставляет функционал кооперативного маркетплейса, позволяя пользователям
 *  создавать, обновлять, принимать и отменять заявки на обмен товаров и услуг. Этот контракт служит
 *  центральной точкой для всех операций обмена в рамках кооперативной экосистемы.
 *
 *  Основные функции класса:
 *  - Создание и управление заявками на обмен, включая предложения и заказы.
 *  - Операции обновления, принятия, отказа и завершения обменных операций.
 *  - Модерация и управление публикацией заявок на обмен.
 *  - Административные функции, такие как создание идентификаторов и авторизация операций.
 *  
 *  \note Контракт маркетплейса является центральной точкой экономической активности на платформе.
 */       
class [[eosio::contract(MARKETPLACE)]] marketplace : public eosio::contract {

public:
  marketplace(eosio::name receiver, eosio::name code,
              eosio::datastream<const char *> ds)
      : eosio::contract(receiver, code, ds) {}


  void apply(uint64_t receiver, uint64_t code, uint64_t action);

  // ... определение методов контракта ...
  
  //marketplace.cpp
  [[eosio::action]] void newid(uint64_t id, eosio::name type);

  //soviet.cpp
  [[eosio::action]] void authorize(eosio::name coopname, uint64_t exchange_id);
  
  //change.cpp
  [[eosio::action]] void offer(const exchange_params& params);
  [[eosio::action]] void order(const exchange_params& params);

  static void create(eosio::name type, const exchange_params& params);
  static void create_parent(eosio::name type, const exchange_params& params);
  static void create_child(eosio::name type, const exchange_params& params);
  static void cancel_parent(eosio::name coopname, eosio::name username, uint64_t exchange_id);
  static void cancel_child(eosio::name coopname, eosio::name username, uint64_t exchange_id);

  [[eosio::action]] void accept(eosio::name coopname, eosio::name username, uint64_t exchange_id);
  [[eosio::action]] void decline(eosio::name coopname, eosio::name username, uint64_t exchange_id, std::string meta);
  
  [[eosio::action]] void complete(eosio::name coopname, eosio::name username, uint64_t exchange_id);

  [[eosio::action]] void cancel(eosio::name coopname, eosio::name username, uint64_t exchange_id);
  
  [[eosio::action]] void update(eosio::name coopname, eosio::name username, uint64_t exchange_id, uint64_t remain_pieces, eosio::asset price_for_piece, std::string data, std::string meta);
  [[eosio::action]] void addpieces(eosio::name coopname, eosio::name username, uint64_t exchange_id, uint64_t new_pieces);
  [[eosio::action]] void setcontrib(eosio::name coopname, uint64_t exchange_id, uint64_t contribution_id);
  [[eosio::action]] void setdecision(eosio::name coopname, uint64_t exchange_id, uint64_t decision_id);

  //admins.cpp
  [[eosio::action]] void moderate(eosio::name coopname, eosio::name username, uint64_t exchange_id);
  [[eosio::action]] void prohibit(eosio::name coopname, eosio::name username, uint64_t exchange_id, std::string meta);
  [[eosio::action]] void unpublish(eosio::name coopname, eosio::name username, uint64_t exchange_id);
  [[eosio::action]] void publish(eosio::name coopname, eosio::name username, uint64_t exchange_id);



    
  struct [[eosio::table, eosio::contract("marketplace")]] balances : balances_base {};
  struct [[eosio::table, eosio::contract("marketplace")]] counts : counts_base {};

};
