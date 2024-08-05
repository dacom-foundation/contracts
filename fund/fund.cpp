#include "fund.hpp"
#include <ctime>
#include <eosio/transaction.hpp>

using namespace eosio;


/**
 * @brief Пустой метод регистрации нового идентификатора
 * @ingroup public_actions
 * Этот метод используется для возврата информации из контракта.
 * @param id идентификатор
 * @param type тип идентификатора
 */
  [[eosio::action]] void fund::newfund(eosio::name coopname, eosio::name type, uint64_t id) {
    require_auth(_fund);
  };

/**
 * @brief Пустой метод регистрации нового идентификатора
 * @ingroup public_actions
 * Этот метод используется для возврата информации из контракта.
 * @param id идентификатор
 * @param type тип идентификатора
 */
  [[eosio::action]] void fund::newwithdraw(eosio::name coopname, eosio::name type, uint64_t id) {
    require_auth(_fund);
  };



  [[eosio::action]] void fund::init(eosio::name coopname, eosio::asset initial) {
    //вызывается при запуске кооператива для создания кооперативного кошелька и некоторых фондов
    require_auth(_soviet);

    fundwallet_index fundwallet(_fund, coopname.value);
    accfunds_index accfunds(_fund, coopname.value);
    expfunds_index expfunds(_fund, coopname.value);

    //создаём кошелёк
    fundwallet.emplace(_soviet, [&](auto &w){
      w.id = 0;
      w.coopname = coopname;
      w.circulating = asset(0, initial.symbol);
      w.membership = asset(0, initial.symbol);
      w.accumulated = asset(0, initial.symbol);
      w.withdrawed = asset(0, initial.symbol);
      w.available = asset(0, initial.symbol);
      w.expended = asset(0, initial.symbol);
    });

    

    //неделимый 
    accfunds.emplace(_soviet, [&](auto &a) {
      a.id = get_global_id(_fund, "funds"_n);
      a.coopname = coopname;
      a.contract = ""_n;
      a.name = "Неделимый фонд";
      a.description = "";
      a.percent = 1 * ONE_PERCENT;
      a.available = asset(0, initial.symbol);
      a.withdrawed = asset(0, initial.symbol);
    });

    //резервный
    accfunds.emplace(_soviet, [&](auto &a) {
      a.id = get_global_id(_fund, "funds"_n);
      a.coopname = coopname;
      a.contract = ""_n;
      a.name = "Резервный фонд";
      a.description = "";
      a.percent = 15 * ONE_PERCENT;
      a.available = asset(0, initial.symbol);
      a.withdrawed = asset(0, initial.symbol);
    });


    //развития
    accfunds.emplace(_soviet, [&](auto &a) {
      a.id = get_global_id(_fund, "funds"_n);
      a.coopname = coopname;
      a.contract = ""_n;
      a.name = "Фонд развития кооперации";
      a.description = "";
      a.percent = 5 * ONE_PERCENT;
      a.available = asset(0, initial.symbol);
      a.withdrawed = asset(0, initial.symbol);
    });


    auto e_id = expfunds.available_primary_key(); 
  
    //хозяйственный
    expfunds.emplace(_soviet, [&](auto &e){
      e.id = get_global_id(_fund, "funds"_n);
      e.coopname = coopname;
      e.contract = ""_n;
      e.name = "Хозяйственный фонд";
      e.description = "";
      e.expended = asset(0, initial.symbol);
    });

    //взаимный
    expfunds.emplace(_soviet, [&](auto &e){
      e.id = get_global_id(_fund, "funds"_n);
      e.coopname = coopname;
      e.contract = ""_n;
      e.name = "Фонд взаимного обеспечения";
      e.description = "";
      e.expended = asset(0, initial.symbol);
    });

    
  }

//type: accumulation, expend
  [[eosio::action]] void fund::createfund(eosio::name coopname, eosio::name username, eosio::name type, eosio::name contract, std::string name, std::string description, uint64_t percent){

    require_auth(username);

    eosio::check(type == "accumulation"_n || type == "expend"_n, "Неверный тип фонда");

    auto cooperative = get_cooperative_or_fail(coopname);

    auto soviet = get_board_by_type_or_fail(coopname, "soviet"_n);
    auto chairman = soviet.get_chairman();

    eosio::check(username == chairman, "Только председатель может управлять фондами");
    
    uint64_t id;

    if (type == "accumulation"_n) {

      eosio::check(percent > 0 && percent <= HUNDR_PERCENTS, "Процент фонда накопления должен быть больше нуля и меньше 1 000 000 (= 100%)");
      
      accfunds_index accfunds(_fund, coopname.value);
      uint64_t total_percent = percent;

      //получаем сумму процентов всех фондов
      for (auto it = accfunds.begin(); it != accfunds.end(); it++) {
        total_percent += it->percent;
      }

      //сумма не должна превышать 100%
      check(total_percent <= HUNDR_PERCENTS, "Сумма всех процентов превышает 100% (1 000 000)");
      id = get_global_id(_fund, "funds"_n);
      accfunds.emplace(username, [&](auto &a){
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
      eosio::check(percent == 0, "Процент для фонда списания должен быть равен нулю (не используется)");
      
      expfunds_index expfunds(_fund, coopname.value);
      id = get_global_id(_fund, "funds"_n);
      
      expfunds.emplace(username, [&](auto &e){
        e.id = id;
        e.coopname = coopname;
        e.contract = contract;
        e.name = name;
        e.description = description;
        e.expended = asset(0, cooperative.initial.symbol);
      });

    };

    action(
      permission_level{ _fund, "active"_n},
      _fund,
      "newfund"_n,
      std::make_tuple(coopname, type, id)
    ).send();
  
  };



  [[eosio::action]] void fund::editfund(eosio::name coopname, eosio::name username, eosio::name type, uint64_t fund_id, eosio::name contract, std::string name, std::string description, uint64_t percent){
    require_auth(username);

    eosio::check(type == "accumulation"_n || type == "expend"_n, "Неверный тип фонда");
    
    auto cooperative = get_cooperative_or_fail(coopname);

    auto soviet = get_board_by_type_or_fail(coopname, "soviet"_n);
    auto chairman = soviet.get_chairman();
    eosio::check(username == chairman, "Только председатель может управлять фондами");
    
    eosio::check(fund_id != 0, "Неделимый фонд не может быть отредактирован");    

    if (type == "accumulation"_n) {

      accfunds_index accfunds(_fund, coopname.value);
      auto afund = accfunds.find(fund_id);
      eosio::check(afund != accfunds.end(), "Фонд не найден");

      eosio::check(percent > 0 && percent <= HUNDR_PERCENTS, "Процент фонда накопления должен быть больше нуля и меньше 100%");
      uint64_t total_percent = 0;

      //получаем сумму процентов всех фондов
      for (auto it = accfunds.begin(); it != accfunds.end(); it++) {
        total_percent += it->percent;
      }

      //вычитаем старый процент и добавляем новый
      total_percent += percent - afund -> percent;

      //сумма процентов не должна превышать 100%
      check(total_percent <= HUNDR_PERCENTS, "Сумма всех процентов превышает 100%");

      accfunds.modify(afund, username, [&](auto &a){
        a.contract = contract;
        a.name = name;
        a.description = description;
        a.percent = percent;
      });


    } else if (type == "expend"_n) {
      eosio::check(percent == 0, "Процент для фонда списания должен быть равен нулю (не используется)");

      expfunds_index expfunds(_fund, coopname.value);
      auto efund = expfunds.find(fund_id);
      eosio::check(efund != expfunds.end(), "Фонд не найден");

      expfunds.modify(efund, username, [&](auto &e){
        e.contract = contract;
        e.name = name;
        e.description = description;
      });

    };
  };

  [[eosio::action]] void fund::delfund(eosio::name coopname, eosio::name username, eosio::name type, uint64_t fund_id){
    require_auth(username);

    eosio::check(type == "accumulation"_n || type == "expend"_n, "Неверный тип фонда");

    auto cooperative = get_cooperative_or_fail(coopname);

    auto soviet = get_board_by_type_or_fail(coopname, "soviet"_n);
    auto chairman = soviet.get_chairman();
    eosio::check(username == chairman, "Только председатель может управлять фондами");
    
    eosio::check(fund_id > 2, "Нельзя удалить обязательные фонды");

    if (type == "accumulation"_n) {
      accfunds_index accfunds(_fund, coopname.value);
      auto afund = accfunds.find(fund_id);
      eosio::check(afund != accfunds.end(), "Фонд не найден");
      eosio::check(afund -> available.amount == 0, "Только пустой фонд накопления может быть удален");

      accfunds.erase(afund);

    } else if (type == "expend"_n) {

      expfunds_index expfunds(_fund, coopname.value);
      auto efund = expfunds.find(fund_id);
      eosio::check(efund != expfunds.end(), "Фонд не найден");
      
      expfunds.erase(efund);

    }

  };







  //атомарные транзакции фондового кошелька
    //паевый фонд
  [[eosio::action]] void fund::addcirculate(eosio::name coopname, eosio::asset quantity) /// < добавить сумму в паевый фонд
  {
    //Только контракт шлюза может добавлять оборотные средства в фонд
    require_auth(_gateway);
    
    auto cooperative = get_cooperative_or_fail(coopname);

    fundwallet_index fundwallet(_fund, coopname.value);

    auto wal = fundwallet.find(0);

    eosio::check(wal != fundwallet.end(), "Фондовый кошелёк не найден");

    fundwallet.modify(wal, _gateway, [&](auto &w){
      w.circulating += quantity;
    });

  };



  [[eosio::action]] void fund::subcirculate(eosio::name coopname, eosio::asset quantity) /// < списать сумму из паевого фонда
  {
    //Только контракт шлюза может списывать оборотные средства из фонда
    require_auth(_gateway);
    
    auto cooperative = get_cooperative_or_fail(coopname);

    fundwallet_index fundwallet(_fund, coopname.value);

    auto wal = fundwallet.find(0);

    eosio::check(wal != fundwallet.end(), "Фондовый кошелёк не найден");

    eosio::check(wal -> circulating >= quantity, "Недостаточно средств для списания в фонде");

    fundwallet.modify(wal, _gateway, [&](auto &w) {
      w.circulating -= quantity;
    });

  };




  //фонды накопления
  [[eosio::action]] void fund::addaccum( eosio::name coopname, uint64_t fund_id, eosio::asset quantity) {
    require_auth(_fund);

    fundwallet_index fundwallet(_fund, coopname.value);
    auto wal = fundwallet.find(0);
    eosio::check(wal != fundwallet.end(), "Фондовый кошелёк не найден");

    fundwallet.modify(wal, _fund, [&](auto &w){
      w.membership += quantity;
      w.accumulated += quantity;
    });

    accfunds_index accfunds(_fund, coopname.value);
    auto afund = accfunds.find(fund_id);
    
    eosio::check(afund != accfunds.end(), "Фонд не найден");

    accfunds.modify(afund, _fund, [&](auto &a){
      a.available += quantity;
    });

  };


  [[eosio::action]] void fund::subaccum( eosio::name coopname, uint64_t fund_id, eosio::asset quantity){
    //списать можно только с помощью вызова метода withdraw смарт-контракта
    require_auth(_fund);

    fundwallet_index fundwallet(_fund, coopname.value);
    auto wal = fundwallet.find(0);
    eosio::check(wal != fundwallet.end(), "Фондовый кошелёк не найден");
    eosio::check(wal -> accumulated >= quantity, "Недостаточно средств для списания");

    fundwallet.modify(wal, _fund, [&](auto &w){
      w.accumulated -= quantity;
      w.withdrawed += quantity;
    });

    accfunds_index accfunds(_fund, coopname.value);
    auto afund = accfunds.find(fund_id);
    
    eosio::check(afund != accfunds.end(), "Фонд не найден");

    accfunds.modify(afund, _fund, [&](auto &a){
      a.available -= quantity;
      a.withdrawed += quantity;
    });

  };



  //фонды списания
  [[eosio::action]] void fund::addexpense( eosio::name coopname, uint64_t fund_id, eosio::asset quantity){
    require_auth(_fund);

    fundwallet_index fundwallet(_fund, coopname.value);
    auto wal = fundwallet.find(0);

    eosio::check(wal != fundwallet.end(), "Фондовый кошелёк не найден");
    
    fundwallet.modify(wal, _fund, [&](auto &w){
      w.membership += quantity;
      w.available -= quantity;
    });

    expfunds_index expfunds(_fund, coopname.value);
    auto efund = expfunds.find(fund_id);
    
    eosio::check(efund != expfunds.end(), "Фонд не найден");

    expfunds.modify(efund, _fund, [&](auto &a){
      a.expended += quantity;
    });

  };

  
  
  //метод распределения членской части взноса по фондам накопления с остатком в кошельке для распределения по фондам списания
  [[eosio::action]] void fund::spreadamount( eosio::name coopname, eosio::asset quantity) {
    /// < распределить членские взносы по фондам накопления, положив остаток в фондовый кошелёк для дальнейшего списания 
    //на входе мы получаем общую членскую часть
    eosio::check(has_auth(_marketplace) || has_auth(_gateway), "Недостаточно прав доступа");
    eosio::name payer = has_auth(_marketplace) ? _marketplace : _gateway;

    fundwallet_index fundwallet(_fund, coopname.value);
    auto wal = fundwallet.find(0);

    eosio::check(wal != fundwallet.end(), "Фондовый кошелёк не найден");
    
    auto cooperative = get_cooperative_or_fail(coopname);

    accfunds_index accfunds(_fund, coopname.value);
    eosio::asset total_accumulated = asset(0, cooperative.initial.symbol);
    
    //готовим пакет транзакций пополнения фондов накопления
    for (auto it = accfunds.begin(); it != accfunds.end(); it++) {
      
      eosio::asset fund_quantity = it -> percent * quantity / HUNDR_PERCENTS;
      total_accumulated += fund_quantity;
      action(
        permission_level{ _fund, "active"_n},
        _fund,
        "addaccum"_n,
        std::make_tuple(coopname, it -> id, fund_quantity)
      ).send();
    }

    //считаем сколько не выплачено в фонды накопления
    eosio::asset remain_amount = quantity - total_accumulated;

    //начисляем в кошелёк
    fundwallet.modify(wal, payer, [&](auto &w) {
      w.available += remain_amount;
      w.membership += remain_amount;
    });

  };
  

  //метод создания вывода средств из фонда накопления или по фонду списания
  [[eosio::action]] void fund::fundwithdraw(eosio::name coopname, eosio::name username, eosio::name type, uint64_t fund_id, document document, eosio::asset quantity, std::string bank_data_id) {
    eosio::check(type == _afund_withdraw_action || type == _efund_withdraw_action, "Неверный тип фонда");
    
    eosio::name payer;

    if (type == _afund_withdraw_action) {
      accfunds_index accfunds(_fund, coopname.value);
      auto afund = accfunds.find(fund_id);
      eosio::check(afund != accfunds.end(), "Фонд не найден");
      payer = afund -> contract != ""_n ? afund -> contract : username;
    } else if (type == _efund_withdraw_action) {
      expfunds_index expfunds(_fund, coopname.value);
      auto efund = expfunds.find(fund_id);
      eosio::check(efund != expfunds.end(), "Фонд не найден");
      payer = efund -> contract != ""_n ? efund -> contract : username;
    }
    
    require_auth(payer);

    if (payer == username) {
      //TODO проверить username на право доступа? 
      staff_index staff(_soviet, coopname.value);
      auto persona = staff.find(username.value);
      eosio::check(persona != staff.end(), "Указанный аккаунт не является сотрудником");
      eosio::check(persona -> has_right(_soviet, "complete"_n), "Недостаточно прав доступа"); 
    };
    

    fundwithdraws_index fundwithdraws(_fund, coopname.value);
    uint64_t fundwithdraw_id = get_global_id(_fund, "fundwithdraw"_n);

    fundwithdraws.emplace(payer, [&](auto &s){
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

    action(
      permission_level{ _fund, "active"_n},
      _soviet,
      "fundwithdraw"_n,
      std::make_tuple(coopname, username, type, fundwithdraw_id, document)
    ).send();
    
    //оповещаем себя же
    action(
      permission_level{ _fund, "active"_n},
      _fund,
      "newwithdraw"_n,
      std::make_tuple(coopname, type, fundwithdraw_id)
    ).send();
    
  }



  [[eosio::action]] void fund::authorize(eosio::name coopname, eosio::name type, uint64_t withdraw_id) {
    require_auth(_soviet);

    fundwithdraws_index fundwithdraws(_fund, coopname.value);
    auto withdraw = fundwithdraws.find(withdraw_id);
    eosio::check(withdraw != fundwithdraws.end(), "Вывод не найден");

    fundwithdraws.modify(withdraw, _soviet, [&](auto &s) {
      s.status = "authorized"_n;
    });

  };


  [[eosio::action ]] void fund::complete(eosio::name coopname, eosio::name username, uint64_t withdraw_id){
    require_auth(username);
    //todo check rights for username

    staff_index staff(_soviet, coopname.value);
    auto persona = staff.find(username.value);

    eosio::check(persona != staff.end(), "Указанный аккаунт не является сотрудником");
    eosio::check(persona -> has_right(_soviet, "complete"_n), "Недостаточно прав доступа");

    fundwithdraws_index fundwithdraws(_fund, coopname.value);
    auto withdraw = fundwithdraws.find(withdraw_id);
    eosio::check(withdraw != fundwithdraws.end(), "Вывод не найден");

    fundwithdraws.modify(withdraw, _soviet, [&](auto &s) {
      s.status = "completed"_n;
      s.expired_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch() + 30 * 86400);
    });

    if (withdraw -> type == _afund_withdraw_action) {
      accfunds_index accfunds(_fund, coopname.value);
      auto afund = accfunds.find(withdraw -> fund_id);
      eosio::check(afund != accfunds.end(), "Фонд не найден");

      action(
        permission_level{ _fund, "active"_n},
        _fund,
        "subaccum"_n,
        std::make_tuple(coopname, withdraw -> fund_id, withdraw -> quantity)
      ).send();

    } else if (withdraw -> type == _efund_withdraw_action) {
      expfunds_index expfunds(_fund, coopname.value);
      auto efund = expfunds.find(withdraw -> fund_id);
      eosio::check(efund != expfunds.end(), "Фонд не найден");

      action(
        permission_level{ _fund, "active"_n},
        _fund,
        "addexpense"_n,
        std::make_tuple(coopname, withdraw -> fund_id, withdraw -> quantity)
      ).send();
    }

  };