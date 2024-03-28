#!/bin/zsh

# Определение ассоциативных массива для zsh
declare -A contract_params_test=(
  [eosio.token]="eosio.token"
  [gateway]="gatetest1"
  [draft]="drafttest1"
  [marketplace]="markettest1"
  [soviet]="soviettest1"
  [registrator]="regtest1"
  [system]="eosio"
  [fund]="fundtest1"
)

# Алиас для cleos через Docker
alias cleos='docker exec -it node /usr/bin/cleos'

# Очистка папки types/
rm -rf types/*

# Генерация TypeScript типов из ABI для каждого контракта
for contract in "${(@k)contract_params_test}"; do
  abi_contract="${contract_params_test[$contract]}"
  cleos get abi "$abi_contract" | eosio-abi2ts -e "types/${contract}.ts" -n 2
done

