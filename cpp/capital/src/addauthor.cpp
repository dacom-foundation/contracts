void capital::addauthor(name coopname, name administrator, checksum256 idea_hash, name author, uint64_t shares) {
    check_auth_or_fail(_capital, coopname, administrator, "addauthor"_n);
    eosio::check(shares <= HUNDR_PERCENTS, "Количество shares должно быть меньше или равно 1000000");
    
    auto idea = get_idea(coopname, idea_hash);
    
    eosio::check(idea.has_value(), "Идея с указанным хэшем не найдена");

    authors_index authors(_capital, coopname.value);
    auto idea_author_index = authors.get_index<"byideaauthor"_n>();

    uint128_t combined_id = combine_checksum_ids(idea_hash, author);
    auto author_itr = idea_author_index.find(combined_id);
    
    authors.emplace(coopname, [&](auto& row) {
        row.id = get_global_id_in_scope(_capital, coopname, "authors"_n); 
        row.idea_hash = idea_hash;
        row.username = author;
        row.shares = shares;
    });

    idea_index ideas(_capital, coopname.value);
    auto idea_for_modify = ideas.find(idea -> id);
    
    // Обновляем shares и authors_count в идее
    ideas.modify(idea_for_modify, coopname, [&](auto& row) {
        row.authors_shares += shares;
        row.authors_count++;
    });
}
