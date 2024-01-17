#include <eosio/action.hpp>
#include <eosio/crypto.hpp>
#include <eosio/permission.hpp>
#include "soviet.hpp"
#include "src/admin.cpp"
#include "src/chairman.cpp"
#include "src/joincoop.cpp"
#include "src/voting.cpp"
#include "src/automator.cpp"
#include "src/marketplace.cpp"
#include "src/programs.cpp"
#include "src/contributions.cpp"
#include "src/addresses.cpp"

using namespace eosio;

[[eosio::action]] void soviet::draft(eosio::name coopname, eosio::name username, uint64_t decision_id, uint64_t batch_id) {
  require_auth(_soviet);

  require_recipient(coopname);
  require_recipient(username);

};

[[eosio::action]] void soviet::statement(eosio::name coopname, eosio::name username, eosio::name action, uint64_t decision_id, uint64_t batch_id, document statement) {
  require_auth(_soviet);

  require_recipient(coopname);
  require_recipient(username);

};


[[eosio::action]] void soviet::act(eosio::name coopname, eosio::name username, eosio::name action, uint64_t decision_id, uint64_t batch_id, document act) {
  require_auth(_soviet);

  require_recipient(coopname);
  require_recipient(username);

};


[[eosio::action]] void soviet::decision(eosio::name coopname, eosio::name username, eosio::name action, uint64_t decision_id, uint64_t batch_id, document decision) {
  require_auth(_soviet);

  require_recipient(coopname);
  require_recipient(username);

};


[[eosio::action]] void soviet::batch(eosio::name coopname, eosio::name action, uint64_t batch_id) {
  require_auth(_soviet);

  require_recipient(coopname);
};




[[eosio::action]] void soviet::program(eosio::name coopname, uint64_t program_id) {
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
  }
}


extern "C" {

  /// The apply method implements the dispatch of events to this contract
  void apply(uint64_t receiver, uint64_t code, uint64_t action) {

    if (code == _soviet.value) {
      switch (action) {

        EOSIO_DISPATCH_HELPER (
            soviet, 
            //main
            (exec)(draft)(statement)(act)(decision)(batch)(program)
        )


        EOSIO_DISPATCH_HELPER (
            soviet, 
            //ADMIN
            (addstaff)(rmstaff)(setrights)(validate)
        )


        EOSIO_DISPATCH_HELPER (
            soviet, 
            //CHAIRMAN
            (authorize)(createboard)(updateboard)
        )

        EOSIO_DISPATCH_HELPER (
            soviet, 
            //VOTING
            (votefor)(voteagainst)(cancelvote)
        )


        EOSIO_DISPATCH_HELPER (
            soviet, 
            //REGACCOUNT
            (joincoop)
        )


        EOSIO_DISPATCH_HELPER (
            soviet, 
            //MARKETPLACE
            (change)(cancelorder)
            (pgivestate)(pgetstate)(pgiveact)(valpgiveact)(pgetact)(valpgetact)
        )

        EOSIO_DISPATCH_HELPER (
            soviet, 
            //AUTOMATOR
            (automate)(disautomate)
        )


        EOSIO_DISPATCH_HELPER (
            soviet, 
            //PROGRAMS
            (createprog)(editprog)(disableprog)
        )

        EOSIO_DISPATCH_HELPER (
            soviet, 
            //CONTRIBUTE
            (addcoopbal)(subcoopbal)(blockprogbal)(unblprogbal)(addbaltoprog)(subbalfrprog)
            (contribute)(withdraw)
        )

        EOSIO_DISPATCH_HELPER (
            soviet, 
            //addresses
            (creaddress)(deladdress)(editaddress)
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

        if (op.to == _soviet) {
          eosio::name coopname = eosio::name(op.memo.c_str());
          soviet::deposit(coopname, op.from, eosio::name(code), op.quantity);
        }
      }
    }
  };

};

