void capital::refresh(name coopname, name username) {
    require_auth(username);

    // auto gs = get_global_state(coopname);

    // // Participants table
    // contributors_table contributors(_self, _self.value);
    // auto idx = contributors.get_index<"byaccount"_n>();
    // auto contributor_itr = idx.find(username.value);
    // check(contributor_itr != idx.end(), "Participant not found");

    // auto contributor = *contributor_itr;

    // int64_t contributor_delta = gs.cumulative_reward_per_share - contributor.reward_per_share_last;

    // if (contributor_delta == 0) {
    //     // No update needed
    //     return;
    // }

    // int64_t reward_amount_int = (contributor.share_balance.amount * contributor_delta) / RATIO_DENOM;
    // asset reward_amount = asset(reward_amount_int, TOKEN_SYMBOL);

    // // Update contributor data
    // auto primary_itr = contributors.find(contributor_itr->primary_key());
    // contributors.modify(primary_itr, same_payer, [&](auto& p) {
    //     p.pending_rewards += reward_amount;
    //     p.reward_per_share_last = gs.cumulative_reward_per_share;
    //     p.share_balance += reward_amount;
    // });
}
