void soviet::block(eosio::name coopname, eosio::name admin, eosio::name username, std::string message) {
  
  //блокировку может выписать контракт или администратор
  auto payer = check_auth_and_get_payer_or_fail({_soviet, admin});
  
  //если администратор - проверяем дополнительно его права
  if (payer == admin)
    check_auth_or_fail(coopname, admin, "block"_n);
    
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);
  
  participants.modify(participant, _soviet, [&](auto &row){
      row.status = "blocked"_n;
      row.is_initial = false;
      row.is_minimum = false;
      row.has_vote = false;    
  });
}

void soviet::unblock(eosio::name coopname, eosio::name admin, eosio::name username, bool is_registration, std::string message) {
  //разблокировку может выписать контракт или администратор
  auto payer = check_auth_and_get_payer_or_fail({_soviet, admin});
  
  auto cooperative = get_cooperative_or_fail(coopname);  

  //если администратор - проверяем дополнительно его права
  if (payer == admin)
    check_auth_or_fail(coopname, admin, "unblock"_n);
    
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);
  
  participants.modify(participant, _soviet, [&](auto &row){
      row.status = "accepted"_n;
      row.is_initial = true;
      row.is_minimum = true;
      row.has_vote = true;    
    });
  
  if (is_registration == true) {

    wallets_index wallets(_soviet, coopname.value);
    auto wallet = wallets.find(username.value);
    eosio::check(wallet -> minimum.amount == 0 && wallet -> initial.value().amount == 0, "Пайщик уже совершил вступительный и минимальный паевый взносы");

    eosio::asset minimum = participant -> type.value() == "organization"_n ? cooperative.org_minimum.value() : cooperative.minimum;
    eosio::asset initial = participant -> type.value() == "organization"_n ? cooperative.org_initial.value() : cooperative.initial;
    
    wallets.modify(wallet, _soviet, [&](auto &w) {
      w.minimum = wallet -> minimum + minimum;
      w.initial = wallet -> initial.value() + initial;
    });
    
    //вернуть мин паевый взнос в фонд
    action(
      permission_level{ _gateway, "active"_n},
      _fund,
      "addcirculate"_n,
      std::make_tuple(coopname, minimum)
    ).send();
    
    //вернуть вступительный взнос в фонд
    action(
      permission_level{ _gateway, "active"_n},
      _fund,
      "addinitial"_n,
      std::make_tuple(coopname, initial)
    ).send();      
  };  
};

[[eosio::action]] void soviet::selectbranch(eosio::name coopname, eosio::name username, eosio::name braname, document document){
  require_auth(coopname);
  
  verify_document_or_fail(document);
  
  get_branch_or_fail(coopname, braname);
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);
  
  eosio::check(participant != participants.end(), "Пайщик не найден");
  
  participants.modify(participant, coopname, [&](auto &row){
    row.braname = braname;
  });
  
  // отправляем документ во входящий реестр
  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "newsubmitted"_n,
    std::make_tuple(coopname, username, "selectbranch"_n, uint64_t(0), document)
  ).send();
  
  // отправляем документ в принятый реестр
  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "newresolved"_n,
    std::make_tuple(coopname, username, "selectbranch"_n, uint64_t(0), document)
  ).send();
  
}
  