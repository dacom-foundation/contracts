void capital::authorize(eosio::name coopname, uint64_t claim_id, document decision) {
  require_auth(_soviet);
  
  claim_index claims(_capital, coopname.value);
  auto claim = claims.find(claim_id);
  
  eosio::check(claim != claims.end(), "Объект запроса доли не найден");

  // Проверяем статус. 
  eosio::check(claim -> status == "statement"_n, "Неверный статус");
  
  claims.modify(claim, coopname, [&](auto &n){
    n.status = "decision"_n;
    n.decision = decision;
  });
};
