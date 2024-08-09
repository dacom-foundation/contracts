using namespace eosio;

void add_vote_for(eosio::name coopname, eosio::name member, uint64_t decision_id, bool approved) {
  // Инициализация таблицы решений
  decisions_index decisions(_soviet, coopname.value);

  // Поиск решения по decision_id
  auto decision = decisions.find(decision_id);
  eosio::check(decision != decisions.end(), "Решение с данным ID не найдено.");

  // Модифицируем запись в таблице
  decisions.modify(decision, _soviet, [&](auto& row) {
    row.votes_for.push_back(member); // Добавляем участника в голоса за
    row.approved = approved;
  });
}


void add_vote_against(eosio::name coopname, eosio::name member, uint64_t decision_id) {
  // Инициализация таблицы решений
  decisions_index decisions(_soviet, coopname.value);

  auto decision = decisions.find(decision_id);
  // Модифицируем запись в таблице
  decisions.modify(decision, _soviet, [&](auto& row) {
    row.votes_against.push_back(member); // Добавляем участника в голоса против
  });
}


/**
\ingroup public_actions
\brief Голосование за решение совета
*
* Этот метод позволяет члену совета голосовать за конкретное решение. Если у члена совета нет права голоса или голосование уже было произведено ранее, процедура завершится ошибкой. После голосования рассчитывается, превысило ли количество голосов "за" заданный процент консенсуса от общего количества членов.
*
* @param coopname Имя кооператива
* @param member Имя члена совета, голосующего за решение
* @param decision_id Идентификатор решения, за которое происходит голосование
* 
* @note Авторизация требуется от аккаунта: @p member или @p permission_level{member, "oracle"_n}
*/
void soviet::votefor(eosio::name coopname, eosio::name member, uint64_t decision_id) { 
  if (!has_auth(member)) {
    require_auth(permission_level{member, "oracle"_n});
  } else {
    require_auth(member);
  }
  
  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  eosio::check(decision != decisions.end(), "Документ не найден");
  
  auto board = get_board_by_type_or_fail(coopname, "soviet"_n);
  eosio::check(board.is_voting_member(member), "У вас нет права голоса");
  
  decision -> check_for_any_vote_exist(member); 

  auto [votes_for_count, votes_against_count] = decision->get_votes_count();

  votes_for_count++;

  uint64_t total_members = board.get_members_count();
  uint64_t consensus_percent = 50;
  
  // Рассчитываем, больше ли количество голосов "за" заданного процента консенсуса от общего количества участников
  bool approved = votes_for_count * 100 > total_members * consensus_percent;

  add_vote_for(coopname, member, decision_id, approved);

};

/**
\ingroup public_actions
\brief Голосование против решения совета
*
* Этот метод позволяет члену совета голосовать против конкретного решения. Если у члена совета нет права голоса или голосование уже было произведено ранее, процедура завершится ошибкой.
*
* @param coopname Имя кооператива
* @param member Имя члена совета, голосующего против решения
* @param decision_id Идентификатор решения, против которого происходит голосование
* 
* @note Авторизация требуется от аккаунта: @p member или @p permission_level{member, "provide"_n}
*/
void soviet::voteagainst(eosio::name coopname, eosio::name member, uint64_t decision_id) { 
  
  if (!has_auth(member)) {
    require_auth(permission_level{member, "provide"_n});
  } else {
    require_auth(member);
  }
  
  decisions_index decisions(_soviet, coopname.value);
  auto decision = decisions.find(decision_id);
  eosio::check(decision != decisions.end(), "Документ не найден");

  auto board = get_board_by_type_or_fail(coopname, "soviet"_n);

  eosio::check(board.is_voting_member(member), "У вас нет права голоса");
  
  decision -> check_for_any_vote_exist(member); 

  add_vote_against(coopname, member, decision_id);
};

/**
\ingroup public_actions
\brief Отмена голосования членом совета
*
* Этот метод позволяет члену совета отменить свой голос, поданный ранее, по конкретному решению. Если голосование не было произведено или решение не найдено, процедура завершится ошибкой.
*
* @param coopname Имя кооператива
* @param member Имя члена совета, отменяющего свое голосование
* @param decision_id Идентификатор решения, по которому голосование было проведено
* 
* @note Авторизация требуется от аккаунта: @p member или @p permission_level{member, "provide"_n}
*/
void soviet::cancelvote(eosio::name coopname, eosio::name member, uint64_t decision_id) {
  
  if (!has_auth(member)) {
    require_auth(permission_level{member, "provide"_n});
  } else {
    require_auth(member);
  }

  eosio::check(false, "Отмена голоса запрещена");
  
  decisions_index decisions(_soviet, coopname.value); 
  auto decision = decisions.find(decision_id);
  eosio::check(decision != decisions.end(), "Документ не найден");
  
  // Удаление голоса "за", если он существует
  auto board = get_board_by_type_or_fail(coopname, "soviet"_n);

  auto vote_for_it = std::find(decision->votes_for.begin(), decision->votes_for.end(), member);
  if (vote_for_it != decision->votes_for.end()) {
    uint64_t total_members = board.get_members_count();
    uint64_t consensus_percent = 50;

    decisions.modify(decision, _soviet, [&](auto& row) {
      row.votes_for.erase(vote_for_it);
      uint64_t votes_for_count = row.votes_for.size();
      row.approved = (votes_for_count * 100 / total_members) > consensus_percent;
    });
  }

  // Удаление голоса "против", если он существует
  auto vote_against_it = std::find(decision->votes_against.begin(), decision->votes_against.end(), member);
  if (vote_against_it != decision->votes_against.end()) {
    decisions.modify(decision, _soviet, [&](auto& row) {
      row.votes_against.erase(vote_against_it);
    });
  }
}
