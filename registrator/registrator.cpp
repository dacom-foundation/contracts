#include "registrator.hpp"

/**
 * @brief Регистрирует новый аккаунт.
 * 
 * Действие позволяет создать новый аккаунт. Новый аккаунт может быть создан только верифицированной организацией. 
 * @note Авторизация требуется от аккаунта: @p registrator
 *
 * @param registrator Аккаунт, который оплачивает создание нового аккаунта.
 * @param referer Реферер, который представил нового пользователя.
 * @param username Имя нового аккаунта (от 5 до 12 символов).
 * @param public_key Открытый ключ нового аккаунта.
 * @param meta Дополнительная мета-информация.
 *
 * @ingroup public_actions
 */
[[eosio::action]] void registrator::newaccount(
    eosio::name registrator, eosio::name referer,
    eosio::name username, eosio::public_key public_key, std::string signature_hash,
    std::string meta) {
  
  require_auth(registrator);
  
  authority active_auth;
  active_auth.threshold = 1;
  key_weight keypermission{public_key, 1};
  active_auth.keys.emplace_back(keypermission);

  authority owner_auth;
  owner_auth.threshold = 1;
  
  // Устанавливаем разрешение eosio.prods@active для владельца
  permission_level_weight eosio_prods_plw{
      {eosio::name("eosio.prods"), "active"_n},
      1
  };
  owner_auth.accounts.push_back(eosio_prods_plw);
  
  auto ram_price = eosiosystem::determine_ram_price(_ram_bytes);
  eosio::asset cpu = asset(_stake_cpu_amount, _root_symbol);
  eosio::asset net = asset(_stake_net_amount, _root_symbol);
  eosio::asset total_pay = cpu + net + ram_price;

  registrator::sub_balance(_registrator, registrator, total_pay, _root_contract);

  action(permission_level(_registrator, "active"_n), "eosio"_n, "newaccount"_n,
         std::tuple(_registrator, username, owner_auth, active_auth))
      .send();

  action(permission_level{_registrator, "active"_n}, "eosio"_n, "buyram"_n,
         std::make_tuple(_registrator, username, ram_price))
      .send();

  action(permission_level{_registrator, "active"_n}, "eosio"_n, "delegatebw"_n,
         std::make_tuple(_registrator, username, net, cpu, true))
      .send();

  accounts_index accounts(_registrator, _registrator.value);
  auto card = accounts.find(username.value);

  eosio::check(card == accounts.end(), "account has been already registered");

  accounts.emplace(registrator, [&](auto &n) {
    n.username = username;
    n.status = "pending"_n;
    n.registrator = registrator;
    n.registration_amount = total_pay;
    n.referer = referer;
    n.registered_at =
        eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    n.signature_expires_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch() + _signature_expiration);
    n.meta = meta;
  });
}




/**
\ingroup public_actions
\brief Регистрация пользователя
*
* Этот метод предназначен для регистрации аккаунта в качестве физического лица.
* После регистрации пользователь получает статус "user". Принимает хэш-ссылку на зашифрованный профиль, сохраненный в IPFS.
*
* @param registrator Имя регистратора, который регистрирует (обычно, кооператив, но может быть и участником, который регистрирует свою карточку сам)
* @param username Имя пользователя, который регистрируется
* @param profile_hash Хэш-ссылка на зашифрованный профиль пользователя, сохраненный в IPFS
* 
* @note Авторизация требуется от аккаунта: @p registrator
*/
[[eosio::action]] void registrator::reguser(
   eosio::name coopname,
   eosio::name username,
   storage storage
) {  

  eosio::check(has_auth(coopname), "Только регистратор может подписать эту транзакцию");
  eosio::name payer = coopname;
  
  if (coopname != _ano)
    auto cooperative = get_cooperative_or_fail(coopname);

  accounts_index accounts(_registrator, _registrator.value); 
  auto new_user = accounts.find(username.value);
  eosio::check(new_user!= accounts.end(), "Участник не найден в картотеке аккаунтов");

  accounts.modify(new_user, payer, [&](auto &c) {
    c.type = "user"_n;
  });

  users_index users(_registrator, _registrator.value);
  auto user = users.find(username.value);

  std::vector<struct storage> storages;

  eosio::check(user == users.end(), "Карта пользователя уже существует");

  storages.push_back(storage);
  
  users.emplace(payer, [&](auto &u) {
    u.username = username;
    u.storages = storages;
  });

}




/**
\ingroup public_actions
\brief Регистрация юридического лица
*
* Этот метод позволяет регистрировать аккаунт в качестве юридического лица. 
* Все данные в карточке юридического лица публичны и хранятся в блокчейне.
*
* @param params Структура данных нового юридического лица
* 
* @note Авторизация требуется от одного из аккаунтов: @p coopname || username
*/
[[eosio::action]] void registrator::regorg(eosio::name coopname, eosio::name username, org_data params) {
    has_auth(coopname);
    eosio::name payer = coopname;

    accounts_index accounts(_registrator, _registrator.value); 
    auto new_user = accounts.find(username.value);
    eosio::check(new_user!= accounts.end(), "Участник не найден в картотеке аккаунтов");

    accounts.modify(new_user, payer, [&](auto &c) {
      c.type = "org"_n;
    });

    orgs_index orgs(_registrator, _registrator.value);

    //TODO 
    // проверить поля, если это кооператив
    std::vector<storage> storages;
    
    storages.push_back(params.storage);

    orgs.emplace(payer, [&](auto& org) {
      org.username = username;
      org.storages = storages;
      org.is_active = false;
      org.is_cooperative = params.is_cooperative;
      org.coop_type = params.coop_type;
      org.token_contract = params.token_contract;
      org.announce = params.announce;
      org.description = params.description;
      org.initial = params.initial;
      org.minimum = params.minimum;
    });   

}

/**
\ingroup public_actions
\brief Верификация аккаунта
*
* Этот метод позволяет верифицировать аккаунты как пользователей, так и организаций.
* На данный момент может быть применен только аккаунтом автономной некоммерческой организации "Кооперативная Экономика".
*
* @param username Имя аккаунта, который подлежит верификации
* 
* @note Авторизация требуется от аккаунта: @p _ano
*/
[[eosio::action]] void registrator::verificate(eosio::name username, eosio::name procedure){
  require_auth(_ano);
  eosio::check(procedure == "online"_n, "Только онлайн-верификация доступна сейчас");

  accounts_index accounts(_registrator, _registrator.value);
  auto account = accounts.find(username.value);
  eosio::check(account != accounts.end(), "Аккаунт не найден");

  if (procedure == "online"_n) {
    if (account->type == "user"_n){
      users_index users(_registrator, _registrator.value);
      auto user = users.find(username.value);
      eosio::check(user != users.end(), "Пользователь не найден");

      users.modify(user, _ano, [&](auto &a){
        for (const auto& ver : a.verifications) {
          eosio::check(ver.procedure == "online"_n, "Онлайн верификация уже проведена");
        }
        
        a.is_active = true;

        verification new_verification {
          .verificator = _ano,
          .is_verified = true,
          .procedure = procedure,
          .created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch()),
          .last_update = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch()),
          .notice = ""
        };

        a.verifications.push_back(new_verification);
      });
    } else if (account->type == "org"_n){
      orgs_index orgs(_registrator, _registrator.value);
      auto org = orgs.find(username.value);
      eosio::check(org != orgs.end(), "Организация не найдена");
      
      orgs.modify(org, _ano, [&](auto &a){
        for (const auto& ver : a.verifications) {
          eosio::check(ver.procedure == "online"_n, "Онлайн верификация уже проведена");
        }

        a.is_active = true;

        verification new_verification {
          .verificator = _ano,
          .is_verified = true,
          .procedure = procedure,
          .created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch()),
          .last_update = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch()),
          .notice = ""
        };

        a.verifications.push_back(new_verification);
      });
    }
  }
}

/**
\ingroup public_actions
\brief Подача заявления на членство в кооперативе
*
* Этот метод позволяет подать заявление на вступление в кооператив от имени физического или юридического лица. 
* После подачи заявления, оно направляется на рассмотрение в совет кооператива для голосования.
*
* @param coopname Имя кооператива
* @param username Имя заявителя
* @param position_title Наименование должности заявителя
* @param position Код должности заявителя, который может быть одним из следующего списка: 
*  - chairman
*  - vpchairman
*  - director
*  - vpdirector
*  - boardmember
*  - execmember
*  - votingmember
*  - assocmember
* @param ricardian_data Переменные шаблона заявления, которые вставляются в рикардианский договор, содержащийся в ABI метода действия контракта.
* @param statement_hash Хэш ссылка на заявление
* 
* @note Авторизация требуется от аккаунта: @p username
*/
[[eosio::action]] void registrator::joincoop(eosio::name coopname, eosio::name username, document document){
  require_auth(coopname);

  verify(document);

  participants_index participants(_soviet, coopname.value);
  auto participant = participants.find(username.value);
  eosio::check(participant == participants.end(), "Участник уже является членом кооператива"); 

  action(permission_level{_registrator, "active"_n}, _soviet, "joincoop"_n,
         std::make_tuple(coopname, username, document))
  .send();
};


/**
\ingroup public_actions
\brief Обновление метаданных аккаунта
*
* Этот метод позволяет обновить метаданные указанного аккаунта. 
* Только владелец аккаунта имеет право обновлять его метаданные.
*
* @param username Имя аккаунта, который требуется обновить
* @param meta Новые метаданные для аккаунта
* 
* @note Авторизация требуется от аккаунта: @p username
*/
[[eosio::action]] void registrator::update(eosio::name username,
                                           std::string meta) {
  require_auth(username);

  accounts_index accounts(_registrator, _registrator.value);

  auto account = accounts.find(username.value);

  eosio::check(account != accounts.end(), "account is not registered");

  accounts.modify(account, username, [&](auto &acc) {
    acc.meta = meta;
  });
}




/**
\ingroup public_actions
\brief Изменение ключа активной учетной записи
*
* Этот метод позволяет изменить активный ключ указанной учетной записи.
* Только аккаунт автономной некомерческой организации "Кооперативная Экономика" имеет право изменять ключи учетных записей.
*
* @param username Имя аккаунта, ключ которого требуется изменить
* @param public_key Новый публичный ключ для активной учетной записи
* 
* @note Авторизация требуется от аккаунта: @p _ano
*/
[[eosio::action]] void registrator::changekey(eosio::name username,
                                              eosio::public_key public_key) {
  require_auth(_ano);

  accounts_index accounts(_registrator, _registrator.value);

  auto card = accounts.find(username.value);

  if (card != accounts.end()) {
    authority active_auth;
    active_auth.threshold = 1;
    key_weight keypermission{public_key, 1};
    active_auth.keys.emplace_back(keypermission);

    // Change active authority of card to a new key
    eosio::action(eosio::permission_level(card->username, eosio::name("owner")),
                  eosio::name("eosio"), eosio::name("updateauth"),
                  std::tuple(card->username, eosio::name("active"),
                             eosio::name("owner"), active_auth))
    .send();
  }
}



/**
\ingroup public_actions
\brief Подтверждение регистрации члена кооператива
*
* Этот метод позволяет подтвердить регистрацию пользователя (физического или юридического лица) в качестве члена кооператива.
* Подтверждение может быть осуществлено только аккаунтом контракта совета кооператива после принятия соответствующего решения.
*
* @param coopname Имя кооператива
* @param member Имя члена кооператива
* @param position_title Название должности
* @param position Код должности (например, chairman, director и др.)
* 
* @note Авторизация требуется от аккаунта: @p _soviet
*/
[[eosio::action]] void registrator::confirmreg(eosio::name coopname, eosio::name member) {
  require_auth(_soviet);

  accounts_index accounts(_registrator, _registrator.value);
  auto account = accounts.find(member.value);

  eosio::check(account != accounts.end(), "Аккаунт не найден в картотеке");

  accounts.modify(account, _soviet, [&](auto &g) { g.status = "active"_n; });

}

extern "C" {

/// The apply method implements the dispatch of events to this contract
void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  if (code == ::_registrator.value) {
    if (action == "update"_n.value) {
      execute_action(name(receiver), name(code), &registrator::update);
    } else if (action == "confirmreg"_n.value) {
      execute_action(name(receiver), name(code), &registrator::confirmreg);
    } else if (action == "newaccount"_n.value) {
      execute_action(name(receiver), name(code), &registrator::newaccount);
    } else if (action == "reguser"_n.value) {
      execute_action(name(receiver), name(code), &registrator::reguser);
    } else if (action == "regorg"_n.value) {
      execute_action(name(receiver), name(code), &registrator::regorg);
    } else if (action == "changekey"_n.value) {
      execute_action(name(receiver), name(code), &registrator::changekey);
    } else if (action == "joincoop"_n.value) {
      execute_action(name(receiver), name(code), &registrator::joincoop);
    } else if (action == "verificate"_n.value){
      execute_action(name(receiver), name(code), &registrator::verificate);
    }

} else { if (action == "transfer"_n.value) { struct transfer { eosio::name from; eosio::name to; eosio::asset quantity; std::string memo; };
      auto op = eosio::unpack_action_data<transfer>();
      if (op.to == _registrator) {

        eosio::name username = eosio::name(op.memo.c_str());
        
        registrator::add_balance(_registrator, username, op.quantity, eosio::name(code));
      }
    }
  }
};
};
