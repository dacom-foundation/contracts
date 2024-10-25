using namespace eosio;
#include <eosio/eosio.hpp>
#include <eosio/crypto.hpp>

/**
\ingroup public_actions
\brief Авторизация принятого решения советом
*
* Этот метод позволяет председателю совета утвердить принятое решение совета. 
*
* @param coopname Имя кооператива
* @param chairman Имя председателя совета кооператива
* @param decision_id Идентификатор решения для авторизации
* 
* @note Авторизация требуется от аккаунта: @p chairman
*/
void soviet::authorize(eosio::name coopname, eosio::name chairman, uint64_t decision_id, document document) { 
  require_auth(chairman);

  boards_index boards(_soviet, coopname.value);
  autosigner_index autosigner(_soviet, coopname.value);
  
  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  eosio::check(decision != decisions.end(), "Документ не найден");
  auto board = get_board_by_type_or_fail(coopname, "soviet"_n);
  eosio::check(board.is_valid_chairman(chairman), "Только председатель совета может утвердить решение");
  
  eosio::check(decision -> approved == true, "Консенсус совета по решению не достигнут");

  verify_document_or_fail(document);

  decisions.modify(decision, chairman, [&](auto &d){
    d.authorized_by = chairman;
    d.authorized = !decision -> authorized;
    d.authorization = document;
  });

  auto signer = autosigner.find(decision -> id);
  
  if (signer != autosigner.end())
    autosigner.erase(signer);

}

/**
\ingroup public_actions
\brief Создание нового совета кооператива
*
* Этот метод позволяет председателю кооператива создать новый совет с указанными членами и параметрами. Если совет создается как совет кооператива, председатель должен быть указан в списке членов совета.
*
* @param coopname Имя кооператива
* @param username Имя председателя кооператива
* @param type Тип совета. Доступные типы:
*   - **soviet** - Совет кооператива (Board of Members)
*   - **executive** - Правление (Executive Board)
*   - **audit** - Ревизионный комитет (Audit and Revision Board)
*   - **other** - Другая комиссия (Other committee)
* @param members Список членов совета
* @param name Название совета
* @param description Описание совета
* 
* @note Авторизация требуется от аккаунта: @p username
*/
void soviet::createboard(eosio::name coopname, eosio::name username, eosio::name type, std::vector<board_member> members, std::string name, std::string description){
  require_recipient(coopname);
  
  check_auth_or_fail(coopname, username, "createboard"_n);

  eosio::name payer = username;

  cooperatives_index coops(_registrator, _registrator.value);
  auto org = coops.find(coopname.value);
  eosio::check(org != coops.end(), "Организация не найдена");
  eosio::check(org -> is_coop(), "Организация - не кооператив");
    
  participants_index participants(_soviet, coopname.value);

  if (type == "soviet"_n) {
    bool is_exist = check_for_exist_board_by_type(coopname, "soviet"_n);
    eosio::check(is_exist == false, "Совет кооператива уже создан");
    
    bool has_chairman = false;
    std::set<eosio::name> usernames;
    eosio::name chairman;
    
    bool chairman_found = false;

    for (const auto& m : members) {
        auto participant = participants.find(m.username.value);
        eosio::check(participant != participants.end(), "Один из аккаунтов не найден в реестре пайщиков");
        eosio::check(participant -> type.value() == "individual"_n, "Только физическое лицо может быть членом Совета");

        eosio::check(usernames.insert(m.username).second, "Обнаружено повторение username");

        if (m.position == "chairman"_n) {
            eosio::check(!chairman_found, "Обнаружено более одного председателя");
            has_chairman = true;
            chairman = m.username;
            chairman_found = true;
        }
    }

    eosio::check(has_chairman, "Председатель кооператива должен быть указан в членах совета");

    //Добавляем председателя в пайщики кооператива автоматически
    participants_index participants(_soviet, coopname.value);
    auto cooperative = get_cooperative_or_fail(coopname);
    
    addresses_index addresses(_soviet, coopname.value);
    address_data data;

    addresses.emplace(payer, [&](auto &a) {
      a.id = 0;
      a.coopname = coopname;
      a.data = data;
    });
    
    //создаём обязательные программы и соглашения
    programs_index programs(_soviet, coopname.value);
    auto program_id = get_global_id_in_scope(_soviet, coopname, "programs"_n);
  
    //создаём программу кошелька
    programs.emplace(username, [&](auto &pr) {
      pr.id = program_id;
      pr.is_active = true;
      pr.draft_id = 1;
      pr.coopname = coopname;
      pr.title = "Цифровой Кошелёк";
      pr.announce = "";
      pr.description = "";
      pr.preview = "";
      pr.images = "";
      pr.calculation_type = "free"_n;
      pr.fixed_membership_contribution = asset(0, org -> minimum.symbol);  
      pr.membership_percent_fee = 0;
      pr.meta = "";
    });
    
    coagreements_index coagreements(_soviet, coopname.value);
  
    //создаём соглашение для программы кошелька    
    coagreements.emplace(_soviet, [&](auto &row){
      row.type = "wallet"_n;
      row.coopname = coopname;
      row.program_id = program_id;
      row.draft_id = 1;
    });
    
    //создаём соглашение для шаблона простой электронной подписи
    coagreements.emplace(_soviet, [&](auto &row){
      row.type = "signature"_n;
      row.coopname = coopname;
      row.program_id = 0;
      row.draft_id = 2;
    });

    //создаём соглашение для шаблона пользовательского соглашения
    coagreements.emplace(_soviet, [&](auto &row){
      row.type = "user"_n;
      row.coopname = coopname;
      row.program_id = 0;
      row.draft_id = 3;
    });
    
    //создаём соглашение для шаблона политики конфиденциальности
    coagreements.emplace(_soviet, [&](auto &row){
      row.type = "privacy"_n;
      row.coopname = coopname;
      row.program_id = 0;
      row.draft_id = 4;
    });
    

  } else {
    
    auto soviet = get_board_by_type_or_fail(coopname, "soviet"_n);
  }

  boards_index boards(_soviet, coopname.value);

  boards.emplace(payer, [&](auto &b) {
    b.id = boards.available_primary_key();
    b.type = type;
    b.members = members;
    b.name = name;
    b.description = description;
    b.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    b.last_update = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
  });

}




/**
\ingroup public_actions
\brief Обновление совета или комиссии
*
* Этот метод позволяет председателю кооператива обновить совет с указанными членами. 
*
* @param coopname Имя кооператива
* @param chairman Имя председателя кооператива
* @param type Тип совета. Доступные типы:
*   - **soviet** - Совет кооператива (Board of Members)
*   - **executive** - Правление (Executive Board)
*   - **audit** - Ревизионный комитет (Audit and Revision Board)
*   - **other** - Другая комиссия (Other committee)
* @param members Список членов совета
* @param name Название совета
* @param description Описание совета
* 
* @note Авторизация требуется от аккаунта: @p username
*/
void soviet::updateboard(eosio::name coopname, eosio::name username, uint64_t board_id, std::vector<board_member> members, std::string name, std::string description){
  require_recipient(coopname);
  
  check_auth_or_fail(coopname, username, "updateboard"_n);
  
  cooperatives_index coops(_registrator, _registrator.value);
  auto org = coops.find(coopname.value);
  eosio::check(org != coops.end(), "Организация не найдена");
  eosio::check(org -> is_coop(), "Организация - не кооператив");

  boards_index boards(_soviet, coopname.value);
  auto board = boards.find(board_id);
  eosio::check(board != boards.end(), "Доска не найдена");

  participants_index participants(_soviet, coopname.value);

  if (board -> type == "soviet"_n) {
    bool has_chairman = false;
    std::set<eosio::name> usernames;
    eosio::name chairman;
    
    bool chairman_found = false;

    for (const auto& m : members) {
        auto participant = participants.find(m.username.value);
        eosio::check(participant != participants.end(), "Один из аккаунтов не найден в реестре пайщиков");
        eosio::check(participant -> type.value() == "individual"_n, "Только физическое лицо может быть членом Совета");

        eosio::check(usernames.insert(m.username).second, "Обнаружено повторение username");

        if (m.position == "chairman"_n) {
            eosio::check(!chairman_found, "Обнаружено более одного председателя");
            has_chairman = true;
            chairman = m.username;
            chairman_found = true;
        }
    }

    
    
    eosio::check(has_chairman, "Председатель кооператива должен быть указан в членах совета");
  };

  
  boards.modify(board, username, [&](auto &b) {
    b.members = members;
    b.name = name;
    b.description = description;
    b.last_update = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
  });

}
