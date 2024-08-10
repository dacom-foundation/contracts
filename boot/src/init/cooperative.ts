import axios from 'axios'
import { describe, expect, it } from 'vitest'
import type { Account, Contract, Keys } from '../types'
import config from '../configs'
import Blockchain from '../blockchain'
import { sleep } from '../utils'

const test_hash = '157192b276da23cc84ab078fc8755c051c5f0430bf4802e55718221e6b76c777'
const test_sign = 'SIG_K1_KmKWPBC8dZGGDGhbKEoZEzPr3h5crRrR2uLdGRF5DJbeibY1MY1bZ9sPwHsgmPfiGFv9psfoCVsXFh9TekcLuvaeuxRKA8'
const test_pkey = 'EOS5JhMfxbsNebajHcTEK8yC9uNN9Dit9hEmzE8ri8yMhhzxrLg3J'
const test_meta = JSON.stringify({})

const document = {
  hash: test_hash,
  signature: test_sign,
  public_key: test_pkey,
  meta: test_meta,
}

class Cooperative {
  public blockchain: Blockchain

  constructor(blockchain: Blockchain) {
    this.blockchain = blockchain
  }

  async createCooperative(username?: string, keys?: Keys) {
    const account = await this.blockchain.generateKeypair(username, keys, 'Аккаунт кооператива')
    console.log('Регистрируем аккаунт')
    await this.blockchain.registerAccount2({
      registrator: config.provider,
      coopname: config.provider,
      referer: '',
      username: account.username,
      public_key: account.publicKey,
      signature_hash: '',
      meta: '',
    })

    console.log('Переводим аккаунт в организации')

    await this.blockchain.registerOrganization({
      registrator: config.provider_chairman,
      coopname: config.provider,
      username: account.username,
      params: {
        is_cooperative: true,
        coop_type: 'conscoop',
        announce: 'Тестовый кооператив',
        description: 'Тестовое описание',
        initial: `500.0000 ${config.token.govern_symbol}`,
        minimum: `500.0000 ${config.token.govern_symbol}`,
        org_initial: `1000.0000 ${config.token.govern_symbol}`,
        org_minimum: `1000.0000 ${config.token.govern_symbol}`,
      },
    })

    console.log('Отправляем заявление на вступление')

    await this.blockchain.joinCoop({
      registrator: config.provider_chairman,
      coopname: config.provider,
      username: account.username,
      document,
    })

    console.log('Голосуем по решению в провайдере')

    await this.blockchain.votefor({
      coopname: config.provider,
      member: config.provider_chairman,
      decision_id: 1,
    })

    console.log('Утверждаем решение в провайдере')

    await this.blockchain.authorize({
      coopname: config.provider,
      chairman: config.provider_chairman,
      decision_id: 1,
      document,
    })

    console.log('Исполняем решение в провайдере')

    await this.blockchain.exec({
      executer: config.provider_chairman,
      coopname: config.provider,
      decision_id: 1,
    })
  }
}

export async function startCoop() {
  // инициализируем инстанс с ключами
  const blockchain = new Blockchain(config.network, config.private_keys)
  const cooperative = new Cooperative(blockchain)

  await blockchain.powerup({
    payer: 'eosio',
    receiver: config.provider,
    days: config.powerup.days,
    payment: `100.0000 ${config.token.symbol}`,
    transfer: true,
  })

  await blockchain.transfer({
    from: 'eosio',
    to: config.provider,
    quantity: `100.0000 ${config.token.symbol}`,
    memo: '',
  })

  await cooperative.createCooperative()

  // арендуем ресурсы провайдеру
  // регистрируем провайдера как кооператив

  // арендуем ресурсы аккаунтам

  // другим скриптом:
  // - регистрируем все прочие аккаунты
  // - развернуть провайдера
  // - подключить кооператив
  // - выдаём кодовые разрешения

  // ______
  // другим скриптом:
  // - развернуть маркетплейс
}
