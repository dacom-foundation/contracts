void capital::accumulate(name coopname, asset amount) {
    require_auth(_self); // Only the contract account can call this action
    check(amount.symbol == ACCUMULATION_SYMBOL, "Invalid token symbol");
    check(amount.is_valid(), "Invalid asset");
    check(amount.amount > 0, "Amount must be positive");

    auto gs = get_global_state(coopname);

    gs.accumulated_amount += amount;
    update_global_state(gs);

    // Process withdrawals after adding fees
    // process_withdrawals(coopname);
}

