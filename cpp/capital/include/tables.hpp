#pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

struct bonus_result {
    int64_t creators_bonus;
    int64_t authors_bonus;
    int64_t generated;
    int64_t participants_bonus;
    int64_t total;
};

/**
  * @brief Структура участника, хранящая данные индивидуального участника.
  * \ingroup public_tables
  */
struct [[eosio::table, eosio::contract(CAPITAL)]] contributor {
    uint64_t id;                                ///< Уникальный ID участника.
    name account;                               ///< Имя аккаунта участника.
    asset share_balance = asset(0, _root_govern_symbol); ///< Баланс долей участника.
    asset pending_rewards = asset(0, _root_symbol); ///< Накопленные вознаграждения.
    asset intellectual_contributions = asset(0, _root_govern_symbol); ///< Сумма интеллектуальных вкладов.
    asset property_contributions = asset(0, _root_govern_symbol); ///< Сумма имущественных вкладов.
    asset total_contributions = asset(0, _root_govern_symbol); ///< Общая сумма вкладов (интеллектуальных + имущественных).
    int64_t reward_per_share_last = 0;          ///< Последнее зафиксированное значение cumulative_reward_per_share (масштабировано).
    asset withdrawed = asset(0, _root_symbol);  ///< Общая сумма, выведенная через withdraw1.
    asset queued_withdrawal = asset(0, _root_symbol); ///< Сумма, запрошенная к выводу через withdraw2.

    uint64_t primary_key() const { return id; }             ///< Основной ключ.
    uint64_t by_account() const { return account.value; }   ///< Вторичный индекс по аккаунту.
};

typedef eosio::multi_index<"contributors"_n, contributor,
    indexed_by<"byaccount"_n, const_mem_fun<contributor, uint64_t, &contributor::by_account>>
> contributors_table; ///< Таблица для хранения участников.


/**
  * @brief Таблица идей
  * 
  */
struct [[eosio::table, eosio::contract(CAPITAL)]] idea {
    uint64_t id;
    checksum256 hash;
    name coopname;
    name administrator;
    eosio::name status = "created"_n; ///< created
    
    std::string title;
    std::string description;
    uint64_t authors_count;
    uint64_t authors_shares;
    
    eosio::asset target = asset(0, _root_govern_symbol);
    eosio::asset contributed = asset(0, _root_govern_symbol);
    eosio::asset withdrawed = asset(0, _root_govern_symbol);
    eosio::asset available = asset(0, _root_govern_symbol);
    
    time_point_sec created_at = current_time_point();
    
    uint64_t primary_key() const { return id; }
    uint64_t by_created_at() const { return created_at.sec_since_epoch(); }
    checksum256 by_hash() const { return hash; }
};

typedef eosio::multi_index<"ideas"_n, idea,
  indexed_by<"bycreatedat"_n, const_mem_fun<idea, uint64_t, &idea::by_created_at>>,
  indexed_by<"byhash"_n, const_mem_fun<idea, checksum256, &idea::by_hash>>
> idea_index;


struct [[eosio::table, eosio::contract(CAPITAL)]] result {
    uint64_t id;
    checksum256 hash;
    checksum256 idea_hash;
    eosio::name status = "created"_n; ///< created | started | expired?
    // checksum256 circle_hash; 

    eosio::name coopname;
    time_point_sec created_at = current_time_point();
    time_point_sec expired_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch() + 365 * 86400);
    uint64_t authors_shares;
    uint64_t authors_count;
    
    uint64_t creators_count;
    eosio::asset creators_amount = asset(0, _root_govern_symbol); ///< себестоимость РИД
    eosio::asset creators_bonus = asset(0, _root_govern_symbol); ///< премии создателей - 0.382 от себестоимости (creators_amount)
    eosio::asset authors_bonus = asset(0, _root_govern_symbol);  ///< премии авторов - 1.618 от себестоимости (creators_amount)
    eosio::asset generated_amount = asset(0, _root_govern_symbol); ///< стоимость РИД с учётом премий авторов и создателей
    eosio::asset participants_bonus = asset(0, _root_govern_symbol); ///< премии пайщиков кооператива - 1.618 от generated_amount
    
    eosio::asset total_amount = asset(0, _root_govern_symbol);; ///< Капитализация РИД  (стоимость РИД в generated_amount + participants_bonus)

    eosio::asset participants_bonus_remain = asset(0, _root_govern_symbol); ///< сумма остатка для выплаты пайщикам

    uint64_t primary_key() const { return id; }     ///< Основной ключ.
    checksum256 by_hash() const { return hash; } ///< Индекс по хэшу результата.
    checksum256 by_idea_hash() const { return idea_hash; } ///< Индекс по хэшу идеи    
};

  typedef eosio::multi_index<"results"_n, result,
    indexed_by<"byhash"_n, const_mem_fun<result, checksum256, &result::by_hash>>,
    indexed_by<"byideahash"_n, const_mem_fun<result, checksum256, &result::by_idea_hash>>
  > result_index;



struct [[eosio::table, eosio::contract(CAPITAL)]] claim {
    uint64_t id;
    checksum256 hash;
    checksum256 idea_hash;
    checksum256 result_hash;
    
    eosio::name coopname;
    eosio::name owner;

    eosio::name type; ///< intellectual | property
    eosio::name status = "created"_n; ///< created | statement | decision | act1 | act2 | completed
    time_point_sec created_at = current_time_point();

    eosio::asset amount = asset(0, _root_govern_symbol);

    document statement; ///< Заявление
    document decision; ///< Решение
    document act1; ///< Акт передачи
    document act2; ///< Акт приёма

    uint64_t primary_key() const { return id; }     ///< Основной ключ.
    uint64_t by_owner() const { return owner.value; } ///< Индекс по владельцу
    checksum256 by_hash() const { return hash; } ///< Индекс по хэшу
    checksum256 by_result_hash() const { return result_hash; } ///< Индекс по хэшу
    checksum256 by_idea_hash() const { return idea_hash; } ///< Индекс по хэшу идеи    
    
    uint128_t by_result_user() const {
        return combine_checksum_ids(result_hash, owner);
    }
};

  typedef eosio::multi_index<"claims"_n, claim,
    indexed_by<"byowner"_n, const_mem_fun<claim, uint64_t, &claim::by_owner>>,
    indexed_by<"byhash"_n, const_mem_fun<claim, checksum256, &claim::by_hash>>,
    indexed_by<"byresulthash"_n, const_mem_fun<claim, checksum256, &claim::by_result_hash>>,
    indexed_by<"byideahash"_n, const_mem_fun<claim, checksum256, &claim::by_idea_hash>>,
    indexed_by<"byresuser"_n, const_mem_fun<claim, uint128_t, &claim::by_result_user>>
  > claim_index;


struct [[eosio::table, eosio::contract(CAPITAL)]] author {
    uint64_t id;
    checksum256 idea_hash;
    eosio::name username;
    uint64_t shares;
    
    uint64_t primary_key() const { return id; } ///< Основной ключ
    uint64_t by_username() const { return username.value; } ///< Индекс по имени пользователя
    checksum256 by_idea_hash() const { return idea_hash; } ///< Индекс по хэшу идеи
    
    uint128_t by_idea_author() const {
        return combine_checksum_ids(idea_hash, username);
    }
};

  typedef eosio::multi_index<"authors"_n, author,
    indexed_by<"byusername"_n, const_mem_fun<author, uint64_t, &author::by_username>>,
    indexed_by<"byideahash"_n, const_mem_fun<author, checksum256, &author::by_idea_hash>>,
    indexed_by<"byideaauthor"_n, const_mem_fun<author, uint128_t, &author::by_idea_author>>
  > authors_index;



struct [[eosio::table, eosio::contract(CAPITAL)]] result_author {
    uint64_t id;
    checksum256 idea_hash;
    checksum256 result_hash;
    
    eosio::name username;
    
    uint64_t shares;
    
    uint64_t primary_key() const { return id; } ///< Основной ключ
    uint64_t by_username() const { return username.value; } ///< Индекс по имени пользователя
    checksum256 by_idea_hash() const { return idea_hash; } ///< Индекс по хэшу идеи
    
    uint128_t by_result_author() const {
        return combine_checksum_ids(idea_hash, username);
    }
};

  typedef eosio::multi_index<"resauthors"_n, result_author,
    indexed_by<"byusername"_n, const_mem_fun<result_author, uint64_t, &result_author::by_username>>,
    indexed_by<"byideahash"_n, const_mem_fun<result_author, checksum256, &result_author::by_idea_hash>>,
    indexed_by<"byresauthor"_n, const_mem_fun<result_author, uint128_t, &result_author::by_result_author>>
  > result_authors_index;




struct [[eosio::table, eosio::contract(CAPITAL)]] creator {
  uint64_t id; ///< id и primary_key
  
  checksum256 idea_hash; ///< Хэш идеи
  checksum256 result_hash; ///< Хэш результата интеллектуальной деятельности
  
  eosio::name username; ///< Имя пользователя
  eosio::asset spended = asset(0, _root_govern_symbol); ///< Стоимость использованных ресурсов

  uint64_t primary_key() const { return id; }
  checksum256 by_result_hash() const { return result_hash; }
  checksum256 by_idea_hash() const { return idea_hash; }
  
  uint128_t by_result_creator() const {
        return combine_checksum_ids(result_hash, username);
    }
    
  uint64_t by_username() const { return username.value; }
};

  typedef eosio::multi_index<"creators"_n, creator,
    indexed_by<"byresulthash"_n, const_mem_fun<creator, checksum256, &creator::by_result_hash>>,
    indexed_by<"byideahash"_n, const_mem_fun<creator, checksum256, &creator::by_idea_hash>>,
    indexed_by<"byusername"_n, const_mem_fun<creator, uint64_t, &creator::by_username>>,
    indexed_by<"byresultcrtr"_n, const_mem_fun<creator, uint128_t, &creator::by_result_creator>>
  > creators_index;

/**
  * @brief Структура глобального состояния, хранящая общие данные контракта.
  * \ingroup public_tables
  */
struct [[eosio::table, eosio::contract(CAPITAL)]] global_state {
    eosio::name coopname;                                ///< Имя кооператива глобального состояния.
    uint64_t program_id;                                  ///<  Идентификатор целевой программы.
    asset total_shares = asset(0, _root_govern_symbol);    ///< Общая сумма долей всех участников.
    asset total_contributions = asset(0, _root_govern_symbol); ///< Общая сумма всех вкладов.
    asset total_rewards_distributed = asset(0, _root_symbol); ///< Общая сумма распределенных вознаграждений.
    asset total_withdrawed = asset(0, _root_symbol); ///< Общая сумма, выведенная через withdraw1.
    asset total_intellectual_contributions = asset(0, _root_govern_symbol); ///< Общая сумма интеллектуальных вкладов.
    asset total_property_contributions = asset(0, _root_govern_symbol); ///< Общая сумма имущественных вкладов.
    asset accumulated_amount = asset(0, _root_symbol); ///< Накопленные членские взносы.
    int64_t cumulative_reward_per_share = 0;        ///< Накопленное вознаграждение на долю (масштабировано).

    uint64_t primary_key() const { return coopname.value; }     ///< Основной ключ.
};

  typedef eosio::multi_index<"state"_n, global_state> global_state_table; ///< Таблица для хранения глобального состояния.


/**
  * @brief Структура запроса на вывод для обработки очереди на вывод.
  * \ingroup public_tables
  */
struct [[eosio::table, eosio::contract(CAPITAL)]] withdrawal_claim {
    uint64_t id;                                ///< Уникальный ID запроса на вывод.
    name account;                               ///< Имя аккаунта участника, запрашивающего вывод.
    asset amount = asset(0, _root_symbol);      ///< Запрошенная сумма для вывода.
    uint64_t timestamp;                         ///< Время создания запроса.

    uint64_t primary_key() const { return id; }             ///< Основной ключ.
    uint64_t by_account() const { return account.value; }   ///< Вторичный индекс по аккаунту.
    uint64_t by_timestamp() const { return timestamp; }     ///< Вторичный индекс по времени.
};

  typedef eosio::multi_index<"withdrawals"_n, withdrawal_claim,
    indexed_by<"byaccount"_n, const_mem_fun<withdrawal_claim, uint64_t, &withdrawal_claim::by_account>>,
    indexed_by<"bytimestamp"_n, const_mem_fun<withdrawal_claim, uint64_t, &withdrawal_claim::by_timestamp>>
  > withdrawals_table; ///< Таблица для хранения запросов на вывод.
