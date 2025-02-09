import { randomInt } from 'node:crypto'
import { afterAll, beforeAll, describe, expect, it } from 'vitest'
import { FundContract, GatewayContract, SovietContract } from 'cooptypes'
import Blockchain from '../blockchain'
import config from '../configs'
import { getTotalRamUsage, globalRamStats } from '../utils/getTotalRamUsage'
import { createParticipant } from '../init/participant'
import { generateRandomUsername } from '../utils/randomUsername'

// const CLI_PATH = 'src/index.ts'

const blockchain = new Blockchain(config.network, config.private_keys)
let tester1: string
let deposit_id_1: number
const walletProgramStates1: any[] = []

const fakeDocument = {
  hash: '157192B276DA23CC84AB078FC8755C051C5F0430BF4802E55718221E6B76C777',
  public_key: 'PUB_K1_5JhMfxbsNebajHcTEK8yC9uNN9Dit9hEmzE8ri8yMhhzzEtUA4',
  signature: 'SIG_K1_KmKWPBC8dZGGDGhbKEoZEzPr3h5crRrR2uLdGRF5DJbeibY1MY1bZ9sPwHsgmPfiGFv9psfoCVsXFh9TekcLuvaeuxRKA8',
  meta: '{}',
}

beforeAll(async () => {
  await blockchain.update_pass_instance()

  tester1 = generateRandomUsername()
  console.log('tester1: ', tester1)
  await createParticipant(tester1)

  // const { stdout } = await execa('esno', [CLI_PATH, 'boot'], { stdio: 'inherit' })
  // expect(stdout).toContain('Boot process completed')
}, 500_000)

afterAll(() => {
  console.log('\n📊 **RAM USAGE SUMMARY** 📊')
  let totalGlobalRam = 0

  for (const [key, ramUsed] of Object.entries(globalRamStats)) {
    const ramKb = (ramUsed / 1024).toFixed(2)
    console.log(`  ${key} = ${ramKb} kb`)
    totalGlobalRam += ramUsed
  }

  console.log(`\n💾 **TOTAL RAM USED IN TESTS**: ${(totalGlobalRam / 1024).toFixed(2)} kb\n`)
})

async function getUserProgramWallet(coopname: string, username: string, program_id: number) {
  const wallets = (await blockchain.getTableRows(
    SovietContract.contractName.production,
    coopname,
    'progwallets',
    1,
    username,
    username,
    2,
  ))

  const wallet = wallets.find((el: any) => el.program_id === program_id)

  return wallet
}

async function getCoopProgramWallet(coopname: string, program_id: number) {
  const program = (await blockchain.getTableRows(
    SovietContract.contractName.production,
    coopname,
    'programs',
    1,
    program_id.toString(),
    program_id.toString(),
  ))[0]

  return program
}

async function getCoopWallet(coopname: string) {
  const wallet = (await blockchain.getTableRows(
    FundContract.contractName.production,
    coopname,
    'coopwallet',
    1,
  ))[0]

  return wallet
}

async function getDeposit(coopname: string, deposit_id: number) {
  const deposit = (await blockchain.getTableRows(
    GatewayContract.contractName.production,
    coopname,
    'deposits',
    1,
    deposit_id.toString(),
    deposit_id.toString(),
  ))[0]

  return deposit
}

function compareTokenAmounts(prevAmount: string, currentAmount: string, expectedIncrease: number): void {
  const prev = parseFloat(prevAmount.split(' ')[0])
  const current = parseFloat(currentAmount.split(' ')[0])
  const expected = prev + expectedIncrease

  expect(current).toBe(expected)
}

describe('тест Wallet в Soviet', () => {
  it('подписываем соглашение ЦПП кошелька', async () => {
    const data: SovietContract.Actions.Agreements.SendAgreement.ISendAgreement = {
      coopname: 'voskhod',
      administrator: 'voskhod',
      username: 'ant',
      agreement_type: 'wallet',
      document: fakeDocument,
    }

    const result = await blockchain.api.transact(
      {
        actions: [
          {
            account: SovietContract.contractName.production,
            name: SovietContract.Actions.Agreements.SendAgreement.actionName,
            authorization: [
              {
                actor: 'ant',
                permission: 'active',
              },
            ],
            data,
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      },
    )
    getTotalRamUsage(result)
    expect(result.transaction_id).toBeDefined()
    const wallet = await getUserProgramWallet('voskhod', 'ant', 1)
    expect(wallet).toEqual(expect.objectContaining({
      coopname: 'voskhod',
      username: 'ant',
      available: expect.any(String),
      blocked: expect.any(String),
      program_id: 1,
      membership_contribution: expect.any(String),
    }))
    const program = await getCoopProgramWallet('voskhod', 1)

    walletProgramStates1.push(program)

    console.log(walletProgramStates1)
  })

  it('совершаем депозит в ЦПП кошелька', async () => {
    deposit_id_1 = randomInt(100000)

    const data: GatewayContract.Actions.CreateDeposit.ICreateDeposit = {
      coopname: 'voskhod',
      username: 'ant',
      deposit_id: deposit_id_1,
      type: 'deposit',
      quantity: '100.0000 RUB',
    }

    const result = await blockchain.api.transact(
      {
        actions: [
          {
            account: GatewayContract.contractName.production,
            name: GatewayContract.Actions.CreateDeposit.actionName,
            authorization: [
              {
                actor: 'ant',
                permission: 'active',
              },
            ],
            data,
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      },
    )
    getTotalRamUsage(result)
    expect(result.transaction_id).toBeDefined()

    const deposit = await getDeposit('voskhod', deposit_id_1)

    expect(deposit).toEqual(expect.objectContaining({
      id: deposit_id_1,
      username: 'ant',
      coopname: 'voskhod',
      type: 'deposit',
      quantity: '100.0000 RUB',
      status: 'pending',
    }))

    const prevUserWallet = await getUserProgramWallet('voskhod', 'ant', 1)
    const prevUserWalletAvailable = prevUserWallet?.available || '0.0000 RUB'

    const prevCoopWallet = await getCoopWallet('voskhod')
    const prevCoopWalletAvailable = prevCoopWallet?.circulating_account?.available || '0.0000 RUB'
    console.log('prevcoopWallet', prevCoopWallet)

    const data2: GatewayContract.Actions.CompleteDeposit.ICompleteDeposit = {
      coopname: 'voskhod',
      admin: 'voskhod',
      deposit_id: deposit_id_1,
      memo: '',
    }

    const result2 = await blockchain.api.transact(
      {
        actions: [
          {
            account: GatewayContract.contractName.production,
            name: GatewayContract.Actions.CompleteDeposit.actionName,
            authorization: [
              {
                actor: 'voskhod',
                permission: 'active',
              },
            ],
            data: data2,
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      },
    )

    getTotalRamUsage(result2)
    const deposit2 = await getDeposit('voskhod', deposit_id_1)
    expect(deposit2.status).equal('completed')

    const program = await getCoopProgramWallet('voskhod', 1)

    walletProgramStates1.push(program)

    const userWallet = await getUserProgramWallet('voskhod', 'ant', 1)
    const prevProgramState = walletProgramStates1[walletProgramStates1.length - 2]

    // Проверяем изменение баланса программы
    compareTokenAmounts(prevProgramState.available, program.available, 100.0000)
    compareTokenAmounts(prevProgramState.share_contributions, program.share_contributions, 100.0000)

    // Проверяем изменение баланса пользователя
    compareTokenAmounts(prevUserWalletAvailable, userWallet.available, 100.0000)

    // Проверяем изменение баланса кооператива
    const currentCoopWallet = await getCoopWallet('voskhod')

    compareTokenAmounts(prevCoopWalletAvailable, currentCoopWallet.circulating_account.available, 100.0000)
  }, 20_000)
})
