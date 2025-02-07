void capital::delcreator(name coopname, 
                         name administrator, 
                         checksum256 result_hash,
                         name creator)
{
    check_auth_or_fail(_capital, coopname, administrator, "delcreator"_n);

    result_index results(_capital, coopname.value);
    auto result_hash_index = results.get_index<"byhash"_n>();
    auto result_itr = result_hash_index.find(result_hash);

    eosio::check(result_itr != result_hash_index.end(), "Результат не найден");
    eosio::check(result_itr->status == "created"_n, 
                 "Удаление возможно только для результатов в статусе created");

    creators_index creators(_capital, coopname.value);
    auto creator_username_index = creators.get_index<"byusername"_n>();
    auto creator_username_itr = creator_username_index.find(creator.value);
    eosio::check(creator_username_itr != creator_username_index.end(), "Создатель не найден");

    eosio::asset spended = creator_username_itr->spended;
    int64_t spended_amount = spended.amount;

    // Считаем бонусы на вычет
    auto br = capital::calculcate_capital_amounts(spended_amount);

    eosio::asset creators_bonus(br.creators_bonus, spended.symbol);
    eosio::asset authors_bonus(br.authors_bonus, spended.symbol);
    eosio::asset generated(br.generated, spended.symbol);
    eosio::asset participants_bonus(br.participants_bonus, spended.symbol);
    eosio::asset total(br.total, spended.symbol);

    // Удаляем запись о создателе
    creator_username_index.erase(creator_username_itr);

    // Вычитаем рассчитанные суммы из result
    results.modify(*result_itr, coopname, [&](auto& row) {
        row.creators_amount     -= spended;
        row.creators_bonus      -= creators_bonus;
        row.authors_bonus       -= authors_bonus;
        row.generated_amount    -= generated;
        row.participants_bonus  -= participants_bonus;
        row.total_amount        -= total;

        if (row.creators_count > 0) {
            row.creators_count--;
        }
    });
}