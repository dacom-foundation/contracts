
void capital::withdraw1(name coopname, name username, asset amount) {
    require_auth(username);
    check(amount.symbol == TOKEN_SYMBOL, "Invalid token symbol");
    check(amount.is_valid(), "Invalid asset");
    check(amount.amount > 0, "Amount must be positive");

    auto gs = get_global_state(coopname);

    // Participants table
    contributors_table contributors(_self, _self.value);
    auto idx = contributors.get_index<"byaccount"_n>();
    auto contributor_itr = idx.find(username.value);
    check(contributor_itr != idx.end(), "Participant not found");

    // Check if contributor is updated
    auto contributor = *contributor_itr;
    check(contributor.reward_per_share_last == gs.cumulative_reward_per_share, "Please refresh before withdrawing");

    // Calculate maximum amount available for withdrawal
    asset max_withdrawable = contributor.intellectual_contributions - contributor.withdrawed;
    check(max_withdrawable.amount >= amount.amount, "Insufficient balance to withdraw");

    // Update contributor data
    auto primary_itr = contributors.find(contributor_itr->primary_key());
    contributors.modify(primary_itr, same_payer, [&](auto& p) {
        p.share_balance -= amount;
        p.withdrawed += amount;
    });

    // Update global state
    gs.total_shares -= amount;
    gs.total_withdrawed += amount;
    update_global_state(gs);

    // TODO Create gateway withdraw payment
    
}

void capital::withdraw2(name coopname, name username, asset amount) {
    require_auth(username);
    check(amount.symbol == TOKEN_SYMBOL, "Invalid token symbol");
    check(amount.is_valid(), "Invalid asset");
    check(amount.amount > 0, "Amount must be positive");

    auto gs = get_global_state(coopname);

    // Participants table
    contributors_table contributors(_self, _self.value);
    auto idx = contributors.get_index<"byaccount"_n>();
    auto contributor_itr = idx.find(username.value);
    check(contributor_itr != idx.end(), "Participant not found");

    // Check if contributor is updated
    auto contributor = *contributor_itr;
    check(contributor.reward_per_share_last == gs.cumulative_reward_per_share, "Please refresh before withdrawing");

    // Calculate available balance excluding queued withdrawals
    asset available_balance = contributor.share_balance - contributor.queued_withdrawal;
    check(available_balance.amount >= amount.amount, "Insufficient balance to withdraw");

    // Add withdrawal claim to queue
    withdrawals_table withdrawals(_self, _self.value);
    withdrawals.emplace(_self, [&](auto& w) {
        w.id = withdrawals.available_primary_key();
        w.account = username;
        w.amount = amount;
        w.timestamp = current_time_point().sec_since_epoch();
    });

    // Update contributor data
    auto primary_itr = contributors.find(contributor_itr->primary_key());
    contributors.modify(primary_itr, same_payer, [&](auto& p) {
        p.queued_withdrawal += amount;
        p.share_balance -= amount; // Remove from share balance so it stops earning
    });

    // Update global state
    gs.total_shares -= amount; // Remove from total shares
    update_global_state(gs);

    // Process withdrawals
    // process_withdrawals(coopname);
}

