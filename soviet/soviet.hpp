// #pragma once

#include <eosio/binary_extension.hpp>
#include <eosio/eosio.hpp>
#include <eosio/ignore.hpp>
#include <eosio/transaction.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>

#include "../common/consts.hpp"
#include "../common/utils.hpp"
#include "../common/balances.hpp"
#include "../common/accounts.hpp"
#include "../common/drafts.hpp"
#include "../common/coops.hpp"
#include "../common/changes.hpp"
#include "../common/counts.hpp"
#include "../common/admins.hpp"
#include "../common/programs.hpp"



/**
 *  \ingroup public_contracts
 *
 *  @brief  Класс `soviet` предоставляет средства для управления и работы кооперативов в рамках блокчейн-системы. 
 *  Он служит связующим звеном между различными контрактами и действиями, предоставляя интерфейс для создания, 
 *  автоматизации, управления и голосования по различным решениям, связанным с деятельностью кооператива.
 *  
 *  Основные функции класса:
 *  - Получение и обработка действий от других контрактов.
 *  - Формирование шаблонов решений для голосования членами совета.
 *  - Выполнение действий на основе принятых решений.
 *  - Управление участниками, их правами и автоматическими действиями в рамках кооператива.
 *  
 *  \note Этот класс является основой для реализации логики кооперативов и их взаимодействия внутри блокчейн-среды.
 */
class [[eosio::contract(SOVIET)]] soviet : public eosio::contract {
public:
  using contract::contract;
   //soviet.cpp
  [[eosio::action]] void exec(eosio::name executer, eosio::name coopname, uint64_t decision_id);

  //ниже реестр хранилища документов
  
  //черновик пачки
  [[eosio::action]] void draft(eosio::name coopname, eosio::name username, uint64_t decision_id);
  
  //программа
  [[eosio::action]] void program(eosio::name coopname, uint64_t program_id);

  //документ
  [[eosio::action]] void statement(eosio::name coopname, eosio::name username, eosio::name action, uint64_t decision_id, document document);
  [[eosio::action]] void act(eosio::name coopname, eosio::name username, eosio::name action, uint64_t decision_id, document document);
  [[eosio::action]] void decision(eosio::name coopname, eosio::name username,  eosio::name action, uint64_t decision_id, document document);
  [[eosio::action]] void batch(eosio::name coopname, eosio::name action, uint64_t batch_id);
  //___

  //admin.cpp
  [[eosio::action]] void addstaff(eosio::name coopname, eosio::name chairman, eosio::name username, std::vector<right> rights, std::string position_title);
  [[eosio::action]] void rmstaff(eosio::name coopname, eosio::name chairman, eosio::name username);
  [[eosio::action]] void setrights(eosio::name coopname, eosio::name chairman, eosio::name username, std::vector<right> rights);
  [[eosio::action]] void validate(eosio::name coopname, eosio::name username, uint64_t decision_id);
  
  //regaccount.cpp
  [[eosio::action]] void joincoop(eosio::name coopname, eosio::name username, document document);
  
  static void joincoop_effect(eosio::name executer, eosio::name coopname, uint64_t decision_id, uint64_t batch_id);

  //automator.cpp
  [[eosio::action]] void automate(eosio::name coopname, uint64_t board_id, eosio::name member, eosio::name action_type, eosio::name provider, std::string encrypted_private_key);
  [[eosio::action]] void disautomate(eosio::name coopname, uint64_t board_id, eosio::name member, uint64_t automation_id );
  static void is_valid_action_for_automation(eosio::name action_type);
  
  //chairman.cpp
  [[eosio::action]] void authorize(eosio::name coopname, eosio::name chairman, uint64_t decision_id, document document);
  [[eosio::action]] void createboard(eosio::name coopname, eosio::name chairman, eosio::name type, std::vector<board_member> members, std::string name, std::string description);
  [[eosio::action]] void updateboard(eosio::name coopname, eosio::name chairman, uint64_t board_id, std::vector<board_member> members, std::string name, std::string description);

  //voting.cpp
  [[eosio::action]] void votefor(eosio::name coopname, eosio::name member, uint64_t decision_id);
  [[eosio::action]] void voteagainst(eosio::name coopname, eosio::name member, uint64_t decision_id);
  [[eosio::action]] void cancelvote(eosio::name coopname, eosio::name member, uint64_t decision_id);

  //marketplace.cpp
  [[eosio::action]] void change(eosio::name coopname, eosio::name parent_username, eosio::name username, uint64_t exchange_id, eosio::name money_contributor, eosio::name product_contributor);
  
  [[eosio::action]] void recieved (eosio::name coopname, uint64_t exchange_id);
  [[eosio::action]] void completed(eosio::name coopname, uint64_t exchange_id);


  static void change_effect(eosio::name executer, eosio::name coopname, uint64_t decision_id, uint64_t batch_id);
  [[eosio::action]] void cancelorder(eosio::name coopname, eosio::name username, uint64_t program_id, uint64_t exchange_id, uint64_t contribution_id, eosio::asset quantity);

  //programs.cpp
  [[eosio::action]] void createprog(eosio::name coopname, eosio::name chairman, std::string title, std::string announce, std::string description, std::string preview, std::string images, eosio::asset initial, eosio::asset minimum, eosio::asset maximum, eosio::asset share_contribution, eosio::asset membership_contribution, eosio::name period, eosio::name category, eosio::name calculation_type, uint64_t membership_percent_fee);

  [[eosio::action]] void editprog(eosio::name coopname, uint64_t id, std::string title, std::string announce, std::string description, std::string preview, std::string images, eosio::name category);
  [[eosio::action]] void disableprog(eosio::name coopname, uint64_t id);

  //contributions.cpp
  static void deposit(eosio::name coopname, eosio::name username, eosio::name contract, eosio::asset quantity);
  [[eosio::action]] void withdraw(eosio::name coopname, eosio::name username, eosio::asset quantity);
  [[eosio::action]] void mcontribute(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::name type, uint64_t secondary_id);
  [[eosio::action]] void pcontribute(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::name type, uint64_t secondary_id);

  [[eosio::action]] void addbalance(eosio::name coopname, eosio::name username, eosio::asset quantity);
  [[eosio::action]] void subbalance(eosio::name coopname, eosio::name username, eosio::asset quantity);
  [[eosio::action]] void blockbal(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity);
  [[eosio::action]] void unblockbal(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity);
  [[eosio::action]] void addprogbal(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity);
  [[eosio::action]] void subprogbal(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity);

  //addresses.cpp
  [[eosio::action]] void creaddress(eosio::name coopname, eosio::name chairman, eosio::name cooplate, address data, std::string meta);
  [[eosio::action]] void deladdress(eosio::name coopname, eosio::name chairman, uint64_t address_id); 
  [[eosio::action]] void editaddress(eosio::name coopname, eosio::name chairman, uint64_t address_id, eosio::name cooplate, address data, std::string meta);


};
  

  struct [[eosio::table, eosio::contract(SOVIET)]] counts : counts_base {};
 
/**
\ingroup public_tables
\brief Таблица автоматизированных действий
*
* Таблица содержит набор действий, которые член совета автоматизировал.
*
* @note Таблица хранится в области памяти с именем аккаунта: @p _soviet и скоупом: @p coopname
*/
struct [[eosio::table, eosio::contract(SOVIET)]] automator {
    uint64_t id; ///< Уникальный идентификатор автоматизированного действия
    eosio::name coopname; ///< Имя кооператива, к которому относится данное автоматизированное действие
    uint64_t board_id; ///< Идентификатор совета, который автоматизировал данное действие
    eosio::name member; ///< Член совета, который автоматизировал данное действие
    eosio::name action_type; ///< Тип автоматизированного действия
    eosio::name permission_name; ///< Имя разрешения для авторизации действия
    std::string encrypted_private_key; ///< Зашифрованный приватный ключ для авторизации действия

    uint64_t primary_key() const { return id; }
    uint128_t by_member_and_action_type() const { return combine_ids(member.value, action_type.value); } ///< Индекс по члену совета и типу действия
    uint64_t by_action() const { return action_type.value; } ///< Индекс по типу действия

};


  typedef eosio::multi_index< "automator"_n, automator,
    eosio::indexed_by<"byaction"_n, eosio::const_mem_fun<automator, uint64_t, &automator::by_action>>,
    eosio::indexed_by<"bymembaction"_n, eosio::const_mem_fun<automator, uint128_t, &automator::by_member_and_action_type>>
  > automator_index;


  struct [[eosio::table, eosio::contract(SOVIET)]] autosigner {
    uint64_t decision_id;
    uint64_t primary_key() const { return decision_id; }    
  };

  typedef eosio::multi_index< "autosigner"_n, autosigner> autosigner_index;


  struct [[eosio::table, eosio::contract(SOVIET)]] joincoops {
    uint64_t id;
    eosio::name username;
    bool is_paid = false; 
    document statement;
    
    std::string notice;

    uint64_t primary_key() const {
      return id;
    };
  };

  typedef eosio::multi_index<"joincoops"_n, joincoops> joincoops_index; 

  struct [[eosio::table, eosio::contract(SOVIET)]] changes {
    uint64_t id;
    uint64_t exchange_id; // идентификатор обмена в контракте marketplace
    uint64_t contribution_product_decision_id;
    uint64_t return_product_decision_id;


    uint64_t primary_key() const {
      return id; 
    };

    uint64_t byexchange() const {
      return exchange_id;
    };
  };

  typedef eosio::multi_index<"changes"_n, changes,
    eosio::indexed_by<"byexchange"_n, eosio::const_mem_fun<changes, uint64_t, &changes::byexchange>>
  > changes_index;



