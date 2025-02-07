void capital::addcreator(name coopname,
                         name administrator,
                         checksum256 result_hash,
                         name creator,
                         eosio::asset spended)
{
    check_auth_or_fail(_capital, coopname, administrator, "addcreator"_n);
    
    auto result = get_result(coopname, result_hash);
    eosio::check(result.has_value(), "Результат не найден");
    eosio::check(result->status == "created"_n, 
                 "Только результат в статусе created может иметь новых создателей");
    
    eosio::check(spended.symbol == _root_govern_symbol, "Неверный символ для учета взносов");

    auto existed_creator = get_creator(coopname, creator, result_hash);
    eosio::check(!existed_creator.has_value(), "Создатель уже существует");

    // Запись о новом создателе
    creators_index creators(_capital, coopname.value);
    creators.emplace(coopname, [&](auto& row) {
        row.id = get_global_id_in_scope(_capital, coopname, "creators"_n);
        row.username = creator;
        row.spended = spended;
        row.result_hash = result_hash;
        row.idea_hash = result -> idea_hash;
    });

    // --- Расчитываем все бонусы через утилиту ---
    const int64_t spended_amount = spended.amount;
    auto br = capital::calculcate_capital_amounts(spended_amount);

    // Формируем eosio::asset из результатов
    eosio::asset creators_bonus(br.creators_bonus, spended.symbol);
    eosio::asset authors_bonus(br.authors_bonus, spended.symbol);
    eosio::asset generated(br.generated, spended.symbol);
    eosio::asset participants_bonus(br.participants_bonus, spended.symbol);
    eosio::asset total(br.total, spended.symbol);

    // Обновляем запись в таблице results
    result_index results(_capital, coopname.value);
    auto result_for_modyfy = results.find(result -> id);
  
    results.modify(result_for_modyfy, coopname, [&](auto& row) {
        row.creators_amount     += spended;
        row.creators_bonus      += creators_bonus;
        row.authors_bonus       += authors_bonus;
        row.generated_amount    += generated;
        row.participants_bonus  += participants_bonus;
        row.participants_bonus_remain  += participants_bonus;
        row.total_amount        += total;
        row.creators_count++;
    });
}