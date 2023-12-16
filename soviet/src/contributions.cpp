using namespace eosio;

void soviet::subcoopbal(eosio::name coopname, eosio::name username, eosio::asset quantity) {

  eosio::check(has_auth(_marketplace) || has_auth(_soviet), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;

  auto cooperative = get_cooperative_or_fail(coopname);  
  
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Участник не найден");
  
  eosio::check(participant -> available >= quantity, "Недостаточно средств на балансе");

  participants.modify(participant, payer, [&](auto &p) {
    p.available -= quantity;
  });
}


void soviet::unblprogbal(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity) {
  eosio::check(has_auth(_marketplace) || has_auth(_soviet), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;

  auto cooperative = get_cooperative_or_fail(coopname);  
  
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Участник не найден");
  
  progwallets_index progwallets(_soviet, coopname.value);

  auto balances_by_username_and_program = progwallets.template get_index<"byuserprog"_n>();
  auto username_and_program_index = combine_ids(username.value, program_id);
  auto balance = balances_by_username_and_program.find(username_and_program_index);

  eosio::check(balance != balances_by_username_and_program.end(), "Баланс программы не обнаружен");
  eosio::check(balance -> blocked >= quantity, "Недостаточно средств на балансе");

  balances_by_username_and_program.modify(balance, payer, [&](auto &b) { 
    b.available += quantity; 
    b.blocked -= quantity; 
  });

}



void soviet::addbaltoprog(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity) {
  eosio::check(has_auth(_marketplace) || has_auth(_soviet), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;

  auto cooperative = get_cooperative_or_fail(coopname);  
  
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Участник не найден");
  
  progwallets_index progwallets(_soviet, coopname.value);

  auto balances_by_username_and_program = progwallets.template get_index<"byuserprog"_n>();
  auto username_and_program_index = combine_ids(username.value, program_id);
  auto balance = balances_by_username_and_program.find(username_and_program_index);

  
  if (balance == balances_by_username_and_program.end()) {
    progwallets.emplace(payer, [&](auto &b) {
      b.id = progwallets.available_primary_key();
      b.available = quantity;
      b.blocked = asset(0, quantity.symbol);
      b.program_id = program_id;
      b.coopname = coopname;
      b.username = username;
    });
  } else {

    balances_by_username_and_program.modify(balance, payer, [&](auto &b) { 
      b.available += quantity; 
    });

  };
}


void soviet::blockprogbal(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity) {
  eosio::check(has_auth(_marketplace) || has_auth(_soviet), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;

  auto cooperative = get_cooperative_or_fail(coopname);  
  
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Участник не найден");

  progwallets_index progwallets(_soviet, coopname.value);

  auto balances_by_username_and_program = progwallets.template get_index<"byuserprog"_n>();
  auto username_and_program_index = combine_ids(username.value, program_id);
  auto balance = balances_by_username_and_program.find(username_and_program_index);
  eosio::check(balance -> available >= quantity, "Недостаточный баланс");

  if (balance == balances_by_username_and_program.end()) {
    progwallets.emplace(payer, [&](auto &b) {
      b.id = progwallets.available_primary_key();
      b.available = asset(0, quantity.symbol);
      b.blocked = quantity;
      b.program_id = program_id;
      b.coopname = coopname;
      b.username = username;
    });

  } else {
    balances_by_username_and_program.modify(balance, payer, [&](auto &b) { 
      b.available -= quantity; 
      b.blocked += quantity; 
    });
  };
}


void soviet::subbalfrprog(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity) {
  eosio::check(has_auth(_marketplace) || has_auth(_soviet), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;

  auto cooperative = get_cooperative_or_fail(coopname);  
  
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Участник не найден");
  
  progwallets_index progwallets(_soviet, coopname.value);

  auto balances_by_username_and_program = progwallets.template get_index<"byuserprog"_n>();
  auto username_and_program_index = combine_ids(username.value, program_id);
  auto balance = balances_by_username_and_program.find(username_and_program_index);

  eosio::check(balance != balances_by_username_and_program.end(), "Баланс не найден");

  eosio::check(balance -> available >= quantity, "Недостаточный баланс");

  balances_by_username_and_program.modify(balance, payer, [&](auto &b) { 
    b.available -= quantity; 
  });
}



void soviet::addcoopbal(eosio::name coopname, eosio::name username, eosio::asset quantity) {

  eosio::check(has_auth(_marketplace) || has_auth(_soviet), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;

  auto cooperative = get_cooperative_or_fail(coopname);  
  
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Участник не найден");
  
  participants.modify(participant, payer, [&](auto &p) {
    p.available += quantity;
  });
}

void soviet::contribute(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity, eosio::name type, uint64_t batch_id) { 
  eosio::check(has_auth(_marketplace) || has_auth(username), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : username;
  
  cntrbutions_index contributions(_soviet, coopname.value);
  auto contribution_id = get_global_id(_soviet, "cntrbutions"_n);
  
  programs_index programs(_soviet, coopname.value);
  auto existing_program = programs.find(program_id);

  eosio::check(existing_program != programs.end(), "Программа не найдена.");
  eosio::check(type == "change"_n, "Неподдерживаемый тип операции");

  contributions.emplace(payer, [&](auto &c) {
    c.id = contribution_id;
    c.type = type;
    c.batch_id = batch_id;
    c.coopname = coopname;
    c.program_id = program_id;
    c.username = username;
    c.initial = quantity;
    c.now = quantity;
    c.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    c.last_update = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
  });

  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);
  print("available2: ", participant -> available);

  eosio::check(participant != participants.end(), "Участник не найден");
  eosio::check(participant -> is_active(), "Участник не активен");
  eosio::check(participant -> available >= quantity, "Недостаточно средств для взноса");

  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "subcoopbal"_n,
    std::make_tuple(coopname, username, quantity)
  ).send();

  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "addbaltoprog"_n,
    std::make_tuple(coopname, username, program_id, quantity)
  ).send();

  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "blockprogbal"_n,
    std::make_tuple(coopname, username, program_id, quantity)
  ).send();

  if (type == "change"_n) {
    action(
      permission_level{ _soviet, "active"_n},
      _marketplace,
      "setcontrib"_n,
      std::make_tuple(coopname, batch_id, contribution_id)
    ).send();
  }
};



void soviet::deposit(eosio::name coopname, eosio::name username, eosio::name contract, eosio::asset quantity) {
  require_auth(username);

  auto cooperative = get_cooperative_or_fail(coopname);  
  cooperative.check_contract_or_fail(contract);

  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Участник не найден");
  eosio::check(participant -> is_active(), "Участник не активен");
  
  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "addcoopbal"_n,
    std::make_tuple(coopname, username, quantity)
  ).send();

};



void soviet::withdraw(eosio::name coopname, eosio::name username, eosio::asset quantity) { 
  eosio::check(has_auth(_soviet) || has_auth(_marketplace) || has_auth(username), "Недостаточно прав доступа");

  auto cooperative = get_cooperative_or_fail(coopname);  
  
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Участник не найден");
  eosio::check(participant -> is_active(), "Участник не активен");
  eosio::check(participant -> available >= quantity, "Недостаточно доступных средств на балансе кошелька для списания");

  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "subbalance"_n,
    std::make_tuple(coopname, username, quantity)
  ).send();

  action(
      permission_level{ _soviet, "active"_n },
      cooperative.token_contract, "transfer"_n,
      std::make_tuple( _soviet, username, quantity, std::string("Возврат из кошелька")) 
  ).send();

};


//Инвестиционный сценарий: необходимо производить возврат через совет
// void soviet::withdrawprog(eosio::name coopname, eosio::name username, eosio::asset quantity) { 
  // require_auth(username);

// ???/
  
  // sub_to_program_wallet(coopname, username, quantity, program_id);
  // auto cooperative = get_cooperative_or_fail(coopname);  
  
  // participants_index participants(_soviet, coopname.value);
  // auto participant = participants.find(username.value);

  // eosio::check(participant != participants.end(), "Участник не найден");
  // eosio::check(participant -> is_active(), "Участник не активен");
  // eosio::check(participant -> available >= quantity, "Недостаточно доступных средств на балансе для списания");

  // action(
  //     permission_level{ _soviet, "active"_n },
  //     cooperative.token_contract, "transfer"_n,
  //     std::make_tuple( _soviet, username, quantity, std::string("Возврат из внутреннего кошелька")) 
  // ).send();
// };


