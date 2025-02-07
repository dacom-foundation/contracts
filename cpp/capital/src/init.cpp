void capital::init(eosio::name coopname, eosio::name initiator, uint64_t program_id) {
  check_auth_or_fail(_capital, coopname, initiator, "init"_n);
  
  global_state_table global_state_inst(_self, _self.value);
  auto itr = global_state_inst.find(0);
  eosio::check(itr == global_state_inst.end(), "Контракт уже инициализирован для кооператива");
  
  //TODO check program_id for exist
    
  global_state gs {
    .coopname = coopname,
    .program_id = program_id,
  };
  
  global_state_inst.emplace(initiator, [&](auto& s) {
      s = gs;
  });
}

