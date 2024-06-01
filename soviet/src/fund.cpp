using namespace eosio;

/**
\ingroup public_actions
\brief 

* @note Авторизация требуется от аккаунта: @p _fund
*/
void soviet::fundwithdraw(eosio::name coopname, eosio::name username, eosio::name type, uint64_t withdraw_id, document document) { 
  require_auth(_fund);
  
  decisions_index decisions(_soviet, coopname.value);
  auto decision_id = get_id(_soviet, coopname, "decisions"_n);
  
  decisions.emplace(_registrator, [&](auto &d){
    d.id = decision_id;
    d.coopname = coopname;
    d.username = username;
    d.type = type;
    d.batch_id = withdraw_id;
    d.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
  });
  
  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "newsubmitted"_n,
    std::make_tuple(coopname, username, "fundwithdraw"_n, decision_id, document)
  ).send();
  
};


void soviet::subaccum_effect(eosio::name executer, eosio::name coopname, uint64_t decision_id, uint64_t secondary_id) { 

  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  
  fundwithdraws_index fundwithdraws(_fund, coopname.value);
  auto withdraw = fundwithdraws.find(secondary_id);
  eosio::check(withdraw != fundwithdraws.end(), "Объект не найден");
  eosio::name username = withdraw -> username;
  document document = withdraw -> document;

  action(
      permission_level{ _soviet, "active"_n},
      _fund,
      "authorize"_n,
      std::make_tuple(coopname, decision -> type, secondary_id, decision -> authorization)
  ).send();
  
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "newresolved"_n,
      std::make_tuple(coopname, withdraw -> username, decision -> type, decision_id, document)
  ).send();
  
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "newdecision"_n,
      std::make_tuple(coopname, withdraw -> username, decision -> type, decision_id, decision -> authorization)
  ).send();


  decisions.erase(decision);

};
