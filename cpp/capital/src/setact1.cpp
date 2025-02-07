void capital::setact1(eosio::name coopname, eosio::name administrator, eosio::name owner, checksum256 result_hash, checksum256 claim_hash, document act) {
  check_auth_or_fail(_capital, coopname, administrator, "setact1"_n);
  
  verify_document_or_fail(act);
  
  result_index results(_capital, coopname.value);
  auto result_hash_index = results.get_index<"byhash"_n>();
  auto result = result_hash_index.find(result_hash);
  eosio::check(result != result_hash_index.end(), "Объект результата не найден");

  // claims
  claim_index claims(_capital, coopname.value);
  auto claim_hash_index = claims.get_index<"byhash"_n>();
  auto claim = claim_hash_index.find(claim_hash);
  eosio::check(claim != claim_hash_index.end(), "Объект запроса доли не найден");

  // Проверяем статус. 
  eosio::check(result -> status == "decision"_n, "Неверный статус для поставки акта приёма-передачи");
  
  claim_hash_index.modify(claim, coopname, [&](auto &n){
    n.status = "act1"_n;
    n.act1 = act;
  });
};
