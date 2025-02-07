void capital::setact2(eosio::name coopname, eosio::name administrator, eosio::name owner, checksum256 result_hash, document act) {
  check_auth_or_fail(_capital, coopname, administrator, "setact2"_n);

  verify_document_or_fail(act);
  
  auto result = get_result(coopname, result_hash);
  eosio::check(result.has_value(), "Результат не найден");
  
  auto claim = get_claim(coopname, owner, result_hash);
  eosio::check(claim.has_value(), "Объект запроса доли не найден");

  // Проверяем статус.
  eosio::check(claim -> status == "act1"_n, "Неверный статус для поставки акта");
  
  claim_index claims(coopname, coopname.value);
  auto claim_for_modify = claims.find(claim -> id);
  
  claims.modify(claim_for_modify, coopname, [&](auto &n){
    n.status = "act2"_n;
    n.act1 = act;
  });
  
  // атомарно добавляем взнос в кошелёк на программу и замораживаем его
  capital::add_capital_to_wallet(coopname, owner, claim_for_modify -> amount);

};
