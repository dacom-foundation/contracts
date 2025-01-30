#include "branch.hpp"

using namespace eosio;

[[eosio::action]] void branch::migrate() {
  require_auth(_branch);
  
}


[[eosio::action]] void branch::init()
{
  require_auth(_system);  
};


[[eosio::action]] void branch::addtrusted(eosio::name coopname, eosio::name braname, eosio::name trusted) {
    check_auth_or_fail(_branch, coopname, coopname, "addtrusted"_n);

    branch_index branches(_branch, coopname.value);
    auto branch = branches.find(braname.value);
    eosio::check(branch != branches.end(), "Кооперативный участок не найден");
    
    auto trusted_account = get_account_or_fail(trusted);
    eosio::check(trusted_account.type == "individual"_n, "Только физическое лицо может быть назначено доверенным кооперативного участка");

    branches.modify(branch, coopname, [&](auto &b) {
        eosio::check(std::find(b.trusted.begin(), b.trusted.end(), trusted) == b.trusted.end(), 
            "Доверенный уже добавлен в кооперативный участок");
        eosio::check(b.trusted.size() < 3, "Не больше трех доверенных на одном кооперативном участке");
        b.trusted.push_back(trusted);
    });
}

[[eosio::action]] void branch::deltrusted(eosio::name coopname, eosio::name braname, eosio::name trusted) {
    check_auth_or_fail(_branch, coopname, coopname, "deltrusted"_n);

    branch_index branches(_branch, coopname.value);
    auto branch = branches.find(braname.value);
    eosio::check(branch != branches.end(), "Кооперативный участок не найден");
    
    branches.modify(branch, coopname, [&](auto &b) {
        auto it = std::find(b.trusted.begin(), b.trusted.end(), trusted);
        eosio::check(it != b.trusted.end(), "Доверенный не найден в кооперативном участке");
        b.trusted.erase(it);
    });
}

[[eosio::action]] void branch::createbranch(eosio::name coopname, eosio::name braname, eosio::name trustee) {
    check_auth_or_fail(_branch, coopname, coopname, "createbranch"_n);

    
    branch_index branches(_branch, coopname.value);
    auto coop = get_cooperative_or_fail(coopname);

    auto authorizer_account = get_account_or_fail(trustee);
    eosio::check(authorizer_account.type == "individual"_n, "Только физическое лицо может быть назначено председателем кооперативного участка");

    branches.emplace(coopname, [&](auto &row) {
      row.braname = braname;
      row.trustee = trustee;
    });
    
    action(
      permission_level{ _branch, "active"_n},
      _registrator,
      "createbranch"_n,
      std::make_tuple(coopname, braname)
    ).send();
    
    uint64_t branch_count = add_branch_count(coopname);
    
    if (!coop.is_branched && branch_count >= 3) { //регистрируем переход на кооперативные участки
      action(
        permission_level{ _branch, "active"_n},
        _registrator,
        "enabranches"_n,
        std::make_tuple(coopname)
      ).send();
    }
    
    //TODO create subfunds and subwallets
    
}

[[eosio::action]] void branch::editbranch(eosio::name coopname, eosio::name braname, eosio::name trustee) {
    check_auth_or_fail(_branch, coopname, coopname, "editbranch"_n);

    
    branch_index branches(_branch, coopname.value);
    auto branch = branches.find(braname.value);
    eosio::check(branch != branches.end(), "Кооперативный участок не найден");
      
    auto authorizer_account = get_account_or_fail(trustee);
    eosio::check(authorizer_account.type == "individual"_n, "Только физическое лицо может быть назначено председателем кооперативного участка");

    branches.modify(branch, coopname, [&](auto &b) {
        b.trustee = trustee;
    });
    
}


[[eosio::action]] void branch::deletebranch(eosio::name coopname, eosio::name braname) {
  check_auth_or_fail(_branch, coopname, coopname, "deletebranch"_n);

  branch_index branches(_branch, coopname.value);
  auto branch = branches.find(braname.value);
  auto coop = get_cooperative_or_fail(coopname);

  eosio::check(branch != branches.end(), "Кооперативный участок не найден");
  
  branches.erase(branch);
  
  // отключаем участников кооператива от кооперативного участка
  action(
    permission_level{ _branch, "active"_n},
    _soviet,
    "deletebranch"_n,
    std::make_tuple(coopname, braname)
  ).send();

  uint64_t new_count = sub_branch_count(coopname);
  
  if (coop.is_branched && new_count < 3) { //отключаем систему КУ, если их меньше 3
    action(
        permission_level{ _branch, "active"_n},
        _registrator,
        "disbranches"_n,
        std::make_tuple(coopname)
      ).send();
  }
};
