// capital.hpp

#pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include "../common/common.hpp"
#include "include/tables.hpp"

using namespace eosio;
using std::string;

/**
 *  \ingroup public_contracts
 *  @brief  Контракт Capital управляет вкладами, распределением вознаграждений и выводом средств для участников.
 *  Capital — это контракт, управляющий вкладами участников, распределением вознаграждений и механизмами вывода средств в системе. Он поддерживает два типа вкладов — интеллектуальные и имущественные, и предоставляет два метода для участников, чтобы вывести свои средства при различных условиях.
 */
class [[eosio::contract]] capital : public contract {
public:
    using contract::contract;
    
    /**
     * @brief Инициализирует контракт, устанавливая глобальное состояние.
     * \ingroup public_actions
     * Это действие должно быть вызвано один раз после развертывания контракта.
     */
    [[eosio::action]]
    void init(name coopname, name initiator, uint64_t program_id);

    /**
     * @brief Создать результат интеллектуальной деятельности
     * \ingroup public_actions
     */
    [[eosio::action]]
    void createresult(name coopname, name administrator, checksum256 idea_hash, checksum256 result_hash);
    

    /**
     * @brief Позволяет участнику совершить вклад имуществом.
     * \ingroup public_actions
     */
    [[eosio::action]]
    void contribute(name coopname, name administrator, name owner, checksum256 idea_hash, checksum256 result_hash, asset amount);

    /**
     * @brief Обновляет состояние участника, применяя накопленные вознаграждения.
     * \ingroup public_actions
     * @param username Имя аккаунта участника.
     */
    [[eosio::action]]
    void refresh(name coopname, name username);

    /**
     * @brief Выводит средства из интеллектуальных вкладов участника.
     *
     * Только участники, совершившие интеллектуальные вклады, могут использовать этот метод.
     * \ingroup public_actions
     * @param username Имя аккаунта участника.
     * @param amount Сумма для вывода.
     */
    [[eosio::action]]
    void withdraw1(name coopname, name username, asset amount); // Вывод для интеллектуальных вкладов

    /**
     * @brief Ставит запрос на вывод средств в очередь для обработки из накопленных членских взносов.
     *
     * Средства, запрошенные к выводу, перестают приносить вознаграждения и обрабатываются по порядку.
     * \ingroup public_actions
     * @param username Имя аккаунта участника.
     * @param amount Сумма для вывода.
     */
    [[eosio::action]]
    void withdraw2(name coopname, name username, asset amount); // Вывод через очередь из членских взносов

    /**
     * @brief Добавляет входящие членские взносы в глобальное состояние.
     *
     * Это действие вызывается системой для добавления средств.
     * \ingroup public_actions
     * @param amount Сумма добавленных членских взносов.
     */
    [[eosio::action]]
    void accumulate(name coopname, asset amount); // Добавление входящих членских взносов

    /**
     * @brief Создает новую идею в кооперативе.
     *
     * Данное действие регистрирует новую идею с использованием уникального хеша в качестве идентификатора.
     * Перед созданием производится проверка уникальности хеша, а также проверяются все авторы на членство в кооперативе.
     *
     * @param hash           Уникальный хеш идеи.
     * @param coopname       Имя кооператива, в котором создается идея.
     * @param administrator        Аккаунт, инициирующий создание идеи.
     * @param authors        Список аккаунтов-авторов идеи, которые должны быть участниками кооператива.
     * @param title          Заголовок идеи.
     * @param description    Подробное описание идеи.
     */
    [[eosio::action]]
    void createidea (
      checksum256 hash,
      eosio::name coopname, 
      eosio::name administrator,
      std::string title, 
      std::string description
    );
    
    [[eosio::action]]
    void createclaim(name coopname, name administrator, name owner, checksum256 result_hash);
    
    [[eosio::action]]
    void setstatement(name coopname, name administrator, name owner, checksum256 result_hash, document statement);
    
    [[eosio::action]]
    void authorize(eosio::name coopname, uint64_t claim_id, document decision);
    
    [[eosio::action]]
    void setact1(eosio::name coopname, eosio::name administrator, eosio::name owner, checksum256 result_hash, checksum256 claim_hash, document act);
    
    [[eosio::action]]
    void setact2(eosio::name coopname, eosio::name administrator, eosio::name owner, checksum256 result_hash, document act);
    
    
    [[eosio::action]]
    void start(name coopname, name administrator, checksum256 result_hash);
    
    [[eosio::action]]
    void addauthor(name coopname, name administrator, checksum256 idea_hash, name author, uint64_t shares);
    
    [[eosio::action]]
    void addcreator(name coopname, name administrator, checksum256 result_hash, name creator, eosio::asset spended);
    
    [[eosio::action]]
    void delcreator(name coopname, name administrator, checksum256 result_hash, name creator);

private:
    
  static constexpr symbol TOKEN_SYMBOL = _root_govern_symbol; ///< Символ используемого токена.
  static constexpr symbol ACCUMULATION_SYMBOL = _root_symbol; ///< Символ токена для учёта распределения.
  
  static constexpr int64_t MAX_AUTHORS = 12;
  
  static constexpr name _intellectual = "intellectual"_n; ///< Символьное обозначение интеллектуального взноса
  static constexpr name _property = "property"_n; ///< Символьное обозначение имущественного взноса
  

  /**
    * @brief Обновляет глобальное состояние новыми значениями.
    *
    * @param gs Новое глобальное состояние.
    */
  void update_global_state(const global_state& gs);
      
  /**
    * @brief Получает текущее глобальное состояние.
    *
    * @return Текущее глобальное состояние.
    */
  global_state get_global_state(name coopname);
    
  void ensure_contributor(name coopname, name username);
  
  static bonus_result calculcate_capital_amounts(int64_t spended_amount);
  
  std::optional<author> get_author(eosio::name coopname, eosio::name username, const checksum256 &idea_hash);
  std::optional<creator> get_creator(eosio::name coopname, eosio::name username, const checksum256 &result_hash);
  std::optional<result> get_result(eosio::name coopname, const checksum256 &result_hash);
  std::optional<idea> get_idea(eosio::name coopname, const checksum256 &idea_hash);
  std::optional<claim> get_claim(eosio::name coopname, eosio::name username, const checksum256 &result_hash);
  std::optional<result_author> get_result_author(eosio::name coopname, eosio::name username, const checksum256 &result_hash);
  
  program get_capital_program_or_fail(eosio::name coopname);
  std::optional<progwallet> get_capital_wallet(eosio::name coopname, eosio::name username);
  void add_capital_to_wallet(eosio::name coopname, eosio::name username, eosio::asset amount);

};
