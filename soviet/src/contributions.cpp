using namespace eosio;

void soviet::addbalance(eosio::name coopname, eosio::name username, eosio::asset quantity) {
  print("on add balance");
  eosio::check(has_auth(_marketplace) || has_auth(_soviet), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;

  auto cooperative = get_cooperative_or_fail(coopname);  
  
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Вы не являетесь пайщиком указанного кооператива");
  
  participants.modify(participant, payer, [&](auto &p) {
    p.available += quantity;
  });
}

void soviet::subbalance(eosio::name coopname, eosio::name username, eosio::asset quantity) {
  print("on sub balance");
  eosio::check(has_auth(_marketplace) || has_auth(_soviet), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;

  auto cooperative = get_cooperative_or_fail(coopname);  
  
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Вы не являетесь пайщиком указанного кооператива");
  
  eosio::check(participant -> available >= quantity, "Недостаточно средств на балансе");

  participants.modify(participant, payer, [&](auto &p) {
    p.available -= quantity;
  });
}



void soviet::blockbal(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity) {
  print("on blockbal");

  eosio::check(has_auth(_marketplace) || has_auth(_soviet), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;

  auto cooperative = get_cooperative_or_fail(coopname);  
  cooperative.check_symbol_or_fail(quantity);

  participants_index participants(_soviet, coopname.value);

  auto participant = participants.find(username.value);

  eosio::check(participant -> available >= quantity, "Недостаточно средств в блокировке");

  participants.modify(participant, payer, [&](auto& p) {
      p.available -= quantity;
      p.blocked += quantity;
  });

  // participants_index participants(_soviet, coopname.value);
  // auto participant = participants.find(username.value);

  // eosio::check(participant != participants.end(), "Вы не являетесь пайщиком указанного кооператива");

  // wallets_index wallets(_soviet, coopname.value);

  // auto balances_by_username_and_program = wallets.template get_index<"byuserprog"_n>();
  // auto username_and_program_index = combine_ids(username.value, program_id);
  // auto balance = balances_by_username_and_program.find(username_and_program_index);
  // eosio::check(balance -> available >= quantity, "Недостаточный баланс");

  // if (balance == balances_by_username_and_program.end()) {
  //   wallets.emplace(payer, [&](auto &b) {
  //     b.id = wallets.available_primary_key();
  //     b.available = asset(0, quantity.symbol);
  //     b.blocked = quantity;
  //     b.program_id = program_id;
  //     b.coopname = coopname;
  //     b.username = username;
  //   });

  // } else {
  //   balances_by_username_and_program.modify(balance, payer, [&](auto &b) { 
  //     b.available -= quantity; 
  //     b.blocked += quantity; 
  //   });
  // };
}



void soviet::unblockbal(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity) {
  print("on unblockbal");
  eosio::check(has_auth(_marketplace) || has_auth(_soviet), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;

  auto cooperative = get_cooperative_or_fail(coopname);  
  cooperative.check_symbol_or_fail(quantity);
  
  participants_index participants(_soviet, coopname.value);

  auto participant = participants.find(username.value);

  eosio::check(participant -> blocked >= quantity, "Недостаточно средств в блокировке");

  participants.modify(participant, payer, [&](auto& p) {
      p.available += quantity;
      p.blocked -= quantity;
  });
  // participants_index participants(_soviet, coopname.value);
  // auto participant = participants.find(username.value);

  // eosio::check(participant != participants.end(), "Вы не являетесь пайщиком указанного кооператива");
  
  // wallets_index wallets(_soviet, coopname.value);

  // auto balances_by_username_and_program = wallets.template get_index<"byuserprog"_n>();
  // auto username_and_program_index = combine_ids(username.value, program_id);
  // auto balance = balances_by_username_and_program.find(username_and_program_index);

  // eosio::check(balance != balances_by_username_and_program.end(), "Баланс программы не обнаружен");
  // eosio::check(balance -> blocked >= quantity, "Недостаточно средств на балансе");

  // balances_by_username_and_program.modify(balance, payer, [&](auto &b) { 
  //   b.available += quantity; 
  //   b.blocked -= quantity; 
  // });

}



void soviet::addprogbal(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity) {
  print("on addprogbal");
  eosio::check(has_auth(_marketplace) || has_auth(_soviet), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;

  auto cooperative = get_cooperative_or_fail(coopname);  
  
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Вы не являетесь пайщиком указанного кооператива");
  
  wallets_index wallets(_soviet, coopname.value);

  auto balances_by_username_and_program = wallets.template get_index<"byuserprog"_n>();
  auto username_and_program_index = combine_ids(username.value, program_id);
  auto balance = balances_by_username_and_program.find(username_and_program_index);

  
  if (balance == balances_by_username_and_program.end()) {
    wallets.emplace(payer, [&](auto &b) {
      b.id = wallets.available_primary_key();
      b.available = quantity;
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


void soviet::subprogbal(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity) {
  print("on subprogbal");
  eosio::check(has_auth(_marketplace) || has_auth(_soviet), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;

  auto cooperative = get_cooperative_or_fail(coopname);  
  
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Вы не являетесь пайщиком указанного кооператива");
  
  wallets_index wallets(_soviet, coopname.value);

  auto balances_by_username_and_program = wallets.template get_index<"byuserprog"_n>();
  auto username_and_program_index = combine_ids(username.value, program_id);
  auto balance = balances_by_username_and_program.find(username_and_program_index);

  eosio::check(balance != balances_by_username_and_program.end(), "Баланс не найден");

  eosio::check(balance -> available >= quantity, "Недостаточный баланс");

  if (balance -> available > quantity) {
    
    balances_by_username_and_program.modify(balance, payer, [&](auto &b) { 
      b.available -= quantity; 
    });

  } else {
  
    balances_by_username_and_program.erase(balance);

  }
}



void soviet::mcontribute(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::name type, uint64_t secondary_id) { 
  eosio::check(has_auth(_marketplace) || has_auth(username), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : username;
  
  programs_index programs(_soviet, coopname.value);
  auto existing_program = programs.find(program_id);

  exchange_index exchange(_marketplace, coopname.value);
  auto request = exchange.find(secondary_id);
  eosio::check(request != exchange.end(), "Заявка не обнаружена");

  eosio::check(existing_program != programs.end(), "Программа не найдена.");
  eosio::check(type == "change"_n, "Неподдерживаемый тип операции");

  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);
  
  eosio::check(participant != participants.end(), "Вы не являетесь пайщиком указанного кооператива");
  eosio::check(participant -> is_active(), "Ваш аккаунт не активен в указанном кооперативе");
  
  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "blockbal"_n,
    std::make_tuple(coopname, username, program_id, request -> amount)
  ).send();

  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "addprogbal"_n,
    std::make_tuple(coopname, username, program_id, request -> amount)
  ).send();

};


//Вызвать при совершении взноса имуществом поставщиком из кооплейса для выдачи заблокированного баланса в ЦПП кошелька и добавление баланса к ЦПП не кошелька
void soviet::pcontribute(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::name type, uint64_t secondary_id) {
  require_auth(_marketplace);

  print("on soviet contribute");
  programs_index programs(_soviet, coopname.value);
  auto existing_program = programs.find(program_id);

  eosio::check(existing_program != programs.end(), "Программа не найдена.");
  eosio::check(type == "change"_n, "Неподдерживаемый тип операции");

  exchange_index exchange(_marketplace, coopname.value);
  auto request = exchange.find(secondary_id);
  eosio::check(request != exchange.end(), "Заявка не обнаружена");

  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Вы не являетесь пайщиком указанного кооператива");
  eosio::check(participant -> is_active(), "Ваш аккаунт не активен в указанном кооперативе");
  

  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "addbalance"_n,
    std::make_tuple(coopname, username, request -> amount)
  ).send();


  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "blockbal"_n,
    std::make_tuple(coopname, username, program_id, request -> amount)
  ).send();


  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "addprogbal"_n,
    std::make_tuple(coopname, username, program_id, request -> amount)
  ).send();

}


void soviet::deposit(eosio::name coopname, eosio::name username, eosio::name contract, eosio::asset quantity) {
  require_auth(username);

  //этот метод должен вызываться советом после принятия взноса (exec)

  auto cooperative = get_cooperative_or_fail(coopname);  
  cooperative.check_contract_or_fail(contract);

  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Вы не являетесь пайщиком указанного кооператива");
  eosio::check(participant -> is_active(), "Ваш аккаунт не активен в указанном кооперативе");
  
  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "addbalance"_n,
    std::make_tuple(coopname, username, quantity)
  ).send();

};



//TODO 
// здесь вместо перевода на баланс должна быть инициализация возврата денег по заявлению  
void soviet::withdraw(eosio::name coopname, eosio::name username, eosio::asset quantity) { 

  eosio::check(has_auth(_soviet) || has_auth(_marketplace) || has_auth(username), "Недостаточно прав доступа");

  auto cooperative = get_cooperative_or_fail(coopname);  
  
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Вы не являетесь пайщиком указанного кооператива");
  eosio::check(participant -> is_active(), "Ваш аккаунт не активен в указанном кооперативе");
  eosio::check(participant -> available >= quantity, "Недостаточно доступных средств на балансе кошелька для списания");

  action(
    permission_level{ _soviet, "active"_n},
    _soviet,
    "subbalance"_n,
    std::make_tuple(coopname, username, quantity)
  ).send();

};

