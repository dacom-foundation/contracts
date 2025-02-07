void capital::start(name coopname, name administrator, checksum256 result_hash) {
    check_auth_or_fail(_capital, coopname, administrator, "start"_n);

    auto result = get_result(coopname, result_hash);
    eosio::check(result.has_value(), "Результат не найден");
    eosio::check(result -> status == "created"_n, "Только результат в статусе created может быть запущен в распределение");
    
    auto idea = get_idea(coopname, result -> idea_hash);
    eosio::check(idea.has_value(), "Идея не найдена");
    
    result_index results(_capital, coopname.value);
    auto result_for_modify = results.find(result -> id);
    
    results.modify(result_for_modify, coopname, [&](auto& row) {
      row.status = "started"_n;
    });
};
