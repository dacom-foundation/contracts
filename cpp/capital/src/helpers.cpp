#include <optional>

void capital::add_capital_to_wallet(eosio::name coopname, eosio::name username, eosio::asset amount){
  
  action(
    permission_level{ _capital, "active"_n},
    _soviet,
    "addbalance"_n,
    std::make_tuple(coopname, username, amount)
  ).send();

  action(
    permission_level{ _capital, "active"_n},
    _soviet,
    "blockbal"_n,
    std::make_tuple(coopname, username, amount)
  ).send();

  auto program = get_capital_program_or_fail(coopname);
  
  action(
    permission_level{ _capital, "active"_n},
    _soviet,
    "addprogbal"_n,
    std::make_tuple(coopname, username, program.id, amount)
  ).send();

};



program capital::get_capital_program_or_fail(eosio::name coopname) {
  auto state = get_global_state(coopname);
  
  programs_index programs(_soviet, coopname.value);
  auto program = programs.find(state.program_id);
  
  eosio::check(program != programs.end(), "Программа капитализации не найдена");
  return *program;
}

std::optional<progwallet> capital::get_capital_wallet(eosio::name coopname, eosio::name username) {
  auto state = get_global_state(coopname);
  
  programs_index programs(_soviet, coopname.value);
  auto program = programs.find(state.program_id);
  eosio::check(program != programs.end(), "Программа капитализации не найдена");
  
  auto capital_wallet = get_program_wallet(coopname, username, state.program_id);
  
  if (!capital_wallet.has_value()) {
    return std::nullopt;
  }
  
  return *capital_wallet;
  
}


std::optional<result_author> capital::get_result_author(eosio::name coopname, eosio::name username, const checksum256 &result_hash) {
    result_authors_index result_authors(_capital, coopname.value);
    auto result_author_index = result_authors.get_index<"byresauthor"_n>();

    uint128_t combined_id = combine_checksum_ids(result_hash, username);
    auto result_author_itr = result_author_index.find(combined_id);

    if (result_author_itr == result_author_index.end()) {
        return std::nullopt;
    }

    return *result_author_itr;
}

std::optional<author> capital::get_author(eosio::name coopname, eosio::name username, const checksum256 &idea_hash) {
    authors_index authors(_capital, coopname.value);
    auto idea_author_index = authors.get_index<"byideaauthor"_n>();

    uint128_t combined_id = combine_checksum_ids(idea_hash, username);
    auto author_itr = idea_author_index.find(combined_id);

    if (author_itr == idea_author_index.end()) {
        return std::nullopt;
    }

    return *author_itr;
}

std::optional<creator> capital::get_creator(eosio::name coopname, eosio::name username, const checksum256 &result_hash) {
    creators_index creators(_capital, coopname.value);
    auto result_creator_index = creators.get_index<"byresultcrtr"_n>();

    uint128_t combined_id = combine_checksum_ids(result_hash, username);
    auto creator_itr = result_creator_index.find(combined_id);

    if (creator_itr == result_creator_index.end()) {
        return std::nullopt;
    }

    return *creator_itr;
}

std::optional<result> capital::get_result(eosio::name coopname, const checksum256 &result_hash) {
    result_index results(_capital, coopname.value);
    auto result_hash_index = results.get_index<"byhash"_n>();

    auto result_itr = result_hash_index.find(result_hash);
    if (result_itr == result_hash_index.end()) {
        return std::nullopt;
    }

    return *result_itr;
}

std::optional<idea> capital::get_idea(eosio::name coopname, const checksum256 &idea_hash) {
    idea_index ideas(_capital, coopname.value);
    auto idea_hash_index = ideas.get_index<"byhash"_n>();

    auto idea_itr = idea_hash_index.find(idea_hash);
    if (idea_itr == idea_hash_index.end()) {
        return std::nullopt;
    }

    return *idea_itr;
}

std::optional<claim> capital::get_claim(eosio::name coopname, eosio::name username, const checksum256 &result_hash) {
    claim_index claims(_capital, coopname.value);
    auto claim_hash_index = claims.get_index<"byresuser"_n>();

    uint128_t combined_id = combine_checksum_ids(result_hash, username);
    auto claim_itr = claim_hash_index.find(combined_id);

    if (claim_itr == claim_hash_index.end()) {
        return std::nullopt;
    }

    return *claim_itr;
}



/**
  * @brief Обновляет глобальное состояние новыми значениями.
  *
  * @param gs Новое глобальное состояние.
  */
void capital::update_global_state(const global_state& gs){
  global_state_table global_state_inst(_self, _self.value);
  auto itr = global_state_inst.find(0);
  check(itr != global_state_inst.end(), "Global state not found");
  global_state_inst.modify(itr, _self, [&](auto& s) {
      s = gs;
  });
}
    
/**
  * @brief Получает текущее глобальное состояние.
  *
  * @return Текущее глобальное состояние.
  */
global_state capital::get_global_state(name coopname) {
    global_state_table global_state_inst(_self, _self.value);
    auto itr = global_state_inst.find(coopname.value);
    eosio::check(itr != global_state_inst.end(), "Контракт не инициализирован");
    return *itr;
}


void capital::ensure_contributor(name coopname, name username) {
  // Получаем глобальное состояние для установки reward_per_share_last
  auto gs = get_global_state(coopname);

  // Таблица участников
  contributors_table contributors(_self, _self.value);
  auto idx = contributors.get_index<"byaccount"_n>();

  // Если участник с данным именем не найден, создаём новую запись
  if (idx.find(username.value) == idx.end()) {
      contributors.emplace(_self, [&](auto& p) {
          p.id = contributors.available_primary_key();
          p.account = username;
          p.share_balance = asset(0, TOKEN_SYMBOL);
          p.pending_rewards = asset(0, TOKEN_SYMBOL);
          p.intellectual_contributions = asset(0, TOKEN_SYMBOL);
          p.property_contributions = asset(0, TOKEN_SYMBOL);
          p.total_contributions = asset(0, TOKEN_SYMBOL);
          p.reward_per_share_last = gs.cumulative_reward_per_share;
          p.withdrawed = asset(0, TOKEN_SYMBOL);
          p.queued_withdrawal = asset(0, TOKEN_SYMBOL);
      });
  }
}

//----------------------------------------------------------------------------
// calculcate_capital_amounts: для расчёта премий по формулам:
//   - creators_bonus      = spended * 0.382
//   - authors_bonus       = spended * 1.618
//   - generated           = spended + creators_bonus + authors_bonus
//   - participants_bonus  = generated * 1.618
//   - total               = generated + participants_bonus
//----------------------------------------------------------------------------
bonus_result capital::calculcate_capital_amounts(int64_t spended_amount) {
    bonus_result br{};

    // Преобразуем spended_amount в double для дальнейших расчетов
    double spended = double(spended_amount);

    // 1) creators_bonus = spended_amount * 0.382
    br.creators_bonus = int64_t(spended * 0.382);

    // 2) authors_bonus = spended_amount * 1.618
    br.authors_bonus = int64_t(spended * 1.618);

    // 3) generated = spended + creators_bonus + authors_bonus
    br.generated = int64_t(spended + br.creators_bonus + br.authors_bonus);

    // 4) participants_bonus = generated * 1.618
    br.participants_bonus = int64_t(double(br.generated) * 1.618);

    // 5) total = generated + participants_bonus
    br.total = int64_t(br.generated + br.participants_bonus);

    return br;
}
