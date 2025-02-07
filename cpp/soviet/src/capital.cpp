[[eosio::action]] void soviet::claim(eosio::name coopname, eosio::name username, uint64_t result_id, document statement, std::string meta) {
  
  check_auth_or_fail(_soviet, coopname, username, "claim"_n);
  
  decisions_index decisions(_soviet, coopname.value);
  
  auto decision_id = get_id(_soviet, coopname, "decisions"_n);
  
  decisions.emplace(_soviet, [&](auto &d){
    d.id = decision_id;
    d.coopname = coopname;
    d.username = username;
    d.type = _claim_action;
    d.batch_id = result_id;
    d.statement = statement;
    d.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    d.expired_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch() + _decision_expiration);
    d.meta = meta;
  });
  
  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "newsubmitted"_n,
    std::make_tuple(coopname, username, _claim_action, decision_id, statement)
  ).send();
}

[[eosio::action]] void soviet::contribute(eosio::name coopname, eosio::name username, uint64_t claim_id, document statement, std::string meta) {
  
  check_auth_or_fail(_soviet, coopname, username, "contribute"_n);
  
  decisions_index decisions(_soviet, coopname.value);
  
  auto decision_id = get_id(_soviet, coopname, "decisions"_n);
  
  decisions.emplace(_soviet, [&](auto &d){
    d.id = decision_id;
    d.coopname = coopname;
    d.username = username;
    d.type = _contribute_action;
    d.batch_id = claim_id;
    d.statement = statement;
    d.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    d.expired_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch() + _decision_expiration);
    d.meta = meta;
  });
  
  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "newsubmitted"_n,
    std::make_tuple(coopname, username, _contribute_action, decision_id, statement)
  ).send();
}


void soviet::contribute_or_generate_effect(eosio::name executer, eosio::name coopname, uint64_t decision_id) { 

  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  
  action(
      permission_level{ _soviet, "active"_n},
      _capital,
      "authorize"_n,
      std::make_tuple(coopname, decision -> batch_id, decision -> authorization)
  ).send();
  
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