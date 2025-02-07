
void capital::setstatement(name coopname, name administrator, name owner, checksum256 result_hash, document statement) {
  check_auth_or_fail(_capital, coopname, administrator, "setstatement"_n);

  // проверяем заявление
  verify_document_or_fail(statement);

  // извлекаем клайм
  auto claim = get_claim(coopname, owner, result_hash);
  eosio::check(claim.has_value(), "Объект запроса доли не найден");
  eosio::check(claim->status == "pending"_n, "Нельзя изменить статус клайма");

  // обновляем клайм, добавляя заявление
  claim_index claims(_capital, coopname.value);
  auto claim_for_modify = claims.find(claim->id);
  
  claims.modify(claim_for_modify, coopname, [&](auto &c){
    c.status = "statement"_n;
    c.statement = statement;
  });

  // Отправляем заявление в совет
  action(permission_level{_capital, "active"_n}, _soviet, "claim"_n, 
      std::make_tuple(coopname, owner, claim->id, statement))
  .send();
}
