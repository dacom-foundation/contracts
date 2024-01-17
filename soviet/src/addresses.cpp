using namespace eosio;


void soviet::creaddress(eosio::name coopname, eosio::name chairman, eosio::name cooplate, address data, std::string meta) {

  require_auth(chairman);

  auto cooperative = get_cooperative_or_fail(coopname);  

  if (cooplate != ""_n) {
    auto cooperative_plate = get_cooplate_or_fail(coopname, cooplate);       
  }

  addresses_index addresses(_soviet, coopname.value);
  auto id = get_global_id(_soviet, "addresses"_n);

  addresses.emplace(chairman, [&](auto &a){
    a.id = id;
    a.coopname = coopname;
    a.cooplate = cooplate;
    a.data = data;
    a.meta = meta;
  });

}


void soviet::deladdress(eosio::name coopname, eosio::name chairman, uint64_t address_id) {

  require_auth(chairman);

  auto cooperative = get_cooperative_or_fail(coopname);  

  addresses_index addresses(_soviet, coopname.value);
  auto address = addresses.find(address_id);
  eosio::check(address != addresses.end(), "Адрес не найден");
  
  addresses.erase(address);

}


void soviet::editaddress(eosio::name coopname, eosio::name chairman, uint64_t address_id, eosio::name cooplate, address data, std::string meta) {

  require_auth(chairman);

  auto cooperative = get_cooperative_or_fail(coopname);  

  if (cooplate != ""_n) {
    auto cooperative_plate = get_cooplate_or_fail(coopname, cooplate);       
  }

  addresses_index addresses(_soviet, coopname.value);
 
  auto address = addresses.find(address_id);
  eosio::check(address != addresses.end(), "Адрес не найден");

  addresses.modify(address, chairman, [&](auto &a){
    a.cooplate = cooplate;
    a.data = data;
    a.meta = meta;
  });

}