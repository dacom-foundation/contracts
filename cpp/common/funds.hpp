// // фонд членских взносов
struct [[eosio::table, eosio::contract(FUND)]] fundwallet {
  uint64_t id = 0;  ///< только нулевой идентификатор
  eosio::name coopname;  ///< идентификатор кооператива
  eosio::asset circulating;  ///< индикаторный счет паевого фонда
  eosio::asset
      membership;  ///< индикаторный счет всех поступивших членских взносов
  eosio::asset accumulated;  ///< собрано фонды накопления
  eosio::asset withdrawed;  ///< индикаторный счет вывода из фондов накопления
  eosio::asset available;  ///< доступно для списания в фондах
  eosio::asset expended;  ///< всего списано через фонды

  uint64_t primary_key() const { return id; }  ///< первичный ключ
};

typedef eosio::multi_index<"fundwallet"_n, fundwallet> fundwallet_index;

struct simple_wallet {
  eosio::asset available;
  eosio::asset withdrawed;
};

// кошелёк кооператива
struct [[eosio::table, eosio::contract(FUND)]] coopwallet {
  uint64_t id = 0;  ///< только нулевой идентификатор

  eosio::name coopname;  ///< идентификатор кооператива

  simple_wallet circulating_account;  ///< паевой счет

  simple_wallet initial_account;  ///< счет вступительных взносов

  simple_wallet accumulative_account;  ///< сводный накопительный счет

  simple_wallet accumulative_expense_account;  ///< накопительный счет списания

  uint64_t primary_key() const { return id; }  ///< первичный ключ
};

typedef eosio::multi_index<"coopwallet"_n, coopwallet> coopwallet_index;

struct [[eosio::table, eosio::contract(FUND)]] accfund {  // фонды накопления
  uint64_t id;  ///
  eosio::name coopname;  ///< идентификатор кооператива
  eosio::name
      contract;  ///< внешний контракт, которому передано управление фондом

  std::string name;         ///< название фонда
  std::string description;  ///< описание фонда

  uint64_t percent;  ///< 1000000 == 100%,
  eosio::asset available;  ///< количество средств, накопленных в фонде
  eosio::asset withdrawed;  ///< количество средств, изъятых из фонда накопления

  uint64_t primary_key() const {
    return id;
  }  ///< Первичный ключ для индексации по идентификатору фонда
};

typedef eosio::multi_index<"accfunds"_n, accfund> accfunds_index;

struct [[eosio::table, eosio::contract(FUND)]] expfund {  // фонды списания
  uint64_t id;           ///< идентификатор
  eosio::name coopname;  ///< идентификатор кооператива
  eosio::name
      contract;  ///< внешний контракт, которому передано управление фондом

  std::string name;         ///< название фонда
  std::string description;  ///< описание фонда

  eosio::asset expended;  ///< количество средств, списанных по фонду

  uint64_t primary_key() const {
    return id;
  }  ///< Первичный ключ для индексации по идентификатору фонда
};

typedef eosio::multi_index<"expfunds"_n, expfund> expfunds_index;

struct [[eosio::table,
         eosio::contract(
             FUND)]] fwithdraw {  // таблица ожидания решения совета о
                                  // использовании фонда накопления
  uint64_t id;
  eosio::name coopname;
  eosio::name username;
  eosio::name status;
  eosio::name type;
  uint64_t fund_id;
  eosio::asset quantity;
  document document;
  std::string bank_data_id;
  eosio::time_point_sec expired_at;

  uint64_t primary_key() const {
    return id;
  }  ///< Первичный ключ для индексации по идентификатору фонда
  uint64_t by_username() const { return username.value; }  ///<
  uint64_t by_status() const { return status.value; }      ///<

  uint64_t by_expired() const {
    return expired_at.sec_since_epoch();
  } /*!< Индекс по статусу истечения */
};

typedef eosio::multi_index<
    "fwithdraws"_n, fwithdraw,
    eosio::indexed_by<
        "byusername"_n,
        eosio::const_mem_fun<fwithdraw, uint64_t, &fwithdraw::by_username>>,
    eosio::indexed_by<
        "bystatus"_n,
        eosio::const_mem_fun<fwithdraw, uint64_t, &fwithdraw::by_status>>,
    eosio::indexed_by<
        "byexpired"_n,
        eosio::const_mem_fun<fwithdraw, uint64_t, &fwithdraw::by_expired>>>
    fundwithdraws_index;
