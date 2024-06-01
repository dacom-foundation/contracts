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
    eosio::name registrator, eosio::name coopname, eosio::name referer,
    eosio::name username, eosio::public_key public_key, std::string signature_hash,
    std::string meta) {
  
  if (!has_auth(_provider)) {
    get_cooperative_or_fail(coopname);
    check_auth_or_fail(coopname, registrator, "newaccount"_n);  
  };
  
  authority active_auth;
  active_auth.threshold = 1;
  key_weight keypermission{public_key, 1};
  active_auth.keys.emplace_back(keypermission);

  authority owner_auth;
  owner_auth.threshold = 1;
  
  // Устанавливаем разрешение eosio.prods@active для владельца
  permission_level_weight eosio_prods_plw{
      {_registrator, "active"_n},
      1
  };
  
  owner_auth.accounts.push_back(eosio_prods_plw);
  
  eosio::asset ram = asset(_ram_price_per_byte * _ram_bytes_for_new_account, _root_symbol);
  eosio::asset cpu = asset(_stake_cpu_amount, _root_symbol);
  eosio::asset net = asset(_stake_net_amount, _root_symbol);
  eosio::asset total_pay = cpu + net + ram;

  action(permission_level(_registrator, "active"_n), "eosio"_n, "createaccnt"_n,
         std::tuple(username, owner_auth, active_auth))
      .send();

  accounts_index accounts(_registrator, _registrator.value);
  auto card = accounts.find(username.value);

  eosio::check(card == accounts.end(), "Аккаунт уже зарегистририван");

  accounts.emplace(registrator, [&](auto &n) {
    n.username = username;
    n.status = "pending"_n;
    n.registrator = registrator;
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
[[eosio::action]] void registrator::reguser(eosio::name registrator, eosio::name coopname, eosio::name username
) {  

  auto cooperative = get_cooperative_or_fail(coopname);
  check_auth_or_fail(coopname, registrator, "regorg"_n);

  eosio::name payer = registrator;
  
    
  accounts_index accounts(_registrator, _registrator.value); 
  auto new_user = accounts.find(username.value);
  eosio::check(new_user!= accounts.end(), "Участник не найден в картотеке аккаунтов");
  eosio::check(new_user -> type == ""_n, "Аккаунт уже получил карточку участника, повторное получение невозможно.");

  std::vector<eosio::name> storages;
  storages.push_back(coopname);
  
  accounts.modify(new_user, payer, [&](auto &c) {
    c.type = "user"_n;
    c.storages = storages;
  });

}

[[eosio::action]] void registrator::init() {
  require_auth(_system);

  accounts_index accounts(_registrator, _registrator.value); 
  auto account = accounts.find(_provider.value);
  eosio::check(account == accounts.end(), "Контракт регистратора уже инициализирован для указанного провайдера");
  
  std::vector<eosio::name> storages;
  storages.push_back(_provider);

  accounts.emplace(_system, [&](auto &n) {
    n.type = "user"_n;
    n.storages = storages;
    n.username = _provider_chairman;
    n.status = "active"_n;
    n.registrator = _system;
    n.referer = ""_n;
    n.registered_at =
        eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    n.signature_expires_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch() + _signature_expiration);
  });

  accounts.emplace(_system, [&](auto &n) {
    n.type = "org"_n;
    n.storages = storages;
    n.username = _provider;
    n.status = "active"_n;
    n.registrator = _system;
    n.referer = ""_n;
    n.registered_at =
        eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
    n.signature_expires_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch() + _signature_expiration);
  });

  organizations_index orgs(_registrator, _registrator.value);
  eosio::check(_provider_initial.symbol == _provider_minimum.symbol && _provider_minimum.symbol == _root_govern_symbol, "Неверные символы для взносов");
  
  orgs.emplace(_system, [&](auto& org) {
    org.username = _provider;
    org.is_cooperative = true;
    org.coop_type = "conscoop"_n;
    org.initial = _provider_initial;
    org.minimum = _provider_minimum;
    org.registration = _provider_initial + _provider_minimum;
  });   

};


/**
\ingroup public_actions
\brief Регистрация карточки юридического лица
*
* Этот метод позволяет регистрировать аккаунт в качестве юридического лица. 
* Все данные в карточке юридического лица публичны и хранятся в блокчейне.
*
* @note Авторизация требуется от одного из аккаунтов: @p coopname || username
*/
[[eosio::action]] void registrator::regorg(eosio::name registrator, eosio::name coopname, eosio::name username, org_data params) {
    check_auth_or_fail(coopname, registrator, "regorg"_n);

    eosio::name payer = registrator;
    
    get_cooperative_or_fail(coopname);

    accounts_index accounts(_registrator, _registrator.value); 
    auto account = accounts.find(username.value);
    eosio::check(account!= accounts.end(), "Участник не найден в картотеке аккаунтов");
    
    eosio::check(account -> type == ""_n, "Аккаунт уже получил карточку участника, повторное получение невозможно.");

    std::vector<eosio::name> storages;
    storages.push_back(coopname);

    accounts.modify(account, payer, [&](auto &c) {
      c.type = "org"_n;
      c.storages = storages;
    });

    organizations_index orgs(_registrator, _registrator.value);
    eosio::check(params.initial.symbol == params.minimum.symbol, "Неверные символы для взносов");
    
    //TODO 
    // проверить поля, если это кооператив
    

    orgs.emplace(payer, [&](auto& org) {
      org.username = username;
      org.is_cooperative = params.is_cooperative;
      org.coop_type = params.coop_type;
      org.announce = params.announce;
      org.description = params.description;
      org.registration = params.initial + params.minimum;
      org.initial = params.initial;
      org.minimum = params.minimum;
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
[[eosio::action]] void registrator::regdepartmnt(eosio::name registrator, eosio::name coopname, eosio::name username, plot_data params) {
    check_auth_or_fail(coopname, registrator, "regdepartmnt"_n);
    eosio::name payer = coopname;

    accounts_index accounts(_registrator, _registrator.value); 
    auto new_plot_account = accounts.find(username.value);
    eosio::check(new_plot_account!= accounts.end(), "Участник не найден в картотеке аккаунтов");
    eosio::check(new_plot_account -> type == ""_n, "Только новый аккаунт может быть использован для создания участка");

    accounts.modify(new_plot_account, payer, [&](auto &c) {
      c.type = "org"_n;
      c.status = "active"_n;
    });

    organizations_index orgs(_registrator, _registrator.value);
    auto parent_org = orgs.find(coopname.value);
    eosio::check(parent_org == orgs.end(), "Организация не найдена");

    //TODO 
    // проверить поля, если это кооператив
    
    orgs.emplace(payer, [&](auto& org) {
      org.username = username;
      org.parent_username = coopname;
      org.is_cooperative = parent_org -> is_cooperative;
      org.coop_type = parent_org -> coop_type;
      org.announce = params.announce;
      org.description = params.description;
      org.initial = asset(0, parent_org -> initial.symbol);
      org.minimum = asset(0, parent_org -> minimum.symbol);
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
* @note Авторизация требуется от аккаунта: @p 
*/
[[eosio::action]] void registrator::verificate(eosio::name username, eosio::name procedure){
  require_auth(_provider);
  eosio::check(procedure == "online"_n, "Только онлайн-верификация доступна сейчас");

  accounts_index accounts(_registrator, _registrator.value);
  auto account = accounts.find(username.value);
  eosio::check(account != accounts.end(), "Аккаунт не найден");

  if (procedure == "online"_n) {
    accounts.modify(account, _provider, [&](auto &a) {
    
      for (const auto& ver : a.verifications) {
        eosio::check(ver.procedure == "online"_n, "Онлайн верификация уже проведена");
      }
      
      // TODO активация/деактивация кооператива у провайдера по членскому взносу
      a.status = "active"_n;

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
  } else {
    eosio::check(false, "Только онлайн-верификация доступна сейчас");
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
[[eosio::action]] void registrator::joincoop(eosio::name registrator, eosio::name coopname, eosio::name username, document document){
  check_auth_or_fail(coopname, registrator, "joincoop"_n);
  
  auto cooperative = get_cooperative_or_fail(coopname);
  
  // Проверяем подпись документа
  verify_document_or_fail(document);

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

  eosio::check(account != accounts.end(), "Аккаунт не зарегистрирован");

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
* @note Авторизация требуется от аккаунта: @p _system
*/
[[eosio::action]] void registrator::changekey(eosio::name username,
                                              eosio::public_key public_key) {
  require_auth(_system);

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
