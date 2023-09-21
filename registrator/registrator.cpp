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
    eosio::name username, eosio::public_key public_key,
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

  sub_balance(_registrator, registrator, total_pay, _root_contract);

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
* @param username Имя регистратора, который регистрирует (обычно, кооператив, но может быть и участником, который регистрирует свою карточку сам)
* @param username Имя пользователя, который регистрируется
* @param profile_hash Хэш-ссылка на зашифрованный профиль пользователя, сохраненный в IPFS
* 
* @note Авторизация требуется от аккаунта: @p registrator
*/
[[eosio::action]] void registrator::reguser(
   eosio::name registrator,
   eosio::name username,
   storage storage
) {  

  eosio::check(has_auth(registrator) || has_auth(username), "Только регистратор или пользователь могут подписать эту транзакцию");
  eosio::name payer = has_auth(registrator) ? registrator : username;
    
  accounts_index accounts(_registrator, _registrator.value); 
  auto new_user = accounts.find(username.value);
  eosio::check(new_user!= accounts.end(), "Участник не найден в картотеке аккаунтов");

  accounts.modify(new_user, payer, [&](auto &c) {
    c.type = "user"_n;
  });

  users_index users(_registrator, _registrator.value);
  auto user = users.find(username.value);

  std::vector<struct storage> storages;

  if (user!= users.end()) {

    storages = user -> storages;
    storages.push_back(storage);
    
    users.modify(user, payer, [&](auto &u){
      u.storages = storages;
    });

  } else {

    storages.push_back(storage);

    users.emplace(payer, [&](auto &u) {
      u.username = username;
      u.storages = storages;
    });
  }
}




/**
\ingroup public_actions
\brief Регистрация юридического лица
*
* Этот метод позволяет регистрировать аккаунт в качестве юридического лица. 
* Все данные в карточке юридического лица публичны и хранятся в блокчейне.
*
* @param new_org Структура данных нового юридического лица
* 
* @note Авторизация требуется от одного из аккаунтов: @p registrator || username
*/
[[eosio::action]] void registrator::regorg(eosio::name registrator, eosio::name username, org_data new_org) {
    require_auth(registrator);  

    eosio::check(has_auth(registrator) || has_auth(username), "Только регистратор или пользователь могут подписать эту транзакцию");
    eosio::name payer = has_auth(registrator) ? registrator : username;

    orgs_index orgs(_registrator, _registrator.value);

    orgs.emplace(payer, [&](auto& org) {
      org.username = username;
      org.name = new_org.name;
      org.short_name = new_org.short_name;
      org.address = new_org.address;
      org.ogrn = new_org.ogrn;
      org.inn = new_org.inn;
      org.logo = new_org.logo;
      org.phone = new_org.phone;
      org.email = new_org.email;
      org.registration = new_org.registration;
      org.website = new_org.website;
      org.accounts = new_org.accounts;
      org.is_cooperative = new_org.is_cooperative;
      org.coop_type = new_org.coop_type;
      org.token_contract = new_org.token_contract;
      org.slug = new_org.slug;
      org.announce = new_org.announce;
      org.description = new_org.description;
      org.initial = new_org.initial;
      org.minimum = new_org.minimum;
      org.membership = new_org.membership;
      org.period = new_org.period;
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
[[eosio::action]] void registrator::verificate(eosio::name username){
  require_auth(_ano);

  accounts_index accounts(_registrator, _registrator.value);
  auto account = accounts.find(username.value);
  eosio::check(account != accounts.end(), "Аккаунт не найден");
 
 //  if (account -> type == "user"_n){
 //    users_index users(_registrator, _registrator.value);
 //    auto user = users.find(username.value);
 //    eosio::check(user != users.end(), "Пользователь не найден");
 //    users.modify(user, _ano, [&](auto &a){
 //      a.verification.verificator = _ano;
 //      a.verification.is_verified = true;
 //      a.verification.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
 //      a.verification.last_update = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
 //    });
 // } else if (account -> type == "org"_n){
 //    orgs_index orgs(_registrator, _registrator.value);
 //    auto org = orgs.find(username.value);
 //    eosio::check(org != orgs.end(), "Организация не найдена");
    
 //    orgs.modify(org, _ano, [&](auto &a){
 //      a.verification.verificator = _ano;
 //      a.verification.is_verified = true;
 //      a.verification.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
 //      a.verification.last_update = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
 //    });
 //  }
}


/**
\ingroup public_actions
\brief Подача заявления на членство в кооперативе
*
* Этот метод позволяет подать заявление на вступление в кооператив от имени физического или юридического лица. 
* После подачи заявления, оно направляется на рассмотрение в совет кооператива для голосования.
*
* @param coop_username Имя кооператива
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
[[eosio::action]] void registrator::joincoop(eosio::name coop_username, eosio::name username, std::string position_title, eosio::name position, signed_doc signed_doc){
  require_auth(username);

  verify(signed_doc);

  members_index members(_registrator, coop_username.value);
  auto member = members.find(username.value);
  eosio::check(member == members.end(), "Участник уже является членом кооператива"); 

  action(permission_level{_registrator, "active"_n}, _soviet, "joincoop"_n,
         std::make_tuple(coop_username, username, position_title, position))
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
* @param coop_username Имя кооператива
* @param member Имя члена кооператива
* @param position_title Название должности
* @param position Код должности (например, chairman, director и др.)
* 
* @note Авторизация требуется от аккаунта: @p _soviet
*/
[[eosio::action]] void registrator::confirmreg(eosio::name coop_username, eosio::name member, std::string position_title, eosio::name position) {
  require_auth(_soviet);

  accounts_index accounts(_registrator, _registrator.value);
  auto account = accounts.find(member.value);
  eosio::check(account != accounts.end(), "account is not found");

  accounts.modify(account, _soviet, [&](auto &g) { g.status = "active"_n; });

  members_index members(_registrator, coop_username.value);
  members.emplace(_soviet, [&](auto &m){
    m.username = member;
    m.created_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    m.last_update = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    m.is_accepted = true;
    m.is_initial = true;
    m.is_minimum = true;
    m.position_title = position_title;
    m.position = position;
  });
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
    }
} else { if (action == "transfer"_n.value) { struct transfer { eosio::name from; eosio::name to; eosio::asset quantity; std::string memo; };
      auto op = eosio::unpack_action_data<transfer>();
      if (op.to == _registrator) {

        eosio::name username = eosio::name(op.memo.c_str());
        
        add_balance(_registrator, username, op.quantity, eosio::name(code));
      }
    }
  }
};
};
