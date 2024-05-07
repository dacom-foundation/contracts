#!/bin/zsh

# Определение ассоциативных массива для zsh
declare -A contract_params_test=(
  [system]="eosio"
  # [msig]="eosio.msig"
  # [wrap]="eosio.wrap"
  # [token]="eosio.token"
  [ano]="anotest22222"
  [gateway]="gatetest2222"
  [draft]="drafttest222"
  [marketplace]="markettest22"
  [soviet]="soviettest22"
  [registrator]="regtest22222"
  [fund]="fundtest2222"
)

# Алиас для cleos через Docker
alias cleos='docker exec -it node /usr/local/bin/cleos'

rm -rf types/*

# Генерация TypeScript типов из ABI для каждого контракта
for contract in "${(@k)contract_params_test}"; do
  abi_contract="${contract_params_test[$contract]}"
  cleos get abi "$abi_contract" | eosio-abi2ts -e "types/${contract}.ts"  -p I -n 2 
done

cp -r types/* ../cooptypes/src/interfaces
