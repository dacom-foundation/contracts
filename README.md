# Введение.

Репозиторий содержит в себе смарт-контракты операционной системы coopOS, образующих протокол COOPENOMICS.

## Окружение
В качестве окружения используется контейнер операционной системы coopOS. Для запуска выполните команду из корня репозитория:

```
pnpm run enter
```

После выполнения команды и входа в контейнер, выполняйте команды компиляции внутри него. 

## Компиляция
Для компиляции выполните команду:

```
mkdir build
cd build
cmake -DBUILD_TARGET= ..
make

```
Компиляция всех контрактов произойдёт в папку build/contracts. Для указания конкретного контракта, который необходимо скомпилировать, используйте названия папок из директории cpp в качестве параметра -DBUILD_TARGET. Например, для контракта fund:

```
cmake -DBUILD_TARGET=fund ..
make
```

Для компиляции всех контрактов передавайте пустой параметр -DBUILD_TARGET= . Без явной передачи параметра используйте кэшированный, ранее переданный параметр, что может привести к путанице. Поэтому, всегда явно передавайте параметр цели компиляции.


## Загрузка
Перед автоматической загрузкой контрактов в локальную версию блокчейна - скомпилируйте их как показано выше. После чего, выйдите из контейнера и выполните команду установки загрузчика в вашем окружении:

```
pnpm install
```

Скопируйте файл .env-example в .env:

```
cp .env-example .env
```

И запустите загрузку:

```
pnpm run cli boot
```

Команда загрузки запустит блокчейн и произведет установку всех контрактов для начала работы. Параметры конфигурации блокчейна можно найти в файле scripts/restart.sh

Блокчейн будет запущен в контейнере и остановит свою работу при завершении работы программы. Данные сохраняются в ./blockchain-data корневой директории репозитория. Данные кошелька пишутся в ./wallet-data. 

После остановки контейнера вы можете перезапустить его командной:

```
pnpm run cli start

```

Для перезагрузки блокчейна вновь вызовите:

```
pnpm run boot
```

Последняя команда очистит всю историю цепочки блоков и перезагрузит локальный блокчейн до начального состояния. 

## Кошелёк
Для доступа командному кошельку cleos используйте команду 

```
pnpm run cli cleos 

```

Для отображения полного списка команд воспользуйтесь:
```
pnpm run cli cleos --help
```

Командный кошелёк позволяет совершать любые транзакции и просматривать любые таблицы в блокчейне. Для более удобной постоянной работы занесите алиас в файл ~/.bashrc для быстрого вызова:

```
alias=cd <REPLACE_TO_YOUR_LOCAL_REPOSITORY_DIR/boot> && pnpm run cli cleos
```

Подтяните изменения профиля:
```
source ~/.bashrc
```

Теперь утилита cleos доступна простой командной:

```
cleos get info
```


## Тесты

### Полные тесты
Тесты по-умолчанию НЕ компилируются вместе с контрактами. Для компиляции необходимо передать флаг конфигурации сборки -DBUILD_TESTS=ON:
```
cmake -DBUILD_TARGET= -DBUILD_TESTS=ON
make
```
Все контракты пересоберутся вместе со всеми тестами. 

Для запуска тестов (из директории build):
```
ctest --test-dir contracts/tests --output-on-failure
```

### Выборочные тесты
Для выборочного указания тестового файла используйте из директории build:

```
cmake -DBUILD_TARGET=fund -DBUILD_TESTS=ON -DTEST_TARGET=fund.tester.cpp ..
make
```

Команды пересоберет указанный контракт и его тест. Выборочная сборка и тесты значительно ускоряют отладку. 


### Информативные тесты
Для большей детализации тестового процесса добавьте флаг -DVERBOSE=ON при конфигурации сборки:
```
cmake -DBUILD_TARGET=fund -DTEST_TARGET=fund.tester.cpp -DVERBOSE=ON ..
make
```

Дополнительная вербозность показывает отладочную информацию из тестов, включая консоль контрактов, в которую информация выводится через методы print.


## Документация
Для генерации документации используйте команду Doxygen версии от 1.9.3:
```
git submodule update --init --recursive
doxygen
```

Документация будет собрана в папке docs/html, откройте файл index.html в браузере:
```
open docs/html/index.html
```

## Лицензия
Продукт Потребительского Кооператива "ВОСХОД" распространяется по лицензии BY-NC-SA 4.0. 

Разрешено делиться, копировать и распространять материал на любом носителе и форме, адаптировать, делать ремиксы, видоизменять и создавать новое, опираясь на этот материал. При использовании, Вы должны обеспечить указание авторства, предоставить ссылку, и обозначить изменения, если таковые были сделаны. Если вы перерабатываете, преобразовываете материал или берёте его за основу для производного произведения, вы должны распространять переделанные вами части материала на условиях той же лицензии , в соответствии с которой распространяется оригинал. Запрещено коммерческое использование материала. Использование в коммерческих целях – это использование, в первую очередь направленное на получение коммерческого преимущества или денежного вознаграждения.

Юридический текст лицензии: https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode.ru
