#include <eosio.system/exchange_state.hpp>
#include <eosio/check.hpp>
#include <cmath>

namespace eosiosystem {

  using eosio::check;

  // Цена за байт в малых единицах токена
  constexpr int64_t _ram_price_per_byte = 1; //что даёт 10000 байт или 9.76 кб за 1.0000 AXON

  asset exchange_state::convert_to_exchange(connector& reserve_connector, const asset& payment) {
    int64_t ram_bytes = payment.amount / _ram_price_per_byte;
    reserve_connector.balance += payment;
    supply.amount += ram_bytes;
    
    return asset(ram_bytes, supply.symbol);
  }

  asset exchange_state::convert_from_exchange(connector& reserve_connector, const asset& tokens) {
    int64_t ram_cost = tokens.amount * _ram_price_per_byte;
    reserve_connector.balance.amount -= ram_cost;
    supply -= tokens;

    return asset(ram_cost, reserve_connector.balance.symbol);
  }

  asset exchange_state::convert(const asset& from, const symbol& to_symbol) {
    check(from.symbol != to_symbol, "cannot convert to the same symbol");

    int64_t ram_cost_or_revenue;
    if (from.symbol == base.balance.symbol) {
      ram_cost_or_revenue = from.amount * _ram_price_per_byte;
      base.balance.amount += from.amount;
      quote.balance.amount -= ram_cost_or_revenue;
    } else {
      ram_cost_or_revenue = from.amount * _ram_price_per_byte;
      quote.balance.amount += from.amount;
      base.balance.amount -= ram_cost_or_revenue;
    }

    return asset(ram_cost_or_revenue, to_symbol);
  }

  asset exchange_state::direct_convert(const asset& from, const symbol& to_symbol) {
    return convert(from, to_symbol);
  }

  int64_t exchange_state::get_bancor_output(int64_t input_reserve, int64_t output_reserve, int64_t input_amount) {
    int64_t output_amount = input_amount * _ram_price_per_byte;
    return output_amount;
  }

  int64_t exchange_state::get_bancor_input(int64_t output_reserve, int64_t input_reserve, int64_t output_amount) {
    int64_t input_amount = output_amount / _ram_price_per_byte;
    return input_amount;
  }

} /// namespace eosiosystem
