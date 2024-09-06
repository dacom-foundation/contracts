// #pragma once

#include <eosio/binary_extension.hpp>
#include <eosio/eosio.hpp>
#include <eosio/ignore.hpp>
#include <eosio/transaction.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>

#include "../common/common.hpp"

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

  [[eosio::action]] void init();
  [[eosio::action]] void migrate();
  
  //soviet.cpp
  [[eosio::action]] void exec(eosio::name executer, eosio::name coopname, uint64_t decision_id);

  //ниже реестр хранилища документов
  
  //черновик пачки
  [[eosio::action]] void newsubmitted(eosio::name coopname, eosio::name username, eosio::name action, uint64_t decision_id, document document);
  
  //документ
  [[eosio::action]] void newresolved(eosio::name coopname, eosio::name username, eosio::name action, uint64_t decision_id, document document);
  [[eosio::action]] void newact(eosio::name coopname, eosio::name username, eosio::name action, uint64_t decision_id, document document);
  [[eosio::action]] void newdecision(eosio::name coopname, eosio::name username,  eosio::name action, uint64_t decision_id, document document);
  [[eosio::action]] void newbatch(eosio::name coopname, eosio::name action, uint64_t batch_id);
  [[eosio::action]] void newprogram(eosio::name coopname, uint64_t program_id);

  //___


  //regaccount.cpp
  [[eosio::action]] void joincoop(eosio::name coopname, eosio::name username, document document);
  [[eosio::action]] void adduser(eosio::name coopname, eosio::name username, eosio::name type, eosio::time_point_sec created_at, eosio::asset initial, eosio::asset minimum, bool spread_initial);
  
  static void joincoop_effect(eosio::name executer, eosio::name coopname, uint64_t decision_id, uint64_t batch_id);

  //automator.cpp
  [[eosio::action]] void automate(eosio::name coopname, uint64_t board_id, eosio::name member, eosio::name action_type, eosio::name provider, std::string encrypted_private_key);
  [[eosio::action]] void disautomate(eosio::name coopname, uint64_t board_id, eosio::name member, uint64_t automation_id );
  static void is_valid_action_for_automation(eosio::name action_type);
  
  //chairman.cpp
  [[eosio::action]] void authorize(eosio::name coopname, eosio::name chairman, uint64_t decision_id, document document);
  [[eosio::action]] void createboard(eosio::name coopname, eosio::name username, eosio::name type, std::vector<board_member> members, std::string name, std::string description);
  [[eosio::action]] void updateboard(eosio::name coopname, eosio::name username, uint64_t board_id, std::vector<board_member> members, std::string name, std::string description);
  
  //admin.cpp
  [[eosio::action]] void addstaff(eosio::name coopname, eosio::name chairman, eosio::name username, std::vector<right> rights, std::string position_title);
  [[eosio::action]] void rmstaff(eosio::name coopname, eosio::name chairman, eosio::name username);
  [[eosio::action]] void setrights(eosio::name coopname, eosio::name chairman, eosio::name username, std::vector<right> rights);
  [[eosio::action]] void validate(eosio::name coopname, eosio::name username, uint64_t decision_id);

  [[eosio::action]] void regpaid(eosio::name coopname, eosio::name username);
  


  //voting.cpp
  [[eosio::action]] void votefor(eosio::name coopname, eosio::name member, uint64_t decision_id);
  [[eosio::action]] void voteagainst(eosio::name coopname, eosio::name member, uint64_t decision_id);
  [[eosio::action]] void cancelvote(eosio::name coopname, eosio::name member, uint64_t decision_id);

  //marketplace.cpp
  [[eosio::action]] void change(eosio::name coopname, eosio::name parent_username, eosio::name username, uint64_t exchange_id, eosio::name money_contributor, eosio::name product_contributor);
  [[eosio::action]] void recieved (eosio::name coopname, uint64_t exchange_id);
  static void change_effect(eosio::name executer, eosio::name coopname, uint64_t decision_id, uint64_t batch_id);


  //programs.cpp
  [[eosio::action]] void createprog(eosio::name coopname, eosio::name username, eosio::name program_type, uint64_t draft_registry_id, std::string title, std::string announce, std::string description, std::string preview, std::string images, eosio::name calculation_type, eosio::asset fixed_membership_contribution, uint64_t membership_percent_fee, std::string meta);
  [[eosio::action]] void editprog(eosio::name coopname, eosio::name username, uint64_t program_id, uint64_t draft_registry_id, std::string title, std::string announce, std::string description, std::string preview, std::string images, std::string meta);
  [[eosio::action]] void disableprog(eosio::name coopname, uint64_t program_id);
  
  [[eosio::action]] void sndagreement(eosio::name coopname, eosio::name username, eosio::name agreement_type, uint64_t program_id, uint64_t draft_registry_id, document document);
  
  [[eosio::action]] void confirmagree(eosio::name coopname, eosio::name administrator, eosio::name username, uint64_t agreement_id);
  [[eosio::action]] void declineagree(eosio::name coopname, eosio::name administrator, eosio::name username, uint64_t agreement_id, std::string comment);
  
  
  //decisions
  [[eosio::action]] void withdraw(eosio::name coopname, eosio::name username, uint64_t withdraw_id);
  

  //contributions.cpp
  [[eosio::action]] void addbalance(eosio::name coopname, eosio::name username, eosio::asset quantity);
  [[eosio::action]] void subbalance(eosio::name coopname, eosio::name username, eosio::asset quantity);
  [[eosio::action]] void blockbal(eosio::name coopname, eosio::name username, eosio::asset quantity);
  [[eosio::action]] void unblockbal(eosio::name coopname, eosio::name username, eosio::asset quantity);
  [[eosio::action]] void addprogbal(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity);
  [[eosio::action]] void subprogbal(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity);
  void withdraw_effect(eosio::name executer, eosio::name coopname, uint64_t decision_id, uint64_t batch_id);


  //addresses.cpp
  [[eosio::action]] void creaddress(eosio::name coopname, eosio::name chairman, eosio::name braname, address_data data);
  [[eosio::action]] void deladdress(eosio::name coopname, eosio::name chairman, uint64_t address_id);
  [[eosio::action]] void editaddress(eosio::name coopname, eosio::name chairman, eosio::name braname, uint64_t address_id, address_data data);


  //branches.cpp
  [[eosio::action]] void createbranch(eosio::name coopname, eosio::name chairman, eosio::name braname, std::string name, std::string description, eosio::name authorizer, std::vector<eosio::name> trusted);
  [[eosio::action]] void editbranch(eosio::name coopname, eosio::name chairman, eosio::name braname, std::string name, std::string description, eosio::name authorizer, std::vector<eosio::name> trusted);
  [[eosio::action]] void deletebranch(eosio::name coopname, eosio::name chairman, eosio::name braname);

  //fund.cpp
  void subaccum_effect(eosio::name executer, eosio::name coopname, uint64_t decision_id, uint64_t program_id);
  [[eosio::action]] void fundwithdraw(eosio::name coopname, eosio::name username, eosio::name type, uint64_t withdraw_id, document document);
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
    
    std::string notice;

    uint64_t primary_key() const {
      return id;
    };

    uint64_t byusername() const {return username.value;}

  };

  typedef eosio::multi_index<"joincoops"_n, joincoops,
    eosio::indexed_by<"byusername"_n, eosio::const_mem_fun<joincoops, uint64_t, &joincoops::byusername>>
  > joincoops_index; 

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



