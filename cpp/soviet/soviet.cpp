#include <eosio/action.hpp>
#include <eosio/crypto.hpp>
#include <eosio/permission.hpp>
#include "soviet.hpp"
#include "src/admin.cpp"
#include "src/chairman.cpp"
#include "src/registrator.cpp"
#include "src/voting.cpp"
#include "src/automator.cpp"
#include "src/marketplace.cpp"
#include "src/programs.cpp"
#include "src/contributions.cpp"
#include "src/addresses.cpp"
#include "src/fund.cpp"
#include "src/agreements.cpp"
#include "src/participants.cpp"
#include "src/decisions.cpp"
#include "src/branch.cpp"
#include "src/capital.cpp"

using namespace eosio;
void soviet::migrate() {
    require_auth(_soviet); // Проверяем авторизацию

    cooperatives_index coops(_registrator, _registrator.value);

    for (auto coop_it = coops.begin(); coop_it != coops.end(); ++coop_it) {
        eosio::name coopname = coop_it->username;

        // --- 1. Обновление available у программных кошельков пайщиков ---
        participants_index participants(_soviet, coopname.value);
        progwallets_index progwallets(_soviet, coopname.value);

        for (auto part_it = participants.begin(); part_it != participants.end(); ++part_it) {
            eosio::name username = part_it->username;

            auto user_prog_wallets = progwallets.get_index<"byusername"_n>();
            auto wallet_it = user_prog_wallets.lower_bound(username.value);

            while (wallet_it != user_prog_wallets.end() && wallet_it->username == username) {
              user_prog_wallets.modify(wallet_it, _soviet, [&](auto &w) {
                  w.blocked = asset(0, _root_govern_symbol); ///< недоступные средства паевого взноса
                  w.membership_contribution = asset(0, _root_govern_symbol); ///< внесенный членский взнос  
                  // w.available = new_available;
              });
              ++wallet_it;
            }
        }
        // --- 2. Обновление available у всех программ кооператива ---
        
        // wallets_index wallets(_soviet, coopname.value);
        
        // for (auto wallet_it = wallets.begin(); wallet_it != wallets.end(); ++wallet_it) {
        //   wallets.modify(wallet_it, _soviet, [&](auto &w){
        //     w.initial = coop_it -> initial;
        //   });
        // };
        
        programs_index programs(_soviet, coopname.value);
        for (auto prog_it = programs.begin(); prog_it != programs.end(); ++prog_it) {
            if (!prog_it->share_contributions.has_value()) {
                programs.modify(prog_it, _soviet, [&](auto &pr) {
                    pr.is_can_coop_spend_share_contributions = false;
                    pr.share_contributions = eosio::asset(0, _root_govern_symbol);
                    pr.membership_contributions = eosio::asset(0, _root_govern_symbol);
                    pr.spended = eosio::asset(0, _root_govern_symbol);
                    pr.blocked = eosio::asset(0, _root_govern_symbol);
                    
                });
            }
        }
    }
}





[[eosio::action]] void soviet::init() {
  require_auth(_system);
  
  participants_index participants(_soviet, _provider.value);
    
  participants.emplace(_system, [&](auto &m){
      m.username = _provider_chairman;
      m.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
      m.last_update = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
      m.last_min_pay = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
      m.status = "accepted"_n;
      m.is_initial = true;
      m.is_minimum = true;
      m.has_vote = true;  
      m.type="individual"_n;  
    });

    wallets_index wallets(_soviet, _provider.value);

    wallets.emplace(_system, [&](auto &w) {
      w.username = _provider_chairman;
      w.coopname = _provider;
      w.available = asset(0, _root_govern_symbol);
      w.blocked = asset(0, _root_govern_symbol);
      w.minimum = _provider_minimum; 
    });


    board_member member {
        .username = _provider_chairman,
        .is_voting = true,
        .position_title = "Председатель",
        .position = "chairman"_n
    };

    std::vector<board_member> members;
    members.push_back(member);    

    boards_index boards(_soviet, _provider.value);
    
    boards.emplace(_system, [&](auto &b) {
      b.id = boards.available_primary_key();
      b.type = "soviet"_n;
      b.members = members;
      b.name = "Совет провайдера";
      b.description = "";
      b.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
      b.last_update = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    });

    addresses_index addresses(_soviet, _provider.value);
    address_data data;

    addresses.emplace(_system, [&](auto &a) {
      a.id = 0;
      a.coopname = _provider;
      a.data = data;
    });
    
    //создаём базовые кооперативые соглашения
    soviet::make_base_coagreements(_provider, _root_govern_symbol);
    
    //инициализируем фонды
    action(
      permission_level{ _soviet, "active"_n},
      _fund,
      "init"_n,
      std::make_tuple(_provider, _provider_initial)
    ).send();

}

[[eosio::action]] void soviet::newsubmitted(eosio::name coopname, eosio::name username, eosio::name action, uint64_t decision_id, document document) {
  check_auth_and_get_payer_or_fail({_registrator, _soviet});
  
  require_recipient(coopname);
  require_recipient(username);

};

[[eosio::action]] void soviet::newresolved(eosio::name coopname, eosio::name username, eosio::name action, uint64_t decision_id, document document) {
  check_auth_and_get_payer_or_fail({_registrator, _soviet});

  require_recipient(coopname);
  require_recipient(username);

};

[[eosio::action]] void soviet::newdeclined(eosio::name coopname, eosio::name username, document document) {
  check_auth_and_get_payer_or_fail({_registrator, _soviet});

  require_recipient(coopname);
  require_recipient(username);

};


[[eosio::action]] void soviet::declinedoc(eosio::name coopname, eosio::name username, document document) {
  check_auth_or_fail(_soviet, coopname, username, "declinedoc"_n);

  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "newdeclined"_n,
    std::make_tuple(coopname, username, document)
  ).send();

};


[[eosio::action]] void soviet::newact(eosio::name coopname, eosio::name username, eosio::name action, uint64_t decision_id, document document) {
  require_auth(_soviet);
  
  require_recipient(coopname);
  require_recipient(username);

};


[[eosio::action]] void soviet::newdecision(eosio::name coopname, eosio::name username, eosio::name action, uint64_t decision_id, document document) {
  require_auth(_soviet);

  require_recipient(coopname);
  require_recipient(username);

};


[[eosio::action]] void soviet::newbatch(eosio::name coopname, eosio::name action, uint64_t batch_id) {
  require_auth(_soviet);

  require_recipient(coopname);
};


[[eosio::action]] void soviet::newprogram(eosio::name coopname, uint64_t program_id) {
  require_auth(_soviet);

  require_recipient(coopname);
};


/**
\ingroup public_actions
\brief Исполнение решения совета
*
* Этот метод позволяет исполнить решение совета. Исполнение решения включает в себя проверку, что решение существует, что оно было авторизовано, и что оно еще не было выполнено. В зависимости от типа решения, вызывается соответствующая функция для его реализации.
*
* @param executer Имя аккаунта, который исполняет решение
* @param coopname Имя кооператива
* @param decision_id Идентификатор решения для исполнения
* 
* @note Авторизация требуется от аккаунта: @p executer
*/
void soviet::exec(eosio::name executer, eosio::name coopname, uint64_t decision_id) { 
  require_auth(executer);
  
  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  
  eosio::check(decision != decisions.end(),"Решение не найдено в оперативной памяти");
  eosio::check(decision -> authorized == true, "Только авторизованное решение может быть исполнено");
  
  if (decision -> type == _regaccount_action) {
    soviet::joincoop_effect(executer, coopname, decision->id, decision->batch_id);
  } else if (decision -> type == _change_action){
    soviet::change_effect(executer, coopname, decision->id, decision->batch_id);
  } else if (decision -> type == _withdraw_action){
    soviet::withdraw_effect(executer, coopname, decision->id, decision->batch_id);
  } else if (decision -> type == _afund_withdraw_action) {
    soviet::subaccum_effect(executer, coopname, decision->id, decision->batch_id);
  } else if (decision -> type == _free_decision_action) {
    soviet::freedecision_effect(executer, coopname, decision->id);
  } else if (decision -> type == _contribute_action || decision -> type == _claim_action) {
    soviet::contribute_or_generate_effect(executer, coopname, decision -> id);
  };
}
