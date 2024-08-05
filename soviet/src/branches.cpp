using namespace eosio;

[[eosio::action]] void soviet::createbranch(eosio::name coopname, eosio::name chairman, eosio::name braname, std::string name, std::string description, eosio::name authorizer, std::vector<eosio::name> trusted) {
  require_auth(chairman);

  branch_index branches(_soviet, coopname.value);
  auto coop = get_cooperative_or_fail(coopname);

  auto branch_account = get_account_or_fail(braname);
  eosio::check(branch_account.type == "org"_n, "Только аккаунт организации может быть зарегистрирован как кооперативный участок");

  auto authorizer_account = get_account_or_fail(authorizer);
  eosio::check(authorizer_account.type == "individual"_n, "Только физическое лицо может быть назначено председателем кооперативного участка");

  uint64_t count = 0;

  for (const auto& t : trusted) {
    auto trusted_account = get_account_or_fail(t);
    eosio::check(trusted_account.type == "individual"_n, "Только физическое лицо может быть назначено доверенным кооперативного участка");
    count++;
  };

  eosio::check(count <= 3, "Не больше трех доверенных на одном кооперативном участке");

  branches.emplace(chairman, [&](auto &row){
    row.username = braname;
    row.name = name;
    row.description = description;
    row.authorizer = authorizer;
    row.trusted = trusted;
  });

};





[[eosio::action]] void soviet::editbranch(eosio::name coopname, eosio::name chairman, eosio::name braname, std::string name, std::string description, eosio::name authorizer, std::vector<eosio::name> trusted) {
  require_auth(chairman);
  
  auto exist = get_branch_or_fail(coopname, braname);

  auto authorizer_account = get_account_or_fail(authorizer);
  eosio::check(authorizer_account.type == "individual"_n, "Только физическое лицо может быть назначено председателем кооперативного участка");

  uint64_t count = 0;

  for (const auto& username : trusted) {
    auto account = get_account_or_fail(username);
    eosio::check(account.type == "individual"_n, "Только физическое лицо может быть назначено доверенным кооперативного участка");
    count++;
  };

  eosio::check(count <= 3, "Не больше трех доверенных на одном кооперативном участке");
  
  branch_index branches(_soviet, coopname.value);
  
  branches.modify(exist, chairman, [&](auto&b){
    b.name = name;
    b.description = description;
    b.authorizer = authorizer;
    b.trusted = trusted;
  });  

};




[[eosio::action]] void soviet::deletebranch(eosio::name coopname, eosio::name chairman, eosio::name braname) {
  require_auth(chairman);

  auto branch = get_branch_or_fail(coopname, braname);

  branch_index branches(_soviet, coopname.value);

  branches.erase(branch);

};