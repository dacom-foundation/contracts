#include "registrator.hpp"

// [[eosio::action]] void registrator::check(checksum256 hash, public_key public_key, signature signature)
// {
//   assert_recover_key(hash, signature, public_key);
// }

[[eosio::action]] void registrator::migrate() {
  require_auth(_registrator);
}

[[eosio::action]] void registrator::init()
{
  require_auth(_system);

  accounts_index accounts(_registrator, _registrator.value);
  auto account = accounts.find(_provider.value);
  eosio::check(account == accounts.end(), "Контракт регистратора уже инициализирован для указанного провайдера");

  std::vector<eosio::name> storages;
  storages.push_back(_provider);

  accounts.emplace(_system, [&](auto &n)
  {
      n.type = "user"_n;
      n.storages = storages;
      n.username = _provider_chairman;
      n.status = "active"_n;
      n.registrator = _system;
      n.referer = ""_n;
      n.registered_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch()); 
  });

  accounts.emplace(_system, [&](auto &n)
  {
      n.type = "organization"_n;
      n.storages = storages;
      n.username = _provider;
      n.status = "active"_n;
      n.registrator = _system;
      n.referer = ""_n;
      n.registered_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch()); 
  });

  cooperatives_index coops(_registrator, _registrator.value);
  eosio::check(_provider_initial.symbol == _provider_minimum.symbol && _provider_minimum.symbol == _root_govern_symbol, "Неверные символы для взносов");
  eosio::check(_provider_org_initial.symbol == _provider_org_minimum.symbol && _provider_org_minimum.symbol == _root_govern_symbol, "Неверные символы для взносов");

  eosio::check(_provider_org_initial.amount > 0 && _provider_org_minimum.amount > 0 && _provider_initial.amount > 0 && _provider_minimum.amount > 0, "Вступительный и минимальный паевые взносы должны быть положительными");


  coops.emplace(_system, [&](auto &org)
  {
    org.username = _provider;
    org.is_cooperative = true;
    org.coop_type = "conscoop"_n;
    org.initial = _provider_initial;
    org.minimum = _provider_minimum;
    org.registration = _provider_initial + _provider_minimum; 

    org.org_initial = _provider_org_initial;
    org.org_minimum = _provider_org_minimum;
    org.org_registration = _provider_org_initial + _provider_org_minimum; 
  });

};


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
 * 
 * Диаграмма процесса и inline транзакций: 
 * 1. registrator::adduser (
 *  - добавляем аккаунт
 *  2. soviet::adduser(
 *    - добавляем пайщика
 *    - добавляем кошелёк с минимальным взносом
 *    3. gateway::adduser(
 *    - устанавливаем дату вступления
 *    - фиксируем принятый взнос в реестре взносов
 *      4. fund::addcirculate (добавляем минимальный взнос)
 *      5. fund::spreadamount? (опционально распределяем вступительный взнос по фондам)
 *    )
 *  )
 * )
 * 
 * @ingroup public_actions
 */
[[eosio::action]] void registrator::adduser(
    eosio::name registrator, eosio::name coopname, eosio::name referer,
    eosio::name username, eosio::name type , eosio::time_point_sec created_at, 
    eosio::asset initial, eosio::asset minimum, bool spread_initial, std::string meta)
{

  auto cooperative = get_cooperative_or_fail(coopname);
  check_auth_or_fail(coopname, registrator, "adduser"_n);
  
  eosio::check(created_at.sec_since_epoch() <= eosio::current_time_point().sec_since_epoch(), "Дата created_at должна быть в прошлом" );
  eosio::check(cooperative.initial.symbol == initial.symbol, "Неверный символ");
  eosio::check(cooperative.initial.symbol == minimum.symbol, "Неверный символ");
  
  authority active_auth;
  active_auth.threshold = 1; 

  authority owner_auth;
  owner_auth.threshold = 1;

  // Устанавливаем разрешение eosio.prods@active для владельца
  permission_level_weight eosio_prods_plw{{_registrator, "active"_n},1};
  owner_auth.accounts.push_back(eosio_prods_plw);

  // Добавьте пустые ключи в active_auth
  active_auth.accounts.push_back(eosio_prods_plw);
  print("on !!!");
  // регистрируем аккаунт  
  action(permission_level(_registrator, "active"_n), "eosio"_n, "createaccnt"_n,
         std::tuple(coopname, username, owner_auth, active_auth))
  .send();
  print("oz !!!");
  accounts_index accounts(_registrator, _registrator.value);
  
  auto card = accounts.find(username.value);

  eosio::check(card == accounts.end(), "Аккаунт уже зарегистририван");

  eosio::check(type == "individual"_n || type == "entrepreneur"_n || type == "organization"_n, "Неверный тип пользователя, допустимы только: individual, entrepreneur и organization.");
  
  std::vector<eosio::name> storages;
  
  storages.push_back(coopname);

  accounts.emplace(registrator, [&](auto &n)
    {
      n.username = username;
      n.status = "active"_n;
      n.registrator = registrator;
      n.referer = referer;
      n.type = type;
      n.storages = storages; 
      n.registered_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
      n.meta = meta; 
    });
    
  
  action(permission_level{_registrator, "active"_n}, _soviet, "adduser"_n,
     std::make_tuple(coopname, username, type, created_at, initial, minimum, spread_initial))
  .send();
      
}




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
    eosio::name username, eosio::public_key public_key, std::string meta)
{

  if (!has_auth(_provider))
  {
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
      1};

  owner_auth.accounts.push_back(eosio_prods_plw);

  action(permission_level(_registrator, "active"_n), "eosio"_n, "createaccnt"_n,
         std::tuple(coopname, username, owner_auth, active_auth))
      .send();

  accounts_index accounts(_registrator, _registrator.value);
  auto card = accounts.find(username.value);

  eosio::check(card == accounts.end(), "Аккаунт уже зарегистририван");

  accounts.emplace(registrator, [&](auto &n)
    {
      n.username = username;
      n.status = "pending"_n;
      n.registrator = registrator;
      n.referer = referer;
      n.registered_at =
          eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
      n.meta = meta; 
    });
}

/**
\ingroup public_actions
\brief Регистрация пользователя
*
* Этот метод предназначен для регистрации аккаунта в качестве физического лица.
* После регистрации пользователь получает статус "user". 
*
* @param registrator Имя регистратора, который регистрирует (обычно, кооператив, но может быть и участником, который регистрирует свою карточку сам)
* @param username Имя пользователя, который регистрируется
* @param profile_hash Хэш-ссылка на зашифрованный профиль пользователя, сохраненный в IPFS
*
* @note Авторизация требуется от аккаунта: @p registrator
*/
[[eosio::action]] void registrator::reguser(eosio::name registrator, eosio::name coopname, eosio::name username, eosio::name type)
{

  auto cooperative = get_cooperative_or_fail(coopname);
  check_auth_or_fail(coopname, registrator, "reguser"_n);

  eosio::name payer = registrator;

  accounts_index accounts(_registrator, _registrator.value);
  auto new_user = accounts.find(username.value);
  eosio::check(new_user != accounts.end(), "Участник не найден в картотеке аккаунтов");
  eosio::check(new_user->type == ""_n, "Аккаунт уже получил карточку участника, повторное получение невозможно.");
  eosio::check(type == "individual"_n || type == "entrepreneur"_n || type == "organization"_n, "Неверный тип пользователя, допустимы только: individual, entrepreneur и organization.");
  
  std::vector<eosio::name> storages;
  storages.push_back(coopname);

  accounts.modify(new_user, payer, [&](auto &c)
  {
    c.type = type;
    c.storages = storages; 
  });

}



/**
\ingroup public_actions
\brief Регистрация карточки юридического лица
*
* Этот метод позволяет регистрировать аккаунт в качестве юридического лица.
* Все данные в карточке юридического лица публичны и хранятся в блокчейне.
*
* @note Авторизация требуется от одного из аккаунтов: @p coopname || username
*/
[[eosio::action]] void registrator::regcoop(eosio::name registrator, eosio::name coopname, eosio::name username, org_data params)
{
  check_auth_or_fail(coopname, registrator, "regcoop"_n);

  eosio::name payer = registrator;

  get_cooperative_or_fail(coopname);

  accounts_index accounts(_registrator, _registrator.value);
  auto account = accounts.find(username.value);
  eosio::check(account != accounts.end(), "Участник не найден в картотеке аккаунтов");

  eosio::check(account->type == ""_n, "Аккаунт уже получил карточку участника, повторное получение невозможно.");

  std::vector<eosio::name> storages;
  storages.push_back(coopname);

  accounts.modify(account, payer, [&](auto &c)
  {
    c.type = "organization"_n;
    c.storages = storages; 
  });

  cooperatives_index coops(_registrator, _registrator.value);
  eosio::check(params.initial.symbol == params.minimum.symbol && params.initial.symbol == _root_govern_symbol, "Неверные символы для взносов");
  eosio::check(params.org_initial.symbol == params.org_minimum.symbol && params.org_initial.symbol == _root_govern_symbol, "Неверные символы для взносов");  

  eosio::check(params.initial.amount > 0 && params.org_initial.amount > 0 && params.minimum.amount > 0 && params.org_minimum.amount > 0, "Вступительный и минимальный паевые взносы должны быть положительными");

  coops.emplace(payer, [&](auto &org)
    {
      org.username = username;
      org.is_cooperative = params.is_cooperative;
      org.coop_type = params.coop_type;
      org.announce = params.announce;
      org.description = params.description;
      org.registration = params.initial + params.minimum;
      org.initial = params.initial;
      org.minimum = params.minimum; 
      org.org_registration = params.org_initial + params.org_minimum;
      org.org_initial = params.org_initial;
      org.org_minimum = params.org_minimum; 
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
[[eosio::action]] void registrator::verificate(eosio::name username, eosio::name procedure)
{
  require_auth(_provider);
  eosio::check(procedure == "online"_n, "Только онлайн-верификация доступна сейчас");

  accounts_index accounts(_registrator, _registrator.value);
  auto account = accounts.find(username.value);
  eosio::check(account != accounts.end(), "Аккаунт не найден");

  if (procedure == "online"_n)
  {
    accounts.modify(account, _provider, [&](auto &a)
                    {
    
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

      a.verifications.push_back(new_verification); });
  }
  else
  {
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
[[eosio::action]] void registrator::joincoop(eosio::name registrator, eosio::name coopname, eosio::name username, document document)
{
  check_auth_or_fail(coopname, registrator, "joincoop"_n);
  require_recipient(username);
  
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
[[eosio::action]] void registrator::updateaccnt(eosio::name username, eosio::name account_to_change, std::string meta)
{
  require_auth(username);

  if (account_to_change != username)
    check_auth_or_fail(account_to_change, username, "updateaccnt"_n);
  
  accounts_index accounts(_registrator, _registrator.value);

  auto account = accounts.find(account_to_change.value);

  eosio::check(account != accounts.end(), "Аккаунт не зарегистрирован");

  accounts.modify(account, username, [&](auto &acc)
                  { acc.meta = meta; });
}


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
[[eosio::action]] void registrator::updatecoop(eosio::name coopname, eosio::name username, eosio::asset initial, eosio::asset minimum, eosio::asset org_initial, eosio::asset org_minimum, std::string announce, std::string description)
{
  require_auth(username);
  
  check_auth_or_fail(coopname, username, "newaccount"_n);

  accounts_index accounts(_registrator, _registrator.value);

  auto account = accounts.find(username.value);

  eosio::check(account != accounts.end(), "Аккаунт не зарегистрирован");

  cooperatives_index coops(_registrator, _registrator.value);
  auto org = coops.find(coopname.value);

  eosio::check(org != coops.end(), "Организация не найдена");
  eosio::check(org -> is_cooperative, "Кооператив не найден");

  eosio::check(initial.symbol == minimum.symbol && minimum.symbol == _root_govern_symbol, "Неверные символы взносов");
  eosio::check(org_initial.symbol == org_minimum.symbol && org_minimum.symbol == _root_govern_symbol, "Неверные символы взносов");

  eosio::check(initial.amount > 0 && org_initial.amount > 0 && minimum.amount > 0 && org_minimum.amount > 0, "Вступительный и минимальный паевые взносы должны быть положительными");

  coops.modify(org, username, [&](auto &o){
    o.initial = initial;
    o.minimum = minimum;
    o.registration = initial + minimum;

    o.org_initial = org_initial;
    o.org_minimum = org_minimum;
    o.org_registration = org_minimum + org_initial;

    o.announce = announce;
    o.description = description;
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
* @note Авторизация требуется от аккаунта: @p changer
*/
[[eosio::action]] void registrator::changekey(eosio::name coopname, eosio::name changer, eosio::name username,
                                              eosio::public_key public_key)
{
  require_auth(changer);

  auto cooperative = get_cooperative_or_fail(coopname);
  check_auth_or_fail(coopname, changer, "changekey"_n);
  
  accounts_index accounts(_registrator, _registrator.value);

  auto participant = get_participant_or_fail(coopname, username);
  eosio::check(participant.status == "accepted"_n, "Пайщик не активен в кооперативе");

  auto coop_account = accounts.find(username.value);
  eosio::check(coop_account != accounts.end(), "Аккаунт кооператива не найден");

  eosio::check(coop_account -> status == "active"_n, "Кооператив не активен и не может изменять ключи доступа");
  
  authority active_auth;
  active_auth.threshold = 1;
  key_weight keypermission{public_key, 1};
  active_auth.keys.emplace_back(keypermission);

  // Change active authority of card to a new key
  eosio::action(eosio::permission_level(_registrator, eosio::name("active")),
                eosio::name("eosio"), eosio::name("changekey"),
                std::tuple(username, eosio::name("active"),
                           eosio::name("owner"), active_auth))
      .send();

}

/**
\ingroup public_actions
\brief Подтверждение регистрации члена кооператива
*
* Этот метод позволяет подтвердить регистрацию пользователя (физического или юридического лица) в качестве члена кооператива.
* Подтверждение может быть осуществлено только аккаунтом контракта совета кооператива после принятия соответствующего решения.
*
* @param coopname Имя кооператива
* @param username Имя члена кооператива
* @param position_title Название должности
* @param position Код должности (например, chairman, director и др.)
*
* @note Авторизация требуется от аккаунта: @p _soviet
*/
[[eosio::action]] void registrator::confirmreg(eosio::name coopname, eosio::name username)
{
  require_auth(_soviet);
  require_recipient(username);

  accounts_index accounts(_registrator, _registrator.value);
  auto account = accounts.find(username.value);

  eosio::check(account != accounts.end(), "Аккаунт не найден в картотеке");

  accounts.modify(account, _soviet, [&](auto &g){ 
    g.status = "active"_n; 
  });
}
