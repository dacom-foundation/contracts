void capital::contribute(name coopname, name administrator, name owner, checksum256 idea_hash, checksum256 contribution_hash, asset amount) {
    check_auth_or_fail(_capital, coopname, administrator, "contribute"_n);

    check(amount.symbol == TOKEN_SYMBOL, "Invalid token symbol");
    check(amount.is_valid(), "Invalid asset");
    check(amount.amount > 0, "Amount must be positive");

    // Регистрируем участника (если он ещё не существует)
    ensure_contributor(coopname, owner);

    // принять взнос в программу
    // 


    // TODO: зарегистрировать имущественный взнос в объекте contribute
    // create_contribute(coopname, idea_hash, contribution_hash, amount);
}