#pragma once

void check_auth_or_fail(eosio::name coopname, eosio::name admin, eosio::name action_name){
  staff_index staff(_soviet, coopname.value);
  auto administrator = staff.find(admin.value);
  eosio::check(administrator != staff.end(), "Администратор не найден");
  bool has_right = administrator -> has_right(_gateway, action_name);

  eosio::check(has_right, "Недостаточно прав доступа к действию");
};

  