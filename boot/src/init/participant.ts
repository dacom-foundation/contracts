import axios from 'axios'
import { describe, expect, it } from 'vitest'
import { Registry } from '@coopenomics/factory'
import { RegistratorContract, Cooperative as TCooperative } from 'cooptypes'
import type { Account, Contract, Keys } from '../types'
import config, { GOVERN_SYMBOL, SYMBOL } from '../configs'
import Blockchain from '../blockchain'
import { sendPostToCoopbackWithSecret, sleep } from '../utils'

// const test_hash
//   = '157192b276da23cc84ab078fc8755c051c5f0430bf4802e55718221e6b76c777'
// const test_sign
//   = 'SIG_K1_KmKWPBC8dZGGDGhbKEoZEzPr3h5crRrR2uLdGRF5DJbeibY1MY1bZ9sPwHsgmPfiGFv9psfoCVsXFh9TekcLuvaeuxRKA8'
// const test_pkey = 'EOS5JhMfxbsNebajHcTEK8yC9uNN9Dit9hEmzE8ri8yMhhzxrLg3J'
// const test_meta = JSON.stringify({})

// const document = {
//   hash: test_hash,
//   signature: test_sign,
//   public_key: test_pkey,
//   meta: test_meta,
// }

export class CooperativeClass {
  public blockchain: Blockchain

  constructor(blockchain: Blockchain) {
    this.blockchain = blockchain
  }

  async createParticipant(username: string, keys?: Keys) {
    const account = await this.blockchain.generateKeypair(
      username,
      keys,
      'Аккаунт участника',
    )
    console.log('Регистрируем аккаунт')

    const data: RegistratorContract.Actions.AddUser.IAddUser = {
      registrator: config.provider,
      coopname: config.provider,
      referer: '',
      username: account.username,
      type: 'individual',
      created_at: '2025-02-05T09:34:27',
      initial: '100.0000 RUB',
      minimum: '100.0000 RUB',
      spread_initial: false,
      meta: '',
    }

    await this.blockchain.api.transact({
      actions: [
        {
          account: RegistratorContract.contractName.production,
          name: RegistratorContract.Actions.AddUser.actionName,
          authorization: [{ actor: config.provider, permission: 'active' }],
          data,
        },
      ],
    }, {
      blocksBehind: 3,
      expireSeconds: 30,
    })

    console.log('Регистрируем аккаунт как пользователя')

    const changeKey: RegistratorContract.Actions.ChangeKey.IChangeKey = {
      coopname: config.provider,
      changer: config.provider,
      username: account.username,
      public_key: account.publicKey,
    }

    await this.blockchain.api.transact({
      actions: [
        {
          account: RegistratorContract.contractName.production,
          name: RegistratorContract.Actions.ChangeKey.actionName,
          authorization: [{ actor: config.provider, permission: 'active' }],
          data: changeKey,
        },
      ],
    }, {
      blocksBehind: 3,
      expireSeconds: 30,
    })

    console.log('Отправляем подписанное положение о ЦПП Кошелька оператору')

    await this.blockchain.sendAgreement({
      coopname: config.provider,
      administrator: config.provider,
      username: username!,
      agreement_type: 'wallet',
      document: { // отправляем произвольный документ с валидной подписью
        hash: '157192B276DA23CC84AB078FC8755C051C5F0430BF4802E55718221E6B76C777',
        public_key: 'PUB_K1_5JhMfxbsNebajHcTEK8yC9uNN9Dit9hEmzE8ri8yMhhzzEtUA4',
        signature: 'SIG_K1_KmKWPBC8dZGGDGhbKEoZEzPr3h5crRrR2uLdGRF5DJbeibY1MY1bZ9sPwHsgmPfiGFv9psfoCVsXFh9TekcLuvaeuxRKA8',
        meta: '{}',
      },
    })

    console.log('создаём кошелёк')

    // console.log(`Арендуем ресурсы кооперативу`)

    // await this.blockchain.powerup({
    //   payer: 'eosio',
    //   receiver: username,
    //   days: config.powerup.days,
    //   payment: `100.0000 ${config.token.symbol}`,
    //   transfer: true,
    // })
  }
}

export async function createParticipant(username: string) {
  // инициализируем инстанс с ключами
  const blockchain = new Blockchain(config.network, config.private_keys)
  const cooperative = new CooperativeClass(blockchain)

  await cooperative.createParticipant(username, {
    // eslint-disable-next-line node/prefer-global/process
    privateKey: process.env.EOSIO_PRV_KEY!,
    // eslint-disable-next-line node/prefer-global/process
    publicKey: process.env.EOSIO_PUB_KEY!,
  })
}
