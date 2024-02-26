#pragma once


struct [[eosio::table, eosio::contract(SOVIET)]] progcomarket {
  uint64_t id;                 /*!< идентификатор обмена */
  eosio::name coopname;      /*!< имя аккаунта кооператива */
  bool is_active;
  std::string title;
  std::string announce;
  std::string description;
  std::string preview;
  std::string images;
  std::string meta;

  eosio::name calculation_type; /*!< тип настройки платежей по программе ( absolute | relative ) */
  uint64_t membership_percent_fee; /*!< процент комиссии со взноса */
  eosio::asset fixed_membership_contribution;  /*!< Членский взнос */

  uint64_t primary_key() const { return id; } /*!< return id - primary_key */

};

typedef eosio::multi_index<"progcomarket"_n, progcomarket> progcomarket_index; /*!< Тип мультииндекса для таблицы целевых программ */



progcomarket get_program_or_fail(eosio::name coopname, uint64_t program_id) {
  progcomarket_index programs(_soviet, coopname.value);
  auto program = programs.find(program_id);
  eosio::check(program != programs.end(), "Программа не найдена");

  return *program;
};

  
