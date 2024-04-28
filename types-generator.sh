#!/bin/zsh

# Определение ассоциативных массива для zsh
declare -A contract_params_test=(
  [ano]="anotest22222"
  [gateway]="gatetest2222"
  [draft]="drafttest222"
  [marketplace]="markettest22"
  [soviet]="soviettest22"
  [registrator]="regtest22222"
  [system]="eosio"
  [fund]="fundtest2222"
)

# Алиас для cleos через Docker
alias cleos='docker exec -it node /usr/local/bin/cleos'

# Генерация TypeScript типов из ABI для каждого контракта
for contract in "${(@k)contract_params_test}"; do
  rm types/${contract}.ts
  abi_contract="${contract_params_test[$contract]}"
  cleos get abi "$abi_contract" | eosio-abi2ts -e "types/${contract}.ts" -n 2
done

