void capital::createclaim(name coopname, name administrator, name owner, checksum256 result_hash) {
  check_auth_or_fail(_capital, coopname, administrator, "createclaim"_n);
  
  // извлекаем программу и кошелек пайщика в программе
  auto capital_program = get_capital_program_or_fail(coopname);
  auto capital_wallet = get_capital_wallet(coopname, owner);
  eosio::check(!capital_wallet.has_value(), "Для участия необходимо подписать соглашение целевой потребительской программы!");
    
  // извлекаем результат
  auto result = get_result(coopname, result_hash);
  eosio::check(result.has_value(), "Результат не найден");
  
  // извлекаем идею
  auto idea = get_idea(coopname, result -> idea_hash);
  eosio::check(idea.has_value(), "Идея не найдена");
  
  // извлекаем клайм, и падаем, если он уже есть
  auto existing_claim = get_claim(coopname, owner, result_hash);
  eosio::check(!existing_claim.has_value(), "Объект запроса доли уже существует");
  
  uint64_t author_amount = 0;
  uint64_t creator_amount = 0;
  uint64_t creator_bonus_amount = 0;
  uint64_t participant_amount = 0;
  
  // считаем авторское вознаграждение
  auto author = get_result_author(coopname, owner, result_hash);
  if (author.has_value()) {
    author_amount = uint64_t(double(author->shares) * double(result->authors_bonus.amount) / double(result->authors_shares));
  }
  
  // считаем вознаграждение создателя
  auto creator = get_creator(coopname, owner, result_hash);
  if (creator.has_value()) {
    creator_amount = creator -> spended.amount;
    double creator_bonus_share = double(creator -> spended.amount) / double(result -> creators_amount.amount);
    creator_bonus_amount = uint64_t(creator_bonus_share * double(result -> creators_bonus.amount));
  }  
  
  // считаем премию пайщика
  if (
    capital_program.available -> amount > 0 &&
    capital_wallet -> available.amount > 0 &&
    result -> participants_bonus_remain.amount > 0) 
  {
    double participant_share = double(capital_wallet -> available.amount) / double(capital_program.available -> amount);
    participant_amount = uint64_t(participant_share * double(result -> participants_bonus.amount));

    if (participant_amount > result -> participants_bonus_remain.amount) {
      participant_amount = result -> participants_bonus_remain.amount;
    }
  }
  
  //DEBUG
  print("author_amount: ", author_amount);
  print("creator_amount: ", creator_amount);
  print("creator_bonus_amount: ", creator_bonus_amount);
  print("participant_amount:", participant_amount);
  
  eosio::asset amount = eosio::asset(author_amount + creator_amount + creator_bonus_amount + participant_amount, _root_govern_symbol);

  // создаём объект запроса доли
  claim_index claims(_capital, coopname.value);
  uint64_t claim_id = get_global_id_in_scope(_capital, coopname, "claims"_n);
  
  claims.emplace(coopname, [&](auto &n){
    n.id = claim_id;
    n.owner = owner;
    n.result_hash = result_hash;
    n.coopname = coopname;
    n.status = "pending"_n;
    n.amount = amount;
  });

  // обновляем оставшиеся выплаты пайщикам
  result_index results(_capital, coopname.value);
  auto result_for_modify = results.find(result -> id);
  
  results.modify(result_for_modify, coopname, [&](auto &r){
    r.participants_bonus_remain -= asset(participant_amount, _root_govern_symbol);
  });
}
