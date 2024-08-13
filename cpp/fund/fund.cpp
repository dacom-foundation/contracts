#include "fund.hpp"

#include <ctime>
#include <eosio/transaction.hpp>

using namespace eosio;

[[eosio::action]] void fund::migrate() {
  require_auth(_fund);
  
  counts_index counts(_fund, _provider.value);
  
  counts.emplace(_fund, [&](auto &row){
    row.key = "funds"_n;
    row.value = 5;
  });
  
  coopwallet_index coopwallet(_fund, _provider.value);
  
  // кошелёк кооператива
  coopwallet.emplace(_fund, [&](auto &row) {
    row.id = 0;
    row.coopname = _provider;

    row.circulating_account.available = asset(0, _root_govern_symbol);
    row.circulating_account.withdrawed = asset(0, _root_govern_symbol);

    row.initial_account.available = asset(0, _root_govern_symbol);
    row.initial_account.withdrawed = asset(0, _root_govern_symbol);

    row.accumulative_account.available = asset(0, _root_govern_symbol);
    row.accumulative_account.withdrawed = asset(0, _root_govern_symbol);

    row.accumulative_expense_account.available = asset(0, _root_govern_symbol);
    row.accumulative_expense_account.withdrawed = asset(0, _root_govern_symbol);
  });
  
  
}

/**
 * @brief Пустой метод регистрации нового идентификатора
 * @ingroup public_actions
 * Этот метод используется для возврата информации из контракта.
 * @param id идентификатор
 * @param type тип идентификатора
 */
[[eosio::action]] void fund::newfund(eosio::name coopname, eosio::name type,
                                     uint64_t id) {
  require_auth(_fund);
};

/**
 * @brief Пустой метод регистрации нового идентификатора
 * @ingroup public_actions
 * Этот метод используется для возврата информации из контракта.
 * @param id идентификатор
 * @param type тип идентификатора
 */
[[eosio::action]] void fund::newwithdraw(eosio::name coopname, eosio::name type,
                                         uint64_t id) {
  require_auth(_fund);
};

/**
 * @brief Вызывается при запуске кооператива для создания кооперативного кошелька и некоторых фондов
 * 
 * @param coopname 
 * @param initial 
 */
[[eosio::action]] void fund::init(eosio::name coopname, eosio::asset initial) {
  require_auth(_soviet);

  coopwallet_index coopwallet(_fund, coopname.value);

  accfunds_index accfunds(_fund, coopname.value);
  expfunds_index expfunds(_fund, coopname.value);

  // кошелёк кооператива
  coopwallet.emplace(_soviet, [&](auto &row) {
    row.id = 0;
    row.coopname = coopname;

    row.circulating_account.available = asset(0, initial.symbol);
    row.circulating_account.withdrawed = asset(0, initial.symbol);

    row.initial_account.available = asset(0, initial.symbol);
    row.initial_account.withdrawed = asset(0, initial.symbol);

    row.accumulative_account.available = asset(0, initial.symbol);
    row.accumulative_account.withdrawed = asset(0, initial.symbol);

    row.accumulative_expense_account.available = asset(0, initial.symbol);
    row.accumulative_expense_account.withdrawed = asset(0, initial.symbol);
  });

  // неделимый
  accfunds.emplace(_soviet, [&](auto &a) {
    a.id = get_global_id_in_scope(_fund, coopname, "funds"_n);  // 1
    a.coopname = coopname;
    a.contract = ""_n;
    a.name = "Неделимый фонд";
    a.description = "";
    a.percent = 1 * ONE_PERCENT;
    a.available = asset(0, initial.symbol);
    a.withdrawed = asset(0, initial.symbol);
  });

  // резервный
  accfunds.emplace(_soviet, [&](auto &a) {
    a.id = get_global_id_in_scope(_fund, coopname, "funds"_n);  // 2
    a.coopname = coopname;
    a.contract = ""_n;
    a.name = "Резервный фонд";
    a.description = "";
    a.percent = 15 * ONE_PERCENT;
    a.available = asset(0, initial.symbol);
    a.withdrawed = asset(0, initial.symbol);
  });

  // развития
  accfunds.emplace(_soviet, [&](auto &a) {
    a.id = get_global_id_in_scope(_fund, coopname, "funds"_n);  // 3
    a.coopname = coopname;
    a.contract = ""_n;
    a.name = "Фонд развития кооперации";
    a.description = "";
    a.percent = 5 * ONE_PERCENT;
    a.available = asset(0, initial.symbol);
    a.withdrawed = asset(0, initial.symbol);
  });

  auto e_id = expfunds.available_primary_key();

  // хозяйственный
  expfunds.emplace(_soviet, [&](auto &e) {
    e.id = get_global_id_in_scope(_fund, coopname, "funds"_n);  // 4
    e.coopname = coopname;
    e.contract = ""_n;
    e.name = "Хозяйственный фонд";
    e.description = "";
    e.expended = asset(0, initial.symbol);
  });

  // взаимный
  expfunds.emplace(_soviet, [&](auto &e) {
    e.id = get_global_id_in_scope(_fund, coopname, "funds"_n);  // 5
    e.coopname = coopname;
    e.contract = ""_n;
    e.name = "Фонд взаимного обеспечения";
    e.description = "";
    e.expended = asset(0, initial.symbol);
  });
}

// атомарные транзакции фондового кошелька
// паевый счет
[[eosio::action]] void fund::addcirculate(
    eosio::name coopname,
    eosio::asset quantity)  /// < добавить сумму в паевый фонд
{
  eosio::name payer = check_auth_and_get_payer_or_fail({_gateway});

  auto cooperative = get_cooperative_or_fail(coopname);

  coopwallet_index coopwallet(_fund, coopname.value);

  auto wal = coopwallet.find(0);

  eosio::check(wal != coopwallet.end(), "Кошелёк кооператива не найден");

  coopwallet.modify(wal, _gateway, [&](auto &w) {
    w.circulating_account.available += quantity;
  });
};

[[eosio::action]] void fund::subcirculate(
    eosio::name coopname,
    eosio::asset quantity)  /// < списать сумму из паевого фонда
{
  // Только контракт шлюза может списывать оборотные средства из фонда
  eosio::name payer = check_auth_and_get_payer_or_fail({_gateway});

  auto cooperative = get_cooperative_or_fail(coopname);

  coopwallet_index coopwallet(_fund, coopname.value);

  auto wal = coopwallet.find(0);

  eosio::check(wal != coopwallet.end(), "Фондовый кошелёк не найден");

  eosio::check(wal->circulating_account.available >= quantity,
               "Недостаточно средств для списания на паевом счете кооператива");

  coopwallet.modify(wal, payer, [&](auto &w) {
    w.circulating_account.available -= quantity;
    w.circulating_account.withdrawed += quantity;
  });
};

// фонды накопления
[[eosio::action]] void fund::addaccum(eosio::name coopname, uint64_t fund_id,
                                      eosio::asset quantity) {
  require_auth(_fund);

  coopwallet_index coopwallet(_fund, coopname.value);
  auto wal = coopwallet.find(0);
  eosio::check(wal != coopwallet.end(), "Кошелёк кооператива не найден");

  coopwallet.modify(wal, _fund, [&](auto &row) {
    row.accumulative_account.available += quantity;
  });

  accfunds_index accfunds(_fund, coopname.value);
  auto afund = accfunds.find(fund_id);

  eosio::check(afund != accfunds.end(), "Фонд не найден");

  accfunds.modify(afund, _fund, [&](auto &a) { a.available += quantity; });
};

[[eosio::action]] void fund::subaccum(eosio::name coopname, uint64_t fund_id,
                                      eosio::asset quantity) {
  // списать можно только с помощью вызова метода withdraw смарт-контракта
  require_auth(_fund);

  coopwallet_index coopwallet(_fund, coopname.value);
  auto wal = coopwallet.find(0);
  eosio::check(wal != coopwallet.end(), "Фондовый кошелёк не найден");
  eosio::check(wal->accumulative_account.available >= quantity,
               "Недостаточно средств для списания");

  coopwallet.modify(wal, _fund, [&](auto &row) {
    row.accumulative_account.available -= quantity;
    row.accumulative_account.withdrawed += quantity;
  });

  accfunds_index accfunds(_fund, coopname.value);
  auto afund = accfunds.find(fund_id);

  eosio::check(afund != accfunds.end(), "Фонд не найден");

  accfunds.modify(afund, _fund, [&](auto &a) {
    a.available -= quantity;
    a.withdrawed += quantity;
  });
};

// фонды списания
[[eosio::action]] void fund::addexpense(eosio::name coopname, uint64_t fund_id,
                                        eosio::asset quantity) {
  require_auth(_fund);

  coopwallet_index coopwallet(_fund, coopname.value);
  auto wal = coopwallet.find(0);

  eosio::check(wal != coopwallet.end(), "Фондовый кошелёк не найден");

  eosio::check(wal->accumulative_expense_account.available +
                       wal->initial_account.available >=
                   quantity,
               "Недостаточно средств для списания");

  coopwallet.modify(wal, _fund, [&](auto &w) {

    /**
     * @brief проверить что списание идет по хозяйственному фонду
      если да - уменьшить счет членских взносов автоматически на всю возможную
      сумму. Остаток снять с накопительного счета списания.
     */
    if (fund_id == 4) {
  
      // Списываем из initial_account
      eosio::asset from_initial =
          std::min(quantity, w.initial_account.available);
      w.initial_account.available -= from_initial;
      w.initial_account.withdrawed += from_initial;

      // Если осталось списание, снимаем с accumulative_expense_account
      eosio::asset remaining = quantity - from_initial;
      if (remaining.amount > 0) {
        w.accumulative_expense_account.available -= remaining;
        w.accumulative_expense_account.withdrawed += remaining;
      }
    } else {
      w.accumulative_expense_account.available -= quantity;
      w.accumulative_expense_account.withdrawed += quantity;
    }
  });

  expfunds_index expfunds(_fund, coopname.value);
  auto efund = expfunds.find(fund_id);

  eosio::check(efund != expfunds.end(), "Фонд не найден");

  expfunds.modify(efund, _fund, [&](auto &a) { a.expended += quantity; });
};

/**
 * @brief Атомарный метод добавления вступительного взноса на счет кошелька кооператива.
 * 
 * @param coopname
 * @param quantity 
 */
[[eosio::action]] void fund::addinitial(eosio::name coopname,
                                        eosio::asset quantity) {
  eosio::name payer = check_auth_and_get_payer_or_fail({_gateway});

  auto cooperative = get_cooperative_or_fail(coopname);

  coopwallet_index coopwallet(_fund, coopname.value);
  auto wal = coopwallet.find(0);
  
  coopwallet.modify(
      wal, payer, [&](auto &row) { row.initial_account.available += quantity; });
}

// 

/**
 * @brief Метод распределения членской части взноса по фондам накопления с остатком в 
 кошельке для распределения по фондам списания. Распределить членские взносы по фондам накопления, положив остаток на накопительный счет списания. На входе мы получаем общую членскую часть, которую распределяем по счетам.
 * 
 * @param coopname 
 * @param quantity 
 */
[[eosio::action]] void fund::spreadamount(eosio::name coopname,
                                          eosio::asset quantity) {
  
  eosio::name payer = check_auth_and_get_payer_or_fail({_marketplace, _gateway, _soviet});

  auto cooperative = get_cooperative_or_fail(coopname);

  coopwallet_index coopwallet(_fund, coopname.value);
  auto wal = coopwallet.find(0);

  eosio::check(wal != coopwallet.end(), "Кошелёк кооператива не найден");

  accfunds_index accfunds(_fund, coopname.value);
  eosio::asset total_accumulated = asset(0, cooperative.initial.symbol);

  // готовим пакет транзакций пополнения счетов накопления
  for (auto it = accfunds.begin(); it != accfunds.end(); it++) {
    eosio::asset fund_quantity = it->percent * quantity / HUNDR_PERCENTS;
    total_accumulated += fund_quantity;
    action(permission_level{_fund, "active"_n}, _fund, "addaccum"_n,
           std::make_tuple(coopname, it->id, fund_quantity))
        .send();
  };

  // считаем сколько не выплачено по счетам накопления
  eosio::asset remain_amount = quantity - total_accumulated;

  // начисляем на накопительный счет списания
  coopwallet.modify(wal, payer, [&](auto &w) {
    w.accumulative_expense_account.available += remain_amount;
  });
};

/* Метод создания вывода средств из фонда накопления или по фонду списания
 * Использование метода не накладывает условие проверки на наличие средств в
 * целевом фонде. Проверка на наличие происходит в момент списания (complete)
 * после утверждения решения советом. Т.е. совет может принять решение о будущем
 * расходе, но фактическое списание осуществится только при наличии средств в
 * фондах.
 */
[[eosio::action]] void fund::fundwithdraw(eosio::name coopname,
                                          eosio::name username,
                                          eosio::name type, uint64_t fund_id,
                                          document document,
                                          eosio::asset quantity,
                                          std::string bank_data_id) {
  eosio::check(type == _afund_withdraw_action || type == _efund_withdraw_action,
               "Неверный тип фонда");
  eosio::name payer;

  if (type == _afund_withdraw_action) {
    accfunds_index accfunds(_fund, coopname.value);
    auto afund = accfunds.find(fund_id);
    eosio::check(afund != accfunds.end(), "Фонд не найден");
    payer = afund->contract != ""_n ? afund->contract : username;
  } else if (type == _efund_withdraw_action) {
    expfunds_index expfunds(_fund, coopname.value);
    auto efund = expfunds.find(fund_id);
    eosio::check(efund != expfunds.end(), "Фонд не найден");
    payer = efund->contract != ""_n ? efund->contract : username;
  }

  require_auth(payer);

  if (payer == username) {
    staff_index staff(_soviet, coopname.value);
    auto persona = staff.find(username.value);
    eosio::check(persona != staff.end(),
                 "Указанный аккаунт не является сотрудником");
    eosio::check(persona->has_right(_soviet, "complete"_n),
                 "Недостаточно прав доступа");
  };

  fundwithdraws_index fundwithdraws(_fund, coopname.value);
  uint64_t fundwithdraw_id = get_global_id(_fund, "fundwithdraw"_n);

  fundwithdraws.emplace(payer, [&](auto &s) {
    s.id = fundwithdraw_id;
    s.type = type;
    s.status = "pending"_n;
    s.coopname = coopname;
    s.username = username;
    s.fund_id = fund_id;
    s.quantity = quantity;
    s.document = document;
    s.bank_data_id = bank_data_id;
  });

  action(permission_level{_fund, "active"_n}, _soviet, "fundwithdraw"_n,
         std::make_tuple(coopname, username, type, fundwithdraw_id, document))
      .send();

  // оповещаем себя же
  action(permission_level{_fund, "active"_n}, _fund, "newwithdraw"_n,
         std::make_tuple(coopname, type, fundwithdraw_id))
      .send();
}

[[eosio::action]] void fund::authorize(eosio::name coopname, eosio::name type,
                                       uint64_t withdraw_id) {
  require_auth(_soviet);

  fundwithdraws_index fundwithdraws(_fund, coopname.value);
  auto withdraw = fundwithdraws.find(withdraw_id);
  eosio::check(withdraw != fundwithdraws.end(), "Вывод не найден");

  fundwithdraws.modify(withdraw, _soviet,
                       [&](auto &s) { s.status = "authorized"_n; });
};

[[eosio::action]] void fund::complete(eosio::name coopname,
                                      eosio::name username,
                                      uint64_t withdraw_id) {
  require_auth(username); 
  // todo check rights for username
  
  staff_index staff(_soviet, coopname.value);
  auto persona = staff.find(username.value);

  eosio::check(persona != staff.end(),
               "Указанный аккаунт не является сотрудником");
  eosio::check(persona->has_right(_soviet, "complete"_n),
               "Недостаточно прав доступа");

  fundwithdraws_index fundwithdraws(_fund, coopname.value);
  auto withdraw = fundwithdraws.find(withdraw_id);
  eosio::check(withdraw != fundwithdraws.end(), "Вывод не найден");

  fundwithdraws.modify(withdraw, _soviet, [&](auto &s) {
    s.status = "completed"_n;
    s.expired_at = eosio::time_point_sec(
        eosio::current_time_point().sec_since_epoch() + 30 * 86400);
  });

  if (withdraw->type == _afund_withdraw_action) {
    accfunds_index accfunds(_fund, coopname.value);
    auto afund = accfunds.find(withdraw->fund_id);
    eosio::check(afund != accfunds.end(), "Фонд не найден");

    action(permission_level{_fund, "active"_n}, _fund, "subaccum"_n,
           std::make_tuple(coopname, withdraw->fund_id, withdraw->quantity))
        .send();
  } else if (withdraw->type == _efund_withdraw_action) {
    expfunds_index expfunds(_fund, coopname.value);
    auto efund = expfunds.find(withdraw->fund_id);
    eosio::check(efund != expfunds.end(), "Фонд не найден");

    action(permission_level{_fund, "active"_n}, _fund, "addexpense"_n,
           std::make_tuple(coopname, withdraw->fund_id, withdraw->quantity))
        .send();
  }
};

// type: accumulation, expend
[[eosio::action]] void fund::createfund(eosio::name coopname,
                                        eosio::name username, eosio::name type,
                                        eosio::name contract, std::string name,
                                        std::string description,
                                        uint64_t percent) {
  require_auth(username);

  eosio::check(type == "accumulation"_n || type == "expend"_n,
               "Неверный тип фонда");

  auto cooperative = get_cooperative_or_fail(coopname);

  auto soviet = get_board_by_type_or_fail(coopname, "soviet"_n);
  auto chairman = soviet.get_chairman();

  eosio::check(username == chairman,
               "Только председатель может управлять фондами");

  uint64_t id;

  if (type == "accumulation"_n) {
    eosio::check(percent > 0 && percent <= HUNDR_PERCENTS,
                 "Процент фонда накопления должен быть больше нуля и меньше 1 "
                 "000 000 (= 100%)");

    accfunds_index accfunds(_fund, coopname.value);
    uint64_t total_percent = percent;

    // получаем сумму процентов всех фондов
    for (auto it = accfunds.begin(); it != accfunds.end(); it++) {
      total_percent += it->percent;
    }

    // сумма не должна превышать 100%
    check(total_percent <= HUNDR_PERCENTS,
          "Сумма всех процентов превышает 100% (1 000 000)");
    id = get_global_id(_fund, "funds"_n);
    accfunds.emplace(username, [&](auto &a) {
      a.id = get_global_id(_fund, "funds"_n);
      a.coopname = coopname;
      a.contract = contract;
      a.name = name;
      a.description = description;
      a.percent = percent;
      a.available = asset(0, cooperative.initial.symbol);
      a.withdrawed = asset(0, cooperative.initial.symbol);
    });
  } else if (type == "expend"_n) {
    eosio::check(
        percent == 0,
        "Процент для фонда списания должен быть равен нулю (не используется)");

    expfunds_index expfunds(_fund, coopname.value);
    id = get_global_id(_fund, "funds"_n);

    expfunds.emplace(username, [&](auto &e) {
      e.id = id;
      e.coopname = coopname;
      e.contract = contract;
      e.name = name;
      e.description = description;
      e.expended = asset(0, cooperative.initial.symbol);
    });
  };

  action(permission_level{_fund, "active"_n}, _fund, "newfund"_n,
         std::make_tuple(coopname, type, id))
      .send();
};

[[eosio::action]] void fund::editfund(eosio::name coopname,
                                      eosio::name username, eosio::name type,
                                      uint64_t fund_id, eosio::name contract,
                                      std::string name, std::string description,
                                      uint64_t percent) {
  require_auth(username);

  eosio::check(type == "accumulation"_n || type == "expend"_n,
               "Неверный тип фонда");

  auto cooperative = get_cooperative_or_fail(coopname);

  auto soviet = get_board_by_type_or_fail(coopname, "soviet"_n);
  auto chairman = soviet.get_chairman();
  eosio::check(username == chairman,
               "Только председатель может управлять фондами");

  
  if (fund_id <= 5){
    /**
    Фонд развития кооперации (3) и резервный фонд (2) проценты менять можно.
    Названия фондов менять нельзя (id < 5).
    */
    
    eosio::check(fund_id != 0, "Неделимый фонд не может быть отредактирован");
  
  }


  if (type == "accumulation"_n) {
    accfunds_index accfunds(_fund, coopname.value);
    auto afund = accfunds.find(fund_id);
    eosio::check(afund != accfunds.end(), "Фонд не найден");
    
    if (fund_id <= 3){
      eosio::check(fund_id == 1, "Нельзя изменить процент неделимого фонда накопления");
      eosio::check(name == afund -> name, "Нельзя изменить имя обязательного фонда накопления");  
      eosio::check(contract == ""_n, "Нельзя передать в управление обязательный фонд");
    }
    
    eosio::check(
        percent > 0 && percent <= HUNDR_PERCENTS,
        "Процент фонда накопления должен быть больше нуля и меньше 100%");
    uint64_t total_percent = 0;

    // получаем сумму процентов всех фондов
    for (auto it = accfunds.begin(); it != accfunds.end(); it++) {
      total_percent += it->percent;
    }

    // вычитаем старый процент и добавляем новый
    total_percent += percent - afund->percent;

    // сумма процентов не должна превышать 100%
    check(total_percent <= HUNDR_PERCENTS,
          "Сумма всех процентов превышает 100%");

    accfunds.modify(afund, username, [&](auto &a) {
      a.contract = contract;
      a.name = name;
      a.description = description;
      a.percent = percent;
    });
  } else if (type == "expend"_n) {
    eosio::check(
        percent == 0,
        "Процент для фонда списания должен быть равен нулю (не используется)");

    expfunds_index expfunds(_fund, coopname.value);
    auto efund = expfunds.find(fund_id);
    eosio::check(efund != expfunds.end(), "Фонд не найден");
  
    if (fund_id <= 5){
      eosio::check(name == efund -> name, "Нельзя изменить имя обязательного фонда накопления");  
      eosio::check(contract == ""_n, "Нельзя передать в управление обязательный фонд");
    }

    expfunds.modify(efund, username, [&](auto &e) {
      e.contract = contract;
      e.name = name;
      e.description = description;
    });
  };
};

[[eosio::action]] void fund::delfund(eosio::name coopname, eosio::name username,
                                     eosio::name type, uint64_t fund_id) {
  require_auth(username);

  eosio::check(type == "accumulation"_n || type == "expend"_n,
               "Неверный тип фонда");

  auto cooperative = get_cooperative_or_fail(coopname);

  auto soviet = get_board_by_type_or_fail(coopname, "soviet"_n);
  auto chairman = soviet.get_chairman();
  eosio::check(username == chairman,
               "Только председатель может управлять фондами");

  eosio::check(fund_id > 5, "Нельзя удалить обязательные фонды");

  if (type == "accumulation"_n) {
    accfunds_index accfunds(_fund, coopname.value);
    auto afund = accfunds.find(fund_id);
    eosio::check(afund != accfunds.end(), "Фонд не найден");
    eosio::check(afund->available.amount == 0,
                 "Только пустой фонд накопления может быть удален");

    accfunds.erase(afund);
  } else if (type == "expend"_n) {
    expfunds_index expfunds(_fund, coopname.value);
    auto efund = expfunds.find(fund_id);
    eosio::check(efund != expfunds.end(), "Фонд не найден");

    expfunds.erase(efund);
  }
};
