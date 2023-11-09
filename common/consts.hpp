using namespace eosio;
#define ANO "ano"
#define DRAFT "draft"
#define MARKETPLACE "marketplace"
#define SOVIET "soviet"
#define REGISTRATOR "registrator"
#define SYSTEM "eosio"
#define PARTNERS "part"
#define GATEWAY "gateway"

// /**
//  * @mainpage Обзор контрактов
//  * - _registrator - контракт регистратора аккаунтов, физических и юридических лиц
//  * - _marketplace - контракт обмена всё на всё
//  * - _soviet - контракт советов кооперативов
//  * - _system - системный контракт делегатов при АНО
//  */

/**
\defgroup public_contracts Контракты
\brief Контракты Кооперативной Экономики
*
* Группа содержит контракты, предназначенные для кооперативной экономики. Контракты описывают основные логические 
* единицы и функциональность для управления и взаимодействия в рамках кооперативной экосистемы. От регистрации 
* участников до обменных операций - все аспекты кооперативной деятельности регулируются этими контрактами.
*/

/**
\defgroup public_consts Константы
\brief Константы контрактов
*
* В этой группе представлены основные константы, используемые в контрактах кооперативной экономики. 
* Константы обеспечивают стабильность и предсказуемость поведения контрактов, а также упрощают 
* процесс чтения и понимания кода.
*/

/**
\defgroup public_actions Действия
\brief Методы действий контрактов
*
* Действия представляют собой методы, которые можно вызывать в контрактах. Они отвечают за 
* выполнение определенных задач, таких как создание аккаунта, обновление информации или 
* осуществление транзакции. Эта группа описывает все доступные действия, которые предоставляют 
* контракты кооперативной экономики.
*/

/**
\defgroup public_tables Таблицы
\brief Структуры таблиц контрактов
*
* Таблицы в контрактах EOSIO используются для хранения состояния контракта на блокчейне. В этой группе 
* представлены основные структуры данных, которые используются в таблицах контрактов кооперативной экономики. 
* От сведений о участниках до деталей транзакций - все это сохраняется в таблицах.
*/

/**
\defgroup public_structures Структуры
\brief Структуры контрактов
*
* Структуры - это фундаментальные блоки данных, используемые для описания и хранения информации в контрактах. 
* Эта группа содержит описание основных структур, используемых в контрактах кооперативной экономики. 
* Они обеспечивают четкую организацию данных и упрощают взаимодействие с контрактом.
*/


/**
* @ingroup public_consts
* @{ 
*/

static constexpr uint64_t _auction_name_length_limit = 5;
static constexpr eosio::name _ano = "ano"_n;
static constexpr eosio::name _gateway = "gateway"_n;
static constexpr eosio::name _draft = "draft"_n;
static constexpr eosio::name _marketplace = "marketplace"_n; /*!< имя контракта */
static constexpr eosio::name _soviet = "soviet"_n; /*!< дополнительная константа */
static constexpr eosio::name _registrator = "registrator"_n;
static constexpr eosio::name _registrator_test = "reg.test"_n;
static constexpr eosio::name _system = "eosio"_n;
static constexpr eosio::name _partners = "part"_n; /*!< имя аккаунта контракта хранилища партнёров */
static constexpr eosio::name _regaccount_action = "joincoop"_n;
static constexpr eosio::name _change_action = "change"_n;
static constexpr eosio::symbol _root_symbol = eosio::symbol(eosio::symbol_code("AXON"), 4); /*!< системный токен */
static constexpr eosio::name _root_contract = "eosio.token"_n; /*!< системный контракт */
static constexpr eosio::symbol RAM_symbol{"RAM", 0}; /*!< токен рынка оперативной памяти */
static constexpr eosio::symbol _ramcore_symbol = eosio::symbol(eosio::symbol_code("RAMCORE"),4); /*!< идентификационный токен рынка оперативной памяти */
static constexpr uint64_t _ram_bytes = 16384; /*!< оперативная память нового аккаунта */
static constexpr uint64_t _stake_cpu_amount = 10000; /*!< оперативная память нового аккаунта */
static constexpr uint64_t _stake_net_amount = 10000; /*!< оперативная память нового аккаунта */

static constexpr uint64_t _signature_expiration = 86400 * 365 * 2;
/**
* @}
*/
