#pragma once


struct [[eosio::table, eosio::contract(SOVIET)]] programs {
  uint64_t id;                 /*!< идентификатор обмена */
  eosio::name coopname;      /*!< имя аккаунта кооператива */
  bool is_active;
  std::string title;
  std::string announce;
  std::string description;
  std::string preview;
  std::string images;
  eosio::name category;
  
  // ('management', _('Management')),  # Управление
  // ('legal', _('Legal services')),   # Юридические услуги
  // ('accounting', _('Accounting')),    # Бухгалтерия и учет
  // ('education', _('Education')),    # Обучение, курсы
  // ('goods', _('Goods and products')),   # Товары и продукты
  // ('farm', _('Farms and food')),    # Фермы и питание
  // ('production', _('Production')),    # Производство
  // ('construction', _('Construction')),  # Cтроительство
  // ('coopfunding', _('Coopfunding')),  # КоопФандинг
  // ('services', _('Services')),    # Услуги
  
  eosio::name calculation_type; /*! тип настройки платежей по программе ( absolute | relative ) */
  uint64_t membership_percent_fee;

  eosio::asset share_contribution;     /*!< Паевый взнос */
  eosio::asset membership_contribution;  /*!< Членский взнос */

  eosio::asset total; /*!< Всего ко взносу для участия в программе */
  eosio::asset minimum; /*!< Минимальный взнос для участия в программе */
  eosio::asset maximum; /*!< Максимальный взнос в программу (0 для отсутствия ограничения) */

  eosio::name period;     /*!< Периодичность *///  (percase, daily, weekly, monthly, quarterly, halfayear, annually, onetime)
  
  std::string meta;

  eosio::time_point_sec created_at; ///< Время создания программы.
  eosio::time_point_sec last_update; ///< Время последнего обновления информации о программе.

  //TODO
  eosio::asset total_collected;
  eosio::asset total_withdrawed;


  uint64_t primary_key() const { return id; } /*!< return id - primary_key */
  uint64_t by_category() const { return category.value; } /*!< индекс по категории */

};

typedef eosio::multi_index<
    "programs"_n, programs,
    eosio::indexed_by<"bycategory"_n, eosio::const_mem_fun<programs, uint64_t, &programs::by_category>>>
programs_index; /*!< Тип мультииндекса для таблицы целевых программ */




//scope @coopname
struct [[eosio::table, eosio::contract(SOVIET)]] cntrbutions {
  uint64_t id;
  eosio::name coopname;
  eosio::name type; ///< change | invest | share
  uint64_t program_id;
  uint64_t secondary_id;
  eosio::name username;
  eosio::asset initial;
  eosio::asset now;
  eosio::time_point_sec created_at; ///< Время создания взноса.
  eosio::time_point_sec last_update; ///< Время обновления взноса.

  uint64_t primary_key() const { return id; } /*!< return id - primary_key */
  uint64_t by_username() const { return username.value; } /*!< индекс по пользователю */
  
  uint128_t by_username_and_secondary() const {
    return combine_ids(username.value, secondary_id);
  } /*!< возвращает уникальный индекс, сформированный из значения username и secondary_id */
};


typedef eosio::multi_index<
    "cntrbutions"_n, cntrbutions,
    eosio::indexed_by<"byusername"_n, eosio::const_mem_fun<cntrbutions, uint64_t, &cntrbutions::by_username>>,
    eosio::indexed_by<"byusersecond"_n, eosio::const_mem_fun<cntrbutions, uint128_t, &cntrbutions::by_username_and_secondary>>>
cntrbutions_index; /*!< Тип мультииндекса для таблицы взносов в целевые программы */




programs get_program_or_fail(eosio::name coopname, uint64_t program_id) {
  programs_index programs(_soviet, coopname.value);
  auto program = programs.find(program_id);
  eosio::check(program != programs.end(), "Программа не найдена");

  return *program;
};

  
