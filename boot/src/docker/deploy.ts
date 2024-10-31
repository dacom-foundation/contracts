/* eslint-disable node/prefer-global/process */
import Blockchain from '../blockchain'
import contracts from '../configs/contracts'
import type { INetwork } from '../configs/networks'
import { networks } from '../configs/networks'
import type { Contract } from '../types'
import { execCommand } from './exec'

export async function deployCommand(name: string, pre_target: string, pre_network: string): Promise<void> {
  const network: INetwork | undefined = networks.find(el => el.name === pre_network)

  if (!network)
    throw new Error('Сеть не найдена')

  const contract = contracts.find(el => el.name === name)

  if (!contract)
    throw new Error('Контракт не найден')

  let target = contract.target

  if (pre_target)
    target = pre_target

  const contract_for_deploy: Contract = {
    path: contract.path,
    name,
    target,
  }

  const blockchain = new Blockchain(network, [process.env.EOSIO_PRV_KEY as string])
  await blockchain.update_pass_instance()
  await blockchain.setContract(contract_for_deploy)
}
