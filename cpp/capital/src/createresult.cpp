void capital::createresult(name coopname, name administrator, checksum256 idea_hash, checksum256 result_hash) {
    check_auth_or_fail(_capital, coopname, administrator, "generate"_n);

    auto idea = get_idea(coopname, idea_hash);
    eosio::check(idea.has_value(), "Идея не найдена");
    

    // Проверяем, существует ли уже RESULT
    result_index results(_capital, coopname.value);
    auto result = get_result(coopname, result_hash);
    eosio::check(!result.has_value(), "Объект результата уже существует");

    // Создаём RESULT
    results.emplace(coopname, [&](auto &n){
      n.id = get_global_id_in_scope(_capital, coopname, "results"_n);
      n.coopname = coopname;
      n.idea_hash = idea_hash;
      n.hash = result_hash;
      n.authors_shares = idea -> authors_shares;
      n.authors_count = idea -> authors_count;
    });
    
    authors_index authors(_capital, coopname.value);
    auto authors_hash_index = authors.get_index<"byideahash"_n>();
    
    // Перебираем всех авторов с данным idea_hash
    auto author_itr = authors_hash_index.lower_bound(idea_hash);
    
    result_authors_index result_authors(_capital, coopname.value);
    
    // Копируем запись автора идеи в result_authors
    while(author_itr != authors_hash_index.end() && author_itr->idea_hash == idea_hash) {
        result_authors.emplace(coopname, [&](auto &n){
            n.id = get_global_id_in_scope(_capital, coopname, "resauthors"_n);
            n.username = author_itr->username;
            n.result_hash = result_hash;
            n.idea_hash = idea_hash;
            n.shares = author_itr -> shares;
        });
        
        author_itr++;
    }
}