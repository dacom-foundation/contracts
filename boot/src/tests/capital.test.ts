import { randomBytes } from 'node:crypto'
import { afterAll, beforeAll, describe, expect, it } from 'vitest'
import { execa } from 'execa'
import { CapitalContract, SovietContract, TokenContract } from 'cooptypes'
import { sha256 } from 'eosjs/dist/eosjs-key-conversions'
import Blockchain from '../blockchain'
import config from '../configs'
import { getTotalRamUsage, globalRamStats } from '../utils/getTotalRamUsage'
import { generateRandomSHA256 } from '../utils/randomHash'
import { createParticipant } from '../init/participant'
import { generateRandomUsername } from '../utils/randomUsername'

// const CLI_PATH = 'src/index.ts'

const blockchain = new Blockchain(config.network, config.private_keys)
let idea1: CapitalContract.Actions.CreateIdea.ICreateIdea
let _idea2: CapitalContract.Actions.CreateIdea.ICreateIdea
let _idea3: CapitalContract.Actions.CreateIdea.ICreateIdea

let tester1: string
let tester2: string
let tester3: string

let result1: CapitalContract.Actions.CreateResult.ICreateResult
let _result2: CapitalContract.Actions.CreateResult.ICreateResult
let _result3: CapitalContract.Actions.CreateResult.ICreateResult

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

  tester2 = generateRandomUsername()
  console.log('tester2: ', tester2)
  await createParticipant(tester2)

  tester3 = generateRandomUsername()
  console.log('tester3: ', tester3)
  await createParticipant(tester3)

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

describe('тест контракта CAPITAL', () => {
  it('создаём целевую потребительскую программу', async () => {
    const data: SovietContract.Actions.Programs.CreateProgram.ICreateProgram = {
      coopname: 'voskhod',
      username: 'ant',
      draft_id: '1', // TODO заменить на шаблон ЦПП
      title: 'КАПИТАЛИЗАЦИЯ',
      announce: '',
      description: '',
      preview: '',
      images: '',
      calculation_type: 'free',
      fixed_membership_contribution: '0.0000 RUB',
      membership_percent_fee: '0',
      meta: '',
    }

    const result = await blockchain.api.transact(
      {
        actions: [
          {
            account: SovietContract.contractName.production,
            name: SovietContract.Actions.Programs.CreateProgram.actionName,
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
  })

  it('инициализируем контракт CAPITAL', async () => {
    const data: CapitalContract.Actions.Init.IInit = {
      coopname: 'voskhod',
      initiator: 'voskhod',
      program_id: 2,
    }

    const state = (await blockchain.getTableRows(
      CapitalContract.contractName.production,
      CapitalContract.contractName.production,
      'state',
      1,
      'voskhod',
      'voskhod',
    ))[0]

    if (state) {
      expect(state.coopname).toBe('voskhod')
    }
    else {
      const result = await blockchain.api.transact(
        {
          actions: [
            {
              account: CapitalContract.contractName.production,
              name: CapitalContract.Actions.Init.actionName,
              authorization: [
                {
                  actor: 'voskhod',
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
    }
  })

  it('создаём идею', async () => {
    const hash = generateRandomSHA256()

    const data: CapitalContract.Actions.CreateIdea.ICreateIdea = {
      coopname: 'voskhod',
      administrator: 'voskhod',
      title: `Идея ${hash.slice(0, 10)}`,
      description: `Описание ${hash.slice(0, 10)}`,
      hash,
    }

    idea1 = data

    const result = await blockchain.api.transact(
      {
        actions: [
          {
            account: CapitalContract.contractName.production,
            name: CapitalContract.Actions.CreateIdea.actionName,
            authorization: [
              {
                actor: 'voskhod',
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

    expect(result.transaction_id).toBeDefined()

    const idea = (await blockchain.getTableRows(
      CapitalContract.contractName.production,
      'voskhod',
      'ideas',
      1,
      idea1.hash,
      idea1.hash,
      3,
      'sha256',
    ))[0]

    expect(idea).toBeDefined()

    expect(idea.status).toBe('created')
    expect(idea.target).toBe('0.0000 RUB')
    expect(idea.contributed).toBe('0.0000 RUB')
    expect(idea.authors_count).toBe(0)
    expect(idea.authors_shares).toBe(0)
    expect(idea.hash).toBe(data.hash)
    expect(idea.title).toBe(data.title)
    expect(idea.description).toBe(data.description)
    getTotalRamUsage(result)
  })

  it('добавляем автора к идее', async () => {
    const data: CapitalContract.Actions.AddAuthor.IAddAuthor = {
      coopname: 'voskhod',
      administrator: 'voskhod',
      idea_hash: idea1.hash,
      author: 'ant',
      shares: '100',
    }

    const result = await blockchain.api.transact(
      {
        actions: [
          {
            account: CapitalContract.contractName.production,
            name: CapitalContract.Actions.AddAuthor.actionName,
            authorization: [
              {
                actor: 'voskhod',
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

    const author = (await blockchain.getTableRows(
      CapitalContract.contractName.production,
      'voskhod',
      'authors',
      1,
      idea1.hash,
      idea1.hash,
      3,
      'sha256',
    ))[0]

    expect(author).toBeDefined()
    expect(author.username).toBe('ant')
    expect(author.shares).toBe(100)
    expect(author.idea_hash).toBe(idea1.hash)
    expect(author.added_at).toBeDefined()

    const idea = (await blockchain.getTableRows(
      CapitalContract.contractName.production,
      'voskhod',
      'ideas',
      1,
      idea1.hash,
      idea1.hash,
      3,
      'sha256',
    ))[0]

    expect(idea).toBeDefined()
    expect(idea.authors_count).toBe(1)
    expect(idea.authors_shares).toBe(100)
  })

  it('создать результат', async () => {
    const data: CapitalContract.Actions.CreateResult.ICreateResult = {
      coopname: 'voskhod',
      administrator: 'voskhod',
      idea_hash: idea1.hash,
      result_hash: generateRandomSHA256(),
    }

    result1 = data

    const result = await blockchain.api.transact(
      {
        actions: [
          {
            account: CapitalContract.contractName.production,
            name: CapitalContract.Actions.CreateResult.actionName,
            authorization: [
              {
                actor: 'voskhod',
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

    const blockchainResult = (await blockchain.getTableRows(
      CapitalContract.contractName.production,
      'voskhod',
      'results',
      1,
      result1.result_hash,
      result1.result_hash,
      2,
      'sha256',
    ))[0]

    expect(blockchainResult).toBeDefined()
    expect(blockchainResult.hash).toBe(result1.result_hash)
    expect(blockchainResult.idea_hash).toBe(idea1.hash)
    expect(blockchainResult.coopname).toBe('voskhod')
  })

  it('добавить создателя tester1', async () => {
    const data: CapitalContract.Actions.AddCreator.IAddCreator = {
      coopname: 'voskhod',
      administrator: 'voskhod',
      result_hash: result1.result_hash,
      creator: tester1,
      used: '100.0000 RUB',
    }

    const result = await blockchain.api.transact(
      {
        actions: [
          {
            account: CapitalContract.contractName.production,
            name: CapitalContract.Actions.AddCreator.actionName,
            authorization: [
              {
                actor: 'voskhod',
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

    const blockchainResult = (await blockchain.getTableRows(
      CapitalContract.contractName.production,
      'voskhod',
      'results',
      1,
      result1.result_hash,
      result1.result_hash,
      2,
      'sha256',
    ))[0]

    expect(blockchainResult).toBeDefined()
    expect(blockchainResult.hash).toBe(result1.result_hash)
    expect(blockchainResult.status).toBe('created')
    expect(blockchainResult.creators_count).toBe(1)
    expect(blockchainResult.creators_amount).toBe('100.0000 RUB')
    expect(blockchainResult.creators_bonus).toBe('38.2000 RUB')
    expect(blockchainResult.authors_bonus).toBe('161.8000 RUB')
    expect(blockchainResult.generated_amount).toBe('300.0000 RUB')
    expect(blockchainResult.participants_bonus).toBe('485.4000 RUB')
    expect(blockchainResult.total_amount).toBe('785.4000 RUB')
  })

  it('добавляем второго автора tester2', async () => {
    const data: CapitalContract.Actions.AddAuthor.IAddAuthor = {
      coopname: 'voskhod',
      administrator: 'voskhod',
      idea_hash: idea1.hash,
      author: tester2,
      shares: '100',
    }

    const result = await blockchain.api.transact(
      {
        actions: [
          {
            account: CapitalContract.contractName.production,
            name: CapitalContract.Actions.AddAuthor.actionName,
            authorization: [
              {
                actor: 'voskhod',
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

    const idea = (await blockchain.getTableRows(
      CapitalContract.contractName.production,
      'voskhod',
      'ideas',
      1,
      idea1.hash,
      idea1.hash,
      3,
      'sha256',
    ))[0]

    expect(idea).toBeDefined()
    expect(idea.authors_count).toBe(2)
    expect(idea.authors_shares).toBe(200)
  })

  it('добавляем создателей tester2 и tester3', async () => {
    // Добавляем второго создателя
    let data: CapitalContract.Actions.AddCreator.IAddCreator = {
      coopname: 'voskhod',
      administrator: 'voskhod',
      result_hash: result1.result_hash,
      creator: tester2,
      used: '100.0000 RUB',
    }

    let result = await blockchain.api.transact(
      {
        actions: [
          {
            account: CapitalContract.contractName.production,
            name: CapitalContract.Actions.AddCreator.actionName,
            authorization: [
              {
                actor: 'voskhod',
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

    // Добавляем третьего создателя
    data = {
      coopname: 'voskhod',
      administrator: 'voskhod',
      result_hash: result1.result_hash,
      creator: tester3,
      used: '100.0000 RUB',
    }

    result = await blockchain.api.transact(
      {
        actions: [
          {
            account: CapitalContract.contractName.production,
            name: CapitalContract.Actions.AddCreator.actionName,
            authorization: [
              {
                actor: 'voskhod',
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

    // Проверяем финальные значения
    const blockchainResult = (await blockchain.getTableRows(
      CapitalContract.contractName.production,
      'voskhod',
      'results',
      1,
      result1.result_hash,
      result1.result_hash,
      2,
      'sha256',
    ))[0]

    expect(blockchainResult).toBeDefined()
    expect(blockchainResult.creators_count).toBe(3)
    expect(blockchainResult.creators_amount).toBe('300.0000 RUB')
    // Ниже – пример ожидаемого роста, исходя из логики контракта
    expect(blockchainResult.creators_bonus).toBe('114.6000 RUB')
    expect(blockchainResult.authors_bonus).toBe('485.4000 RUB')
    expect(blockchainResult.generated_amount).toBe('900.0000 RUB')
    expect(blockchainResult.participants_bonus).toBe('1456.2000 RUB')
    expect(blockchainResult.total_amount).toBe('2356.2000 RUB')
  })

  it('удаляем создателя tester3', async () => {
    const data: CapitalContract.Actions.DelCreator.IDelCreator = {
      coopname: 'voskhod',
      administrator: 'voskhod',
      result_hash: result1.result_hash,
      creator: tester3,
    }

    const result = await blockchain.api.transact(
      {
        actions: [
          {
            account: CapitalContract.contractName.production,
            name: CapitalContract.Actions.DelCreator.actionName,
            authorization: [
              {
                actor: 'voskhod',
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

    // Проверяем обновленные значения после удаления
    const blockchainResult = (await blockchain.getTableRows(
      CapitalContract.contractName.production,
      'voskhod',
      'results',
      1,
      result1.result_hash,
      result1.result_hash,
      2,
      'sha256',
    ))[0]

    expect(blockchainResult).toBeDefined()
    expect(blockchainResult.creators_count).toBe(2)
    expect(blockchainResult.creators_amount).toBe('200.0000 RUB')
    // Уменьшаем все показатели ровно на величину последнего добавленного создателя
    expect(blockchainResult.creators_bonus).toBe('76.4000 RUB') // 114.6000 - 38.2000
    expect(blockchainResult.authors_bonus).toBe('323.6000 RUB') // 485.4000 - 161.8000
    expect(blockchainResult.generated_amount).toBe('600.0000 RUB') // 900.0000 - 300.0000
    expect(blockchainResult.participants_bonus).toBe('970.8000 RUB') // 1456.2000 - 485.4000
    expect(blockchainResult.total_amount).toBe('1570.8000 RUB') // 2356.2000 - 785.4000
  })

  it('стартуем распределение долей результата', async () => {
    const data: CapitalContract.Actions.Start.IStart = {
      coopname: 'voskhod',
      administrator: 'voskhod',
      result_hash: result1.result_hash,
    }

    const result = await blockchain.api.transact(
      {
        actions: [
          {
            account: CapitalContract.contractName.production,
            name: CapitalContract.Actions.Start.actionName,
            authorization: [
              {
                actor: 'voskhod',
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

    const blockchainResult = (await blockchain.getTableRows(
      CapitalContract.contractName.production,
      'voskhod',
      'results',
      1,
      result1.result_hash,
      result1.result_hash,
      2,
      'sha256',
    ))[0]

    expect(blockchainResult).toBeDefined()
    expect(blockchainResult.status).toBe('started')
  })

  it('пишем заявления и получаем NFT', async () => {
    const data: CapitalContract.Actions.SetStatement.ISetStatement = {
      coopname: 'voskhod',
      administrator: 'voskhod',
      statement: fakeDocument,
      nft_hash: generateRandomSHA256(),
    }

    const result = await blockchain.api.transact(
      {
        actions: [
          {
            account: CapitalContract.contractName.production,
            name: CapitalContract.Actions.SetStatement.actionName,
            authorization: [
              {
                actor: 'voskhod',
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
  })
})
