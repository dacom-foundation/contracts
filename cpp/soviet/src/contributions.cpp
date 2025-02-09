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

void soviet::addmemberfee(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity){
  eosio::check(has_auth(_marketplace) || has_auth(_soviet), "Недостаточно прав доступа");
  auto cooperative = get_cooperative_or_fail(coopname);  
  
  programs_index programs(_soviet, coopname.value);
  auto prg = programs.find(program_id);
  eosio::check(prg != programs.end(), "Программа с указанным program_id не найдена");
  
  auto participant = get_participant_or_fail(coopname, username);
  auto exist_wallet = get_user_program_wallet_or_fail(coopname, username, program_id);
  
  progwallets_index progwallets(_soviet, coopname.value);
  auto wallet = progwallets.find(exist_wallet.id);
  
  progwallets.modify(wallet, _soviet, [&](auto &p) { 
    p.membership_contribution = p.membership_contribution.value_or(asset(0, quantity.symbol)) + quantity;
  });
  
  // Обновляем агрегированный баланс в самой программе (program_id)
  programs.modify(prg, _soviet, [&](auto &p){
    p.membership_contributions = p.membership_contributions.value_or(asset(0, quantity.symbol)) + quantity;
  });
}


void soviet::addbal(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity) {
  eosio::check(has_auth(_marketplace) || has_auth(_soviet) || has_auth(_capital) || has_auth(_gateway), "Недостаточно прав доступа");
  
  programs_index programs(_soviet, coopname.value);
  auto prg = programs.find(program_id);
  eosio::check(prg != programs.end(), "Программа с указанным program_id не найдена");
  
  auto cooperative = get_cooperative_or_fail(coopname);  
  auto participant = get_participant_or_fail(coopname, username);
  auto exist_wallet = get_user_program_wallet_or_fail(coopname, username, program_id);
  
  progwallets_index progwallets(_soviet, coopname.value);
  auto wallet = progwallets.find(exist_wallet.id);
  
  progwallets.modify(wallet, _soviet, [&](auto &b) { 
    b.available += quantity; 
  });
  
  // Обновляем агрегированный баланс в самой программе (program_id)
  programs.modify(prg, _soviet, [&](auto &p) {
    p.available = p.available.value_or(asset(0, quantity.symbol)) + quantity;
    p.share_contributions = p.share_contributions.value_or(asset(0, quantity.symbol)) + quantity;
  });
  
  
}


void soviet::subbal(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity, bool skip_available_check) {
  eosio::check(has_auth(_marketplace) || has_auth(_soviet) || has_auth(_capital) || has_auth(_gateway), "Недостаточно прав доступа");
  
  programs_index programs(_soviet, coopname.value);
  auto prg = programs.find(program_id);
  eosio::check(prg != programs.end(), "Программа с указанным program_id не найдена");
    
  auto cooperative = get_cooperative_or_fail(coopname);  
  auto participant = get_participant_or_fail(coopname, username);
  
  auto exist_wallet = get_user_program_wallet_or_fail(coopname, username, program_id);
  progwallets_index progwallets(_soviet, coopname.value);
  auto wallet = progwallets.find(exist_wallet.id);
  
  //обход проверки положительности при списании по флагу (необходимо для refund при уходе в минус)
  if (!skip_available_check)
    eosio::check(wallet ->available >= quantity, "Недостаточный баланс");

  progwallets.modify(wallet, _soviet, [&](auto &b) { 
    b.available -= quantity; 
  });

  // Уменьшаем агрегированный баланс в самой программе
  programs.modify(prg, _soviet, [&](auto &p){
    eosio::check(p.available.value() >= quantity, "В программе недостаточно доступных средств");
    eosio::check(p.share_contributions.value() >= quantity, "В программе недостаточно средств");
    
    p.available = p.available.value_or(asset(0, quantity.symbol)) - quantity;
    p.share_contributions = p.share_contributions.value_or(asset(0, quantity.symbol)) - quantity;
  });
}


void soviet::blockbal(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity) {
  eosio::check(has_auth(_marketplace) || has_auth(_gateway) || has_auth(_soviet) || has_auth(_capital), "Недостаточно прав доступа");
  
  auto cooperative = get_cooperative_or_fail(coopname);  
  cooperative.check_symbol_or_fail(quantity);
  
  programs_index programs(_soviet, coopname.value);
  auto prg = programs.find(program_id);
  eosio::check(prg != programs.end(), "Программа с указанным program_id не найдена");
  
  auto participant = get_participant_or_fail(coopname, username);
  
  auto exist_wallet = get_user_program_wallet_or_fail(coopname, username, program_id);
  progwallets_index progwallets(_soviet, coopname.value);
  auto wallet = progwallets.find(exist_wallet.id);
  
  eosio::check(wallet -> available >= quantity, "Недостаточно средств на балансе для блокировки");

  progwallets.modify(wallet, _soviet, [&](auto &w){
    w.available -= quantity;
    w.blocked = w.blocked.value_or(asset(0, quantity.symbol)) + quantity;
  });
  
  eosio::check(prg -> available.value() >= quantity, "Недостаточно средств на балансе программы");
    
  programs.modify(prg, _soviet, [&](auto &p){
    p.available = p.available.value_or(asset(0, quantity.symbol)) - quantity;
    p.blocked = p.blocked.value_or(asset(0, quantity.symbol)) + quantity;
  });
  
}


void soviet::unblockbal(eosio::name coopname, eosio::name username, uint64_t program_id, eosio::asset quantity) {
  eosio::check(has_auth(_marketplace) || has_auth(_gateway) || has_auth(_soviet) || has_auth(_capital), "Недостаточно прав доступа");
  eosio::name payer = has_auth(_marketplace) ? _marketplace : _soviet;

  programs_index programs(_soviet, coopname.value);
  auto prg = programs.find(program_id);
  eosio::check(prg != programs.end(), "Программа с указанным program_id не найдена");
  
  auto cooperative = get_cooperative_or_fail(coopname);  
  cooperative.check_symbol_or_fail(quantity);
  
  auto participant = get_participant_or_fail(coopname, username);
  auto exist_wallet = get_user_program_wallet_or_fail(coopname, username, program_id);
  progwallets_index progwallets(_soviet, coopname.value);
  auto wallet = progwallets.find(exist_wallet.id);
  
  eosio::check(wallet -> blocked.value() >= quantity, "Недостаточно средств в блокировке для разблокировки");

  progwallets.modify(wallet, _soviet, [&](auto &w){
    w.available += quantity;
    w.blocked = w.blocked.value_or(asset(0, quantity.symbol)) - quantity;
  });
  
  //разблокируем средства в программе
  eosio::check(prg -> blocked.value() >= quantity, "Недостаточно средств в блокировке программы");
  programs.modify(prg, _soviet, [&](auto &p){
    p.available = p.available.value_or(asset(0, quantity.symbol)) + quantity;
    p.blocked = p.blocked.value_or(asset(0, quantity.symbol)) - quantity;
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



















