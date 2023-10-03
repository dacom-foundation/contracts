/**
 * @ingroup public_tables
 * @brief Структура, представляющая верификацию.
 * @details Тип процедуры верификации (procedure) может принимать следующие значения:
 * - 0: Online
 * - 1: Video Call
 * - 2: Qualified Signature
 * - 3: Gosuslugi
 * - 4: С помощью мобильного приложения (App)
 * - 5: Некоторые платежные системы проводят верификацию в Связном (Agent Store)
 * - 6: Верификация в банке (Bank)
 * - 7: Верификация в кооперативе (In Person)
 */
struct verification {
  eosio::name verificator; ///< Организация, которая произвела верификацию
  bool is_verified; ///< Флаг, указывающий, прошла ли верификация
  uint64_t procedure; ///< Тип процедуры верификации
    // (0, _('online')),
    // (1, _('video call')),
    // (2, _('qualified signature')),
    // (3, _('gosuslugi')),
    // (4, _('app')),   -  с помощью мобильного приложения
    // (5, _('agent store')),     - некоторые платежные системы 
    // проводят верификацию в Связном
    // (6, _('bank')),    - верификация в банке
    // (7, _('in person')), - верификация в кооперативе

  eosio::time_point_sec created_at; ///< Время создания записи
  eosio::time_point_sec last_update; ///< Время последнего обновления записи
  std::string notice; ///< Дополнительная информация
};


/**
 * @ingroup public_tables
 * @brief Структура, представляющая учетные записи аккаунтов.
 * @details Эта структура хранит информацию о пользователях аккаунта и их статусе, репутации и других параметрах.
 */
struct [[eosio::table, eosio::contract(REGISTRATOR)]] accounts {
  eosio::name username; ///< Имя аккаунта гостя. Имя пользователя в системе.
  eosio::name referer; ///< Имя аккаунта, который был реферером при регистрации.
  eosio::name registrator; ///< Имя аккаунта регистратора, который создал этот аккаунт.
  eosio::name type; ///< Тип аккаунта: user (пользователь) | org (организация).
  eosio::name status; ///< Статус аккаунта:
  std::string meta; ///< Дополнительная мета-информация о аккаунте.
  // "pending" - ожидание утверждения советом,
  // "active" - активный аккаунт;
  // "blocked" - заблокированный аккаунт;
  // "deleted" - удален пользователем;
  uint64_t reputation; ///< Репутация аккаунта, возможно, связанная с его деятельностью.
  eosio::asset registration_amount; ///< Количество токенов, которое требуется для регистрации.
  eosio::time_point_sec registered_at; ///< Время регистрации аккаунта.
  eosio::time_point_sec signature_expires_at; ///< Время истечения срока действия подписи аккаунта.
  
  /**
   * @brief Возвращает первичный ключ учетной записи аккаунта.
   * @return uint64_t - первичный ключ, равный значению имени аккаунта.
   */
  uint64_t primary_key() const {
    return username.value;
  } /*!< return username - primary_key */
  
  /**
   * @brief Возвращает ключ по рефереру.
   * @return uint64_t - ключ, равный значению имени реферера.
   */
  uint64_t by_referer() const { return referer.value; }

  /**
   * @brief Возвращает ключ по типу аккаунта.
   * @return uint64_t - ключ, равный значению типа аккаунта.
   */
  uint64_t by_type() const { return type.value; }

  /**
   * @brief Возвращает ключ по статусу аккаунта.
   * @return uint64_t - ключ, равный значению статуса аккаунта.
   */
  uint64_t by_status() const { return status.value; }

  /**
   * @brief Возвращает ключ по регистратору.
   * @return uint64_t - ключ, равный значению имени регистратора.
   */
  uint64_t by_registr() const { return registrator.value; }
};

typedef eosio::multi_index<
    "accounts"_n, accounts,
    eosio::indexed_by<
        "byreferer"_n,
        eosio::const_mem_fun<accounts, uint64_t, &accounts::by_referer>>,
    eosio::indexed_by<"bytype"_n, eosio::const_mem_fun<accounts, uint64_t,
                                                       &accounts::by_type>>,
    eosio::indexed_by<
        "bystatus"_n,
        eosio::const_mem_fun<accounts, uint64_t, &accounts::by_status>>,
    eosio::indexed_by<
        "byregistr"_n,
        eosio::const_mem_fun<accounts, uint64_t, &accounts::by_registr>>>
    accounts_index;


/**
 * @ingroup public_structs
 * @brief Структура, представляющая хранилища данных, в которых хранятся персональные данные и их идентификаторы.
 */
struct storage {
  eosio::name storage_username; ///< Имя аккаунта хранилища персональных данных
  std::string uid; ///< Идентификатор данных в хранилище
};


/**
 * @ingroup public_tables
 * @brief Структура, представляющая учетные записи пользователей.
 * @details Эта структура хранит информацию о пользователях, их профилях и верификации.
 */
struct [[eosio::table, eosio::contract(REGISTRATOR)]] users {
  eosio::name username; ///< Имя аккаунта пользователя.
  std::vector<storage> storages; ///< Хранилища персональных данных и идентификаторы данных в них.
  std::vector<verification> verifications; ///< Информация о верификации пользователя.

  /**
   * @brief Возвращает первичный ключ учетной записи пользователя.
   * @return uint64_t - первичный ключ, равный значению имени аккаунта пользователя.
   */
  uint64_t primary_key() const {
    return username.value;
  } /*!< return username - primary_key */
  
};

typedef eosio::multi_index<"users"_n, users>users_index;


/**
 * @ingroup public_tables
 * @brief Структура, представляющая информацию о банке.
 * @details Эта структура содержит информацию о номере расчётного счёта банка, времени создания, времени последнего обновления и статусе активности.
 */
struct bank {
  std::string account; ///< Номер расчётного счёта банка.
  eosio::time_point_sec created_at; ///< Время создания записи.
  eosio::time_point_sec last_update; ///< Время последнего обновления записи.
  bool is_active; ///< Флаг, указывающий, активен ли банк.
};



/**
\ingroup public_tables
\brief Структура данных нового юридического лица
*
* Данная структура содержит всю необходимую информацию для регистрации нового юридического лица в блокчейне.
*/
struct org_data {
    storage storage; ///< Хранилища персональных данных и идентификаторы данных в них.
    // TODO - то, что сохраняем в сторадже:    
    // std::string name; ///< Полное наименование
    // std::string short_name; ///< Краткое наименование
    // std::string address; ///< Юридический адрес
    // std::string ogrn; ///< ОГРН
    // std::string inn; ///< ИНН
    // std::string logo; ///< Логотип
    // std::string phone; ///< Номер телефона
    // std::string email; ///< Электронная почта
    // std::string registration; ///< Дата регистрации юрлица
    // std::string website; ///< Веб-сайт
    // std::vector<bank> accounts; ///< Банковские счета

    bool is_cooperative = false; ///< Является ли кооперативом
    std::optional<eosio::name> coop_type; ///< Тип кооператива (union, conscoop, prodcoop, agricoop, builderscoop, nonprofitorg)
    std::optional<eosio::name> token_contract; ///< Контракт токена
    std::optional<std::string> slug; ///< ЧПУ (Человеко-понятный URL)
    std::optional<std::string> announce; ///< Анонс
    std::optional<std::string> description; ///< Описание
    std::optional<eosio::asset> initial; ///< Вступительный взнос
    std::optional<eosio::asset> minimum; ///< Минимальный взнос
    std::optional<eosio::asset> membership; ///< Членский взнос
    std::optional<eosio::name> period; ///< Периодичность (per case, daily, weekly, monthly, quarterly, half a year, annually, onetime)
};


/**
 * @ingroup public_tables
 * @brief Структура, представляющая организации.
 * @details Эта структура содержит информацию о юридических лицах (организациях), их верификации и других параметрах.
 */
struct [[eosio::table, eosio::contract(REGISTRATOR)]] orgs {
  eosio::name username; ///< Имя аккаунта организации.
  eosio::name parent_username; ///< Имя родительской организации, если есть.
  std::vector<verification> verifications; ///< Информация о верификации организации.
  std::vector<storage> storages; ///< Хранилища персональных данных и идентификаторы данных в них.
  
  bool is_cooperative = false; ///< Флаг, указывающий, является ли организация кооперативом.
  std::optional<eosio::name> coop_type; ///< Тип некоммерческой организации (если это кооператив).
  std::optional<eosio::name> token_contract; ///< Контракт токена, связанного с организацией.
  std::optional<std::string> slug; ///< Уникальный идентификатор организации.
  std::optional<std::string> announce; ///< Анонс организации.
  std::optional<std::string> description; ///< Описание организации.
  std::optional<uint64_t> members_count; ///< Количество членов организации.
  std::optional<uint64_t> users_count; ///< Количество пользователей организации.
  std::optional<uint64_t> orgs_counts; ///< Количество подорганизаций.
  std::optional<eosio::asset> initial; ///< Вступительный взнос (если применимо).
  std::optional<eosio::asset> minimum; ///< Минимальный взнос (если применимо).
  std::optional<eosio::asset> membership; ///< Членский взнос (если применимо).
  std::optional<eosio::name> period; ///< Периодичность взносов.
  // Тип некоммерческой организации
  // (0, _('Union of Societies')),
  // (1, _('Consumer Cooperative')),
  // (2, _('Production Cooperative')),
  // (3, _('Agriculture Cooperative')),
  // (4, _('Builder\'s Societes')),
  // (5, _('Non-profit organization'))

  /**
   * @brief Возвращает первичный ключ учетной записи организации.
   * @return uint64_t - первичный ключ, равный значению имени аккаунта организации.
   */
  uint64_t primary_key() const {
    return username.value;
  }

  /**
   * @brief Возвращает ключ по родительской организации.
   * @return uint64_t - ключ, равный значению имени родительской организации.
   */
  uint64_t by_parent() const {
    return parent_username.value;
  }

  /**
   * @brief Возвращает ключ для индекса кооперативных подразделений организации.
   * @return uint128_t - составной ключ, включающий значения имени организации и родительской организации.
   */
  uint128_t by_coop_childs() const {
    return combine_ids(username.value, parent_username.value);
  }

  /**
   * @brief Возвращает индекс для определения, является ли организация кооперативом.
   * @return uint64_t - ключ, равный 1, если организация является кооперативом, иначе 0.
   */
  uint64_t is_coop_index() const {
    return is_cooperative == true ? 1 : 0;
  }

  /**
   * @brief Возвращает ключ для индекса по типу некоммерческой организации (если это кооператив).
   * @return uint64_t - ключ, равный значению типа некоммерческой организации.
   */
  uint64_t bycooptype() const {
    return coop_type.value_or(eosio::name("")).value;
  }

  /**
   * @brief Возвращает индекс для определения, является ли организация верифицированной.
   * @return uint64_t - ключ, равный 1, если организация верифицирована, иначе 0.
   */
  uint64_t is_verified_index() const {
    for (const auto& v : verifications) {
      if (v.is_verified) {
        return 1;
      }
    }
    return 0;
  }

  /**
   * @brief Проверяет, является ли организация кооперативом.
   * @return bool - true, если организация является кооперативом, иначе false.
   */
  bool is_coop() const {
    return is_cooperative;
  }

  /**
   * @brief Проверяет, верифицирована ли организация.
   * @return bool - true, если организация верифицирована, иначе false.
   */
  bool is_verified() const {
    for (const auto& v : verifications) {
      if (v.is_verified) {
        return true;
      }
    }
    return false;
  }
};

typedef eosio::multi_index<"orgs"_n, orgs,
eosio::indexed_by<"iscoop"_n, eosio::const_mem_fun<orgs, uint64_t,
                                                       &orgs::is_coop_index>>,
eosio::indexed_by<"byparent"_n, eosio::const_mem_fun<orgs, uint64_t,
                                                       &orgs::by_parent>>,
eosio::indexed_by<"bycoopchilds"_n, eosio::const_mem_fun<orgs, uint128_t, &orgs::by_coop_childs>>,
eosio::indexed_by<"bycooptype"_n, eosio::const_mem_fun<orgs, uint64_t, &orgs::bycooptype>>,
eosio::indexed_by<"byverif"_n, eosio::const_mem_fun<orgs, uint64_t,
                                                       &orgs::is_verified_index>>

> orgs_index;


