#!/bin/bash

# Импорт параметров сетей
source ./networks.sh

# Устанавливаем значения по умолчанию
network_param=$local
account_name_param=""
network=""

# Получаем первый аргумент, который должен быть именем контракта
contract_name=$1

rm /Users/darksun/testnet/wallet/keosd.sock
./kill.sh

# Проверяем наличие параметра -n и устанавливаем соответствующую сеть
while [[ $# -gt 0 ]]; do
    case $1 in
        -n)
        network=$2
        shift
        shift
        ;;
        -t)
        account_name_param=$2
        shift
        shift
        ;;
        *)
        shift
        ;;
    esac
done

# Устанавливаем значение сети из networks.sh
case $network in
    prod)
    network_param=$prod
    ;;
    test)
    network_param=$test
    ;;
    local)
    network_param=$local
    ;;
esac

# Разблокировка кошелька
./unlock.sh

# Функция для установки контракта
deploy_contract() {
    local contract=$1
    local dir=$2
    local account_name=$3
    echo "Устанавливаем контракт $contract на аккаунт $account_name..."
    if [ -z "$account_name_param" ]; then
        echo ./cleos.sh $network_param set contract $account_name $dir -p $account_name
        ./cleos.sh $network_param set contract $account_name $dir -p $account_name
    else
        echo ./cleos.sh $network_param set contract $account_name_param $dir -p $account_name_param
        ./cleos.sh $network_param set contract $account_name_param $dir -p $account_name_param
    fi
}

# Если контракт указан, деплоим только его
if [ ! -z "$contract_name" ]; then
    ls ./
    echo "Проверка наличия папки для контракта $contract_name..."
    if [ -d "./$contract_name" ]; then
        echo "Папка контракта найдена. Содержимое папки:"
        deploy_contract $contract_name "/contracts/$contract_name" $contract_name
    elif [[ "$contract_name" == "eosio.msig" || "$contract_name" == "eosio.token" || "$contract_name" == "eosio.wrap" ]]; then
        echo "Папка системного контракта найдена. Содержимое папки:"
        ls "/contracts/system/build/contracts/$contract_name"
        deploy_contract $contract_name "/contracts/system/build/contracts/$contract_name" $contract_name
    elif [ "$contract_name" == "eosio.system" ]; then
        echo "Папка системного контракта найдена. Содержимое папки:"
        ls "/contracts/system/build/contracts/$contract_name"
        deploy_contract $contract_name "/contracts/system/build/contracts/$contract_name" "eosio"
    elif [ "$contract_name" == "eosio.boot" ]; then
        echo "Папка системного контракта найдена. Содержимое папки:"
        ls "/contracts/system/build/contracts/$contract_name"
        deploy_contract $contract_name "/contracts/system/build/contracts/$contract_name" "eosio"
    else
        echo "Контракт $contract_name не найден"
    fi
    exit 0
fi

# Перебираем все папки с контрактами на верхнем уровне, исключая 'system'
for dir in $(find /contracts/* -maxdepth 0 -type d ! -name 'system'); do
    contract=$(basename $dir)
    echo "Найдена папка $dir для контракта $contract. Содержимое папки:"
    ls $dir
    deploy_contract $contract $dir $contract
done

# Исключение для контрактов в папке system
special_system_contracts=("eosio.msig" "eosio.token" "eosio.wrap")
for contract in "${special_system_contracts[@]}"; do
    echo "Найдена папка системного контракта для $contract. Содержимое папки:"
    ls "/contracts/system/build/contracts/$contract"
    deploy_contract $contract "/contracts/system/build/contracts/$contract" $contract
done

deploy_contract "eosio.system" "/contracts/system/build/contracts/eosio.system" "eosio"
