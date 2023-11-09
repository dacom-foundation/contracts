#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/contract.hpp>
#include <eosio/crypto.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>

static constexpr eosio::name _me = "bet"_n;
static constexpr eosio::name _root_contract = "eosio.token"_n;

static constexpr eosio::symbol _root_symbol =
    eosio::symbol(eosio::symbol_code("SYS"), 4); /*!< системный токен */

class [[eosio::contract]] bet : public eosio::contract {

public:
  bet(eosio::name receiver, eosio::name code,
              eosio::datastream<const char *> ds)
      : eosio::contract(receiver, code, ds) {}


  [[eosio::action]] void notify(uint64_t bet_id, eosio::name status, eosio::asset amount);
  [[eosio::action]] void newid(uint64_t id);

  [[eosio::action]] void makebet(eosio::name username, uint64_t game_id,
              std::string encrypted_data, std::string hash_for_check,
              eosio::asset amount, eosio::time_point timepoint);

  [[eosio::action]] void addhypo(uint64_t game_id, uint64_t hypo_id, std::string title);

  [[eosio::action]] void creategame(uint64_t id, std::string caption);

  [[eosio::action]] void pushfeed(uint64_t game_id, uint64_t win_hypo_id);
  [[eosio::action]] void settle(uint64_t bet_id, uint64_t hypo_id,
                               std::string decrypted_data);

  static void back(uint64_t game_id, eosio::name username, uint64_t bet_id, eosio::asset amount);
  static void win(uint64_t game_id, eosio::name username, uint64_t bet_id, eosio::asset amount);
  static void lose(uint64_t game_id, eosio::name username, uint64_t bet_id, eosio::asset amount);

  void apply(uint64_t receiver, uint64_t code, uint64_t action);

  static uint128_t combine_ids(const uint64_t &x, const uint64_t &y) {
    return (uint128_t{x} << 64) | y;
  };

  static void add_balance(eosio::name payer, eosio::asset quantity,
                   eosio::name contract) {
    require_auth(payer);

    ubalances_index ubalances(_me, payer.value);
    eosio::check(contract == _root_contract, "Неверный контракт токена");
    eosio::check(quantity.symbol == _root_symbol, "Неверный символ токена");

    auto balances_by_contract_and_symbol =
        ubalances.template get_index<"byconsym"_n>();
    auto contract_and_symbol_index =
        combine_ids(contract.value, quantity.symbol.code().raw());

    auto balance =
        balances_by_contract_and_symbol.find(contract_and_symbol_index);

    if (balance == balances_by_contract_and_symbol.end()) {
      ubalances.emplace(_me, [&](auto &b) {
        b.id = ubalances.available_primary_key();
        b.contract = contract;
        b.quantity = quantity;
      });
    } else {
      balances_by_contract_and_symbol.modify(
          balance, _me, [&](auto &b) { b.quantity += quantity; });
    };
  }

  static void sub_balance(eosio::name username, eosio::asset quantity,
                   eosio::name contract) {

    ubalances_index ubalances(_me, username.value);

    auto balances_by_contract_and_symbol =
        ubalances.template get_index<"byconsym"_n>();
    auto contract_and_symbol_index =
        combine_ids(contract.value, quantity.symbol.code().raw());

    auto balance =
        balances_by_contract_and_symbol.find(contract_and_symbol_index);

    eosio::check(balance != balances_by_contract_and_symbol.end(),
                 "Баланс не найден");

    eosio::check(balance->quantity >= quantity, "Недостаточный баланс");

    if (balance->quantity == quantity) {

      balances_by_contract_and_symbol.erase(balance);

    } else {

      balances_by_contract_and_symbol.modify(
          balance, _me, [&](auto &b) { b.quantity -= quantity; });
    }
  }


  struct [[eosio::table, eosio::contract("bet")]] balance {
    uint64_t id;          /*!< идентификатор баланса */
    eosio::name contract; /*!< имя контракта токена */
    eosio::asset quantity; /*!< количество токенов на балансе */

    uint64_t primary_key() const { return id; } /*!< return id - primary_key */

    uint128_t byconsym() const {
      return combine_ids(contract.value, quantity.symbol.code().raw());
    }
  };

  typedef eosio::multi_index<
      "balance"_n, balance,
      eosio::indexed_by<"byconsym"_n, eosio::const_mem_fun<balance, uint128_t,
                                                           &balance::byconsym>>>
      ubalances_index;

  eosio::checksum256 hashit(std::string str) const {
    return eosio::sha256(const_cast<char *>(str.c_str()), str.size());
  };

  uint64_t get_global_id(eosio::name key) {
    counts_index counts(_me, _me.value);
    auto count = counts.find(key.value);
    uint64_t id = 1;

    if (count == counts.end()) {
      counts.emplace(_me, [&](auto &c) {
        c.key = key;
        c.value = id;
      });
    } else {
      id = count->value + 1;
      counts.modify(count, _me, [&](auto &c) { c.value = id; });
    }

    return id;
  };

  struct [[eosio::table, eosio::contract("bet")]] counts {
    eosio::name key;
    eosio::name secondary_key;
    uint64_t value;

    uint64_t primary_key() const {
      return key.value;
    } /*!< return id - primary_key */
    uint128_t keyskey() const {
      return combine_ids(key.value, secondary_key.value);
    } /*!< (contract, blocked_now.symbol) - комбинированный secondary_key для
         получения курса по имени выходного контракта и символу */
    uint128_t keyvalue() const {
      return combine_ids(key.value, value);
    } /*!< (contract, blocked_now.symbol) - комбинированный secondary_key для
         получения курса по имени выходного контракта и символу */
  };

  typedef eosio::multi_index<
      "counts"_n, counts,
      eosio::indexed_by<"keyskey"_n, eosio::const_mem_fun<counts, uint128_t,
                                                          &counts::keyskey>>>
      counts_index;

  struct [[eosio::table, eosio::contract("bet")]] games {
    uint64_t id;
    uint64_t current_feed_id;
    std::string caption;
    eosio::name contract;
    eosio::asset reserve_fund;
    eosio::asset prize_fund;

    uint64_t primary_key() const { return id; };
  };

  typedef eosio::multi_index<"games"_n, games> games_index;

  struct [[eosio::table, eosio::contract("bet")]] feed {
    uint64_t id;
    uint64_t game_id;
    uint64_t prev_feed_id;
    uint64_t win_hypothesis_id;
    eosio::time_point timepoint;

    uint64_t primary_key() const { return id; };
    uint64_t by_timepoint() const { return timepoint.time_since_epoch().count(); };
    uint128_t by_game_timepoint() const { 
      return combine_ids(game_id, timepoint.time_since_epoch().count()); 
    };

  };

  typedef eosio::multi_index<"feed"_n, feed,
    eosio::indexed_by<"bytimepoint"_n, eosio::const_mem_fun<feed, uint64_t, &feed::by_timepoint>>,
    eosio::indexed_by<"bygamemtime"_n, eosio::const_mem_fun<feed, uint128_t, &feed::by_game_timepoint>>
  > feed_index;

  struct [[eosio::table, eosio::contract("bet")]] hypotheses {
    uint64_t id;
    uint64_t game_id;
    std::string title;

    uint64_t primary_key() const { return id; };
    uint64_t by_game_id() const { return game_id; };

  };

  typedef eosio::multi_index<
      "hypotheses"_n, hypotheses,
      eosio::indexed_by<"bygameid"_n, eosio::const_mem_fun<hypotheses, uint64_t,
                                                           &hypotheses::by_game_id>>>
      hypo_index;

  struct [[eosio::table, eosio::contract("bet")]] bets {
    uint64_t id;
    uint64_t game_id;
    eosio::name username;
    eosio::asset amount;
    eosio::time_point timepoint;

    uint64_t primary_key() const { return id; };
    uint64_t by_game() const {return game_id;}
    uint64_t by_timepoint() const { return timepoint.time_since_epoch().count(); };
  };

  typedef eosio::multi_index<
      "bets"_n, bets,
      eosio::indexed_by<"bytimepoint"_n, eosio::const_mem_fun<bets, uint64_t, &bets::by_timepoint>>,
      eosio::indexed_by<"bygame"_n, eosio::const_mem_fun<bets, uint64_t, &bets::by_game>>
      >
      bets_index;
};
