void capital::createidea (
  checksum256 hash,
  eosio::name coopname, 
  eosio::name administrator,
  std::string title, 
  std::string description
) {
  
    check_auth_or_fail(_capital, coopname, administrator, "createidea"_n);
  
    idea_index ideas(_capital, coopname.value);
    auto idx = ideas.get_index<"byhash"_n>();
    auto itr = idx.find(hash);
    
    check(itr == idx.end(), "Идея с указанным хэшем уже существует");

    ideas.emplace(coopname, [&](auto& row) {
      row.id = get_global_id_in_scope(_capital, coopname, "ideas"_n); 
      row.hash = hash;
      row.coopname = coopname;
      row.administrator = administrator;
      row.title = title;
      row.description = description;
    });   
}