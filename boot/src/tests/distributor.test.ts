// contributor.test.ts
import { beforeAll, describe, expect, it } from 'vitest'
import * as dotenv from 'dotenv'

dotenv.config()

describe('contributor Contract Tests', () => {
  let contractAccount: string
  let participant1: string
  let participant2: string

  beforeAll(async () => {
    // Create contract account
    contractAccount = await framework.createRandomAccount()

    // Deploy contract
    await framework.deployContract(
      contractAccount,
      './contributor/contributor.wasm',
      './contributor/contributor.abi',
    )

    // Create participant accounts
    participant1 = await framework.createRandomAccount()
    participant2 = await framework.createRandomAccount()

    // Transfer some EOS to participants
    await framework.transact([
      {
        account: 'eosio.token',
        name: 'transfer',
        authorization: [{ actor: process.env.CONTRACT_DEPLOYER!, permission: 'active' }],
        data: {
          from: process.env.CONTRACT_DEPLOYER!,
          to: participant1,
          quantity: '1000.0000 EOS',
          memo: 'Initial funding',
        },
      },
      {
        account: 'eosio.token',
        name: 'transfer',
        authorization: [{ actor: process.env.CONTRACT_DEPLOYER!, permission: 'active' }],
        data: {
          from: process.env.CONTRACT_DEPLOYER!,
          to: participant2,
          quantity: '1000.0000 EOS',
          memo: 'Initial funding',
        },
      },
    ])
  }, 10000) // Set timeout for beforeAll

  it(
    'participant 1 makes a property contribution',
    async () => {
      // Participant 1 makes a property contribution
      await framework.transact([
        {
          account: contractAccount,
          name: 'addcontrib',
          authorization: [{ actor: participant1, permission: 'active' }],
          data: {
            participant_account: participant1,
            amount: '100.0000 EOS',
            type: 'имущественный',
          },
        },
      ])

      // Fetch participant data
      const participants = await framework.getTableRows(
        contractAccount,
        contractAccount,
        'participants',
      )
      const participantData = participants.find((p: any) => p.account === participant1)

      expect(participantData.share_balance).toBe('100.0000 EOS')
      expect(participantData.property_contributions).toBe('100.0000 EOS')
    },
    10000, // Set timeout for this test
  )

  it(
    'participant 2 makes an intellectual contribution',
    async () => {
      // Participant 2 makes an intellectual contribution
      await framework.transact([
        {
          account: contractAccount,
          name: 'addcontrib',
          authorization: [{ actor: participant2, permission: 'active' }],
          data: {
            participant_account: participant2,
            amount: '50.0000 EOS',
            type: 'интеллектуальный',
          },
        },
      ])

      // Fetch participant data
      const participants = await framework.getTableRows(
        contractAccount,
        contractAccount,
        'participants',
      )
      const participantData = participants.find((p: any) => p.account === participant2)

      expect(typeof participantData.share_balance).toBe('string')
      expect(participantData.intellectual_contributions).toBe('50.0000 EOS')
    },
    10000, // Set timeout for this test
  )

  it(
    'participant 1 withdraws',
    async () => {
      // Participant 1 withdraws 10 EOS
      await framework.transact([
        {
          account: contractAccount,
          name: 'withdraw',
          authorization: [{ actor: participant1, permission: 'active' }],
          data: {
            participant_account: participant1,
            amount: '10.0000 EOS',
          },
        },
      ])

      // Fetch participant data
      const participants = await framework.getTableRows(
        contractAccount,
        contractAccount,
        'participants',
      )
      const participantData = participants.find((p: any) => p.account === participant1)

      expect(participantData.withdrawed).toBe('10.0000 EOS')
      expect(participantData.share_balance).toBe('90.0000 EOS')
    },
    10000, // Set timeout for this test
  )

  it(
    'check global state',
    async () => {
      const globalState = await framework.getTableRows(
        contractAccount,
        contractAccount,
        'globalstate',
      )

      expect(typeof globalState[0].total_shares).toBe('string')
      expect(typeof globalState[0].total_contributions).toBe('string')
    },
    10000, // Set timeout for this test
  )
})
