using namespace eosio;

// Возвращает текущее значение program.available (если нет - ноль)
inline eosio::asset get_prog_available(const program &prog) {
  if (prog.available.has_value()) {
    return prog.available.value();
  } else {
    return eosio::asset(0, _root_govern_symbol);
  }
}

// Устанавливает значение program.available
inline void set_prog_available(program &prog, const eosio::asset &newval) {
  prog.available = newval;  // достаточно прямого присвоения
}


void soviet::addbalance(eosio::name coopname, eosio::name username, eosio::asset quantity) {
  eosio::check(has_auth(_marketplace) || has_auth(_gateway) || has_auth(_soviet), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;

  auto cooperative = get_cooperative_or_fail(coopname);  
  
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Вы не являетесь пайщиком указанного кооператива");
  
  wallets_index wallets(_soviet, coopname.value);
  auto wallet = wallets.find(username.value);

  wallets.modify(wallet, _soviet, [&](auto &w){
    w.available += quantity;
  });

  // Параллельно добавляем баланс в программу id=1
  addprogbal(coopname, username, 1, quantity);
}

void soviet::subbalance(eosio::name coopname, eosio::name username, eosio::asset quantity, bool skip_available_check) {
  eosio::check(has_auth(_marketplace) || has_auth(_gateway) || has_auth(_soviet), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;

  auto cooperative = get_cooperative_or_fail(coopname);  
  
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Вы не являетесь пайщиком указанного кооператива");

  wallets_index wallets(_soviet, coopname.value);
  auto wallet = wallets.find(username.value);
  
  if (!skip_available_check)
    eosio::check(wallet -> available >= quantity, "Недостаточно средств на балансе");

  wallets.modify(wallet, payer, [&](auto &w){
    w.available -= quantity;
  });
  
  // Параллельно списываем из программы id=1
  subprogbal(coopname, username, 1, quantity);
}



void soviet::blockbal(eosio::name coopname, eosio::name username, eosio::asset quantity) {
  eosio::check(has_auth(_marketplace) || has_auth(_gateway) || has_auth(_soviet) || has_auth(_capital), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;

  auto cooperative = get_cooperative_or_fail(coopname);  
  cooperative.check_symbol_or_fail(quantity);

  participants_index participants(_soviet, coopname.value);

  auto participant = participants.find(username.value);
  
  eosio::check(participant != participants.end(), "Участник не найден");

  wallets_index wallets(_soviet, coopname.value);
  auto wallet = wallets.find(username.value);
  
  eosio::check(wallet -> available >= quantity, "Недостаточно средств на балансе");

  wallets.modify(wallet, payer, [&](auto &w){
    w.available -= quantity;
    w.blocked += quantity;
  });

}



void soviet::unblockbal(eosio::name coopname, eosio::name username, eosio::asset quantity) {
  eosio::check(has_auth(_marketplace) || has_auth(_gateway) || has_auth(_soviet) || has_auth(_capital), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;

  auto cooperative = get_cooperative_or_fail(coopname);  
  cooperative.check_symbol_or_fail(quantity);
  
  participants_index participants(_soviet, coopname.value);

  auto participant = participants.find(username.value);
  eosio::check(participant != participants.end(), "Участник не найден");

  wallets_index wallets(_soviet, coopname.value);
  auto wallet = wallets.find(username.value);
  
  eosio::check(wallet -> blocked >= quantity, "Недостаточно средств в блокировке");

  wallets.modify(wallet, payer, [&](auto &w){
    w.available += quantity;
    w.blocked -= quantity;
  });

}


void soviet::addprogbal(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity) {
  eosio::check(has_auth(_marketplace) || has_auth(_soviet) || has_auth(_capital) || has_auth(_gateway), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;
  
  auto cooperative = get_cooperative_or_fail(coopname);  
  
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Вы не являетесь пайщиком указанного кооператива");

  progwallets_index progwallets(_soviet, coopname.value);

  auto balances_by_username_and_program = progwallets.template get_index<"byuserprog"_n>();
  auto username_and_program_index = combine_ids(username.value, program_id);
  auto balance = balances_by_username_and_program.find(username_and_program_index);
  
  eosio::check(balance != balances_by_username_and_program.end(), "Вы не являетесь участником указанной ЦПП");

  balances_by_username_and_program.modify(balance, payer, [&](auto &b) { 
    b.available += quantity; 
  });
  
  // Обновляем агрегированный баланс в самой программе (program_id)
  programs_index programs(_soviet, coopname.value);
  auto prg = programs.find(program_id);
  eosio::check(prg != programs.end(), "Программа с указанным program_id не найдена");
  programs.modify(prg, payer, [&](auto &p){
    auto curr = get_prog_available(p);
    set_prog_available(p, curr + quantity);
  });
}


void soviet::subprogbal(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity) {
  eosio::check(has_auth(_marketplace) || has_auth(_soviet) || has_auth(_capital) || has_auth(_gateway), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;

  auto cooperative = get_cooperative_or_fail(coopname);  
  
  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);

  eosio::check(participant != participants.end(), "Вы не являетесь пайщиком указанного кооператива");
  
  progwallets_index progwallets(_soviet, coopname.value);

  auto balances_by_username_and_program = progwallets.template get_index<"byuserprog"_n>();
  auto username_and_program_index = combine_ids(username.value, program_id);
  auto balance = balances_by_username_and_program.find(username_and_program_index);

  eosio::check(balance != balances_by_username_and_program.end(), "Баланс не найден");

  eosio::check(balance ->available >= quantity, "Недостаточный баланс");

  balances_by_username_and_program.modify(balance, payer, [&](auto &b) { 
    b.available -= quantity; 
  });

  // Уменьшаем агрегированный баланс в самой программе
  programs_index programs(_soviet, coopname.value);
  auto prg = programs.find(program_id);
  eosio::check(prg != programs.end(), "Программа с указанным program_id не найдена");
  programs.modify(prg, payer, [&](auto &p){
    auto curr = get_prog_available(p);
    eosio::check(curr >= quantity, "В программе недостаточно доступных средств");
    set_prog_available(p, curr - quantity);
  });
}


void soviet::withdraw(eosio::name coopname, eosio::name username, uint64_t withdraw_id) { 

  require_auth(_gateway);

  // auto cooperative = get_cooperative_or_fail(coopname);  
  
  // decisions_index decisions(_soviet, coopname.value);
  // auto decision_id = get_id(_soviet, coopname, "decisions"_n);
    
  // decisions.emplace(_gateway, [&](auto &d){
  //   d.id = decision_id;
    
  //   d.coopname = coopname;
  //   d.username = username;
  //   d.type = _withdraw_action;
  //   d.batch_id = withdraw_id;
  //   d.statement
  //   d.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
  // });

  // TODO получить заявление и зафиксировать его здесь или там

  // action(
  //   permission_level{ _soviet, "active"_n},
  //   _soviet,
  //   "newsubmitted"_n,
  //   std::make_tuple(coopname, username, decision_id)
  // ).send();
  
};


void soviet::withdraw_effect(eosio::name executer, eosio::name coopname, uint64_t decision_id, uint64_t batch_id) { 

  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  eosio::check(decision != decisions.end(), "Решение не найдено");

  withdraws_index withdraws(_gateway, coopname.value);
  auto withdraw = withdraws.find(batch_id);
  eosio::check(withdraw != withdraws.end(), "Вывод не найден");

  action(
      permission_level{ _soviet, "active"_n},
      _gateway,
      "withdrawauth"_n,
      std::make_tuple(coopname, batch_id)
  ).send();
  
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "newresolved"_n,
      std::make_tuple(coopname, decision -> username, _withdraw_action, decision_id, withdraw -> document)
  ).send();
  
  action(
      permission_level{ _soviet, "active"_n},
      _soviet,
      "newdecision"_n,
      std::make_tuple(coopname, decision -> username, _withdraw_action, decision_id, decision -> authorization)
  ).send();

  decisions.erase(decision);
  
}



















