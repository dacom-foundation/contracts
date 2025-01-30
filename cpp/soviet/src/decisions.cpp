[[eosio::action]] void soviet::freedecision(eosio::name coopname, eosio::name username, document document, std::string meta) {
  
  check_auth_or_fail(_soviet, coopname, username, "freedecision"_n);
  
  decisions_index decisions(_soviet, coopname.value);
  
  auto decision_id = get_id(_soviet, coopname, "decisions"_n);
  
  decisions.emplace(_soviet, [&](auto &d){
    d.id = decision_id;
    d.coopname = coopname;
    d.username = username;
    d.type = _free_decision_action;
    d.batch_id = 0;
    d.statement = document;
    d.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    d.expired_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch() + _decision_expiration);
    d.meta = meta;
  });
  
  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "newsubmitted"_n,
    std::make_tuple(coopname, username, "freedecision"_n, decision_id, document)
  ).send();
}



void soviet::freedecision_effect(eosio::name executer, eosio::name coopname, uint64_t decision_id) { 

  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "newresolved"_n,
      std::make_tuple(coopname, decision -> username, decision -> type, decision_id, decision -> statement)
  ).send();
  
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "newdecision"_n,
      std::make_tuple(coopname, decision -> username, decision -> type, decision_id, decision -> authorization)
  ).send();


  decisions.erase(decision);

};