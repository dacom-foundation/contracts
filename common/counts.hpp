struct counts_base {
  eosio::name key;
  eosio::name secondary_key;
  uint64_t value;

  uint64_t primary_key() const {
    return key.value;
  } /*!< return id - primary_key */
  uint128_t keyskey() const {
    return combine_ids(key.value, secondary_key.value);
  } /*!< (contract, blocked_now.symbol) - комбинированный secondary_key для
       получения курса по имени выходного контракта и символу */
  uint128_t keyvalue() const {
    return combine_ids(key.value, value);
  } /*!< (contract, blocked_now.symbol) - комбинированный secondary_key для
       получения курса по имени выходного контракта и символу */
};

typedef eosio::multi_index<
    "counts"_n, counts_base,
    eosio::indexed_by<"keyskey"_n, eosio::const_mem_fun<counts_base, uint128_t,
                                                        &counts_base::keyskey>>>
    counts_index;


uint64_t get_id(eosio::name code, eosio::name scope, eosio::name key) {
    counts_index counts(code, scope.value);
    auto count = counts.find(key.value);
    uint64_t id = 1;

    if (count == counts.end()) {
      counts.emplace(code, [&](auto &c) {
        c.key = key;
        c.value = id;
      });
    } else {
      id = count->value + 1;
      counts.modify(count, code, [&](auto &c) { c.value = id; });
    }

    return id;
};

uint64_t get_global_id(eosio::name _me, eosio::name key) {
    counts_index counts(_me, _me.value);
    auto count = counts.find(key.value);
    uint64_t id = 1;

    if (count == counts.end()) {
      counts.emplace(_me, [&](auto &c) {
        c.key = key;
        c.value = id;
      });
    } else {
      id = count->value + 1;
      counts.modify(count, _me, [&](auto &c) { c.value = id; });
    }

    return id;
};
