#!/bin/zsh

# Определение ассоциативных массивов
typeset -A contract_params_test contract_params_prod
contract_params_test=(
  [ano]="anotest1"
  [gateway]="gatetest1"
  [draft]="drafttest1"
  [marketplace]="markettest1"
  [soviet]="soviettest1"
  [registrator]="regtest1"
  [system]="eosio"
)
contract_params_prod=(
  [ano]="ano"
  [gateway]="gateway"
  [draft]="draft"
  [marketplace]="marketplace"
  [soviet]="soviet"
  [registrator]="registrator"
  [system]="eosio"
)

# Выбор массива параметров в зависимости от входящего аргумента
if [ "$2" = "prod" ]; then
    contract_params=("${(@kv)contract_params_prod}")
elif [ "$2" = "test" ]; then
    contract_params=("${(@kv)contract_params_test}")
else
    contract_params=("${(@kv)contract_params_test}")
fi

# Форматирование строки для переменной окружения
contract_params_str=""
for key value in "${(@kv)contract_params}"; do
  contract_params_str+="${key}=${value},"
done

# Проверяем, задан ли аргумент для контрактов
if [ -n "$1" ]; then
  contracts=$1
else
  contracts="*"
fi

# Запуск контейнера Docker
docker run --rm --name cdt_v4.0.0 \
  --volume $(pwd):/project \
  -w /project \
  --env CONTRACT_PARAMS="$contract_params_str" \
  dicoop/cdt_v4.0.0 /bin/bash -c "

# Перебираем все папки в директории
for folder in $contracts/ ; do
  # Убираем слэш из названия папки, чтобы получить имя контракта
  contract=\${folder%/}
  contract=\${contract##*/}

  echo \"Сборка контракта \$contract в \$folder...\"

  # Переходим в директорию контракта
  cd \$folder

  # Извлекаем параметр для -contract из переменной окружения
  contract_param=\$(echo \$CONTRACT_PARAMS | grep -o \"\$contract=[^,]*\" | cut -d'=' -f2)

  # Компилируем с использованием параметра -contract
  cdt-cpp -abigen -I include -R include -contract \$contract_param -o \$contract.wasm \$contract.cpp
  
  # Возвращаемся назад
  cd ..

  echo \"Сборка контракта \$contract завершена!\"
done
"
