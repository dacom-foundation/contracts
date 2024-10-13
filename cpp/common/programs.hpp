#pragma once

struct [[eosio::table, eosio::contract(SOVIET)]] program {
  uint64_t id;                 /*!< идентификатор обмена */
  uint64_t draft_id;         ///< Ссылка на шаблон условий
  eosio::name program_type;  /*!< тип кооперативной программы (wallet | market | ...) */
  
  eosio::name coopname;      /*!< имя аккаунта кооператива */
  bool is_active;
  std::string title;
  std::string announce;
  std::string description;
  std::string preview;
  std::string images;
  std::string meta;

  eosio::name calculation_type; /*!< тип настройки платежей по программе ( absolute | relative | free ) */
  uint64_t membership_percent_fee; /*!< процент комиссии со взноса */
  eosio::asset fixed_membership_contribution;  /*!< Членский взнос */
  
  eosio::time_point_sec start_at;  /*!< Время открытия */
  eosio::time_point_sec expired_at;  /*!< Временное ограничение */

  uint64_t primary_key() const { return id; } /*!< return id - primary_key */
  uint64_t by_program_type() const { return program_type.value;} /*!< return program_type - secondary_key */
  uint64_t by_draft() const {return draft_id;};
};

typedef eosio::multi_index<"programs"_n, program,
  eosio::indexed_by<"programtype"_n, eosio::const_mem_fun<program, uint64_t, &program::by_program_type>>,
  eosio::indexed_by<"bydraft"_n, eosio::const_mem_fun<program, uint64_t, &program::by_draft>>
> programs_index; /*!< Тип мультииндекса для таблицы целевых программ */



program get_program_or_fail(eosio::name coopname, uint64_t program_id) {
  programs_index programs(_soviet, coopname.value);
  auto program = programs.find(program_id);
  eosio::check(program != programs.end(), "Программа не найдена");

  return *program;
};


bool is_participant_of_cpp_by_program_id(eosio::name coopname, eosio::name username, uint64_t program_id) {
  progwallets_index progwallets(_soviet, coopname.value);
  
  auto wallets_by_username_and_program = progwallets.template get_index<"byuserprog"_n>();
  auto username_and_program_index = combine_ids(username.value, program_id);
  auto wallet = wallets_by_username_and_program.find(username_and_program_index);
  
  if (wallet == wallets_by_username_and_program.end())
    return false;
  
  auto program = get_program_or_fail(coopname, program_id);

  //достать agreement и проверить статус 
  agreements_index agreements(_soviet, coopname.value);
  auto agreement = agreements.find(wallet -> agreement_id);
  
  if (agreement -> status == "declined"_n)
    return false;
    
  return true;
}



bool is_valid_participant_of_program_by_type(eosio::name coopname, eosio::name username, eosio::name program_type) {
  programs_index programs(_soviet, coopname.value);
  progwallets_index progwallets(_soviet, coopname.value);
  
  coagreements_index coagreements(_soviet, coopname.value);
  auto coagreement = coagreements.find(program_type.value);
  
  if (coagreement == coagreements.end())
    return false;
  
  auto exist = programs.find(coagreement -> program_id);
  
  if (exist == programs.end())
    return false;
  
  auto wallets_by_username_and_program = progwallets.template get_index<"byuserprog"_n>();
  auto username_and_program_index = combine_ids(username.value, exist -> id);
  auto wallet = wallets_by_username_and_program.find(username_and_program_index);
  
  if (wallet == wallets_by_username_and_program.end())
    return false;
  
  //достать agreement и проверить статус 
  agreements_index agreements(_soviet, coopname.value);
  auto agreement = agreements.find(wallet -> agreement_id);
  
  if (agreement -> status == "declined"_n)
    return false;
  
  return true;
}
