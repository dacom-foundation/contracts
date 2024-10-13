import axios from 'axios'
import { describe, expect, it } from 'vitest'
import { Registry } from 'coopdoc-generator-ts'
import { Cooperative as TCooperative } from 'cooptypes'
import type { Account, Contract, Keys } from '../types'
import config, { GOVERN_SYMBOL, SYMBOL } from '../configs'
import Blockchain from '../blockchain'
import { sendPostToCoopbackWithSecret, sleep } from '../utils'

const test_hash
  = '157192b276da23cc84ab078fc8755c051c5f0430bf4802e55718221e6b76c777'
const test_sign
  = 'SIG_K1_KmKWPBC8dZGGDGhbKEoZEzPr3h5crRrR2uLdGRF5DJbeibY1MY1bZ9sPwHsgmPfiGFv9psfoCVsXFh9TekcLuvaeuxRKA8'
const test_pkey = 'EOS5JhMfxbsNebajHcTEK8yC9uNN9Dit9hEmzE8ri8yMhhzxrLg3J'
const test_meta = JSON.stringify({})

const document = {
  hash: test_hash,
  signature: test_sign,
  public_key: test_pkey,
  meta: test_meta,
}

export class CooperativeClass {
  public blockchain: Blockchain

  constructor(blockchain: Blockchain) {
    this.blockchain = blockchain
  }

  async createProgramsAndAgreements(coopname: string) {
    await this.blockchain.createProgram({
      coopname,
      username: coopname,
      draft_id: TCooperative.Registry.WalletAgreement.registry_id,
      title: 'Цифровой Кошелёк',
      announce: '',
      description: '',
      preview: '',
      images: '',
      calculation_type: 'free',
      fixed_membership_contribution: `${Number(0).toFixed(4)} ${GOVERN_SYMBOL}`,
      membership_percent_fee: '0',
      meta: '',
    })

    console.log('создаём кооперативное соглашение/положение по кошельку')
    await this.blockchain.makeCoagreement({
      coopname,
      administrator: coopname,
      type: 'wallet',
      draft_id: TCooperative.Registry.WalletAgreement.registry_id,
      program_id: 1,
    })

    await this.blockchain.makeCoagreement({
      coopname,
      administrator: coopname,
      type: 'signature',
      draft_id: TCooperative.Registry.RegulationElectronicSignature.registry_id,
      program_id: 0,
    })
    await this.blockchain.makeCoagreement({
      coopname,
      administrator: coopname,
      type: 'user',
      draft_id: TCooperative.Registry.UserAgreement.registry_id,
      program_id: 0,
    })

    await this.blockchain.makeCoagreement({
      coopname,
      administrator: coopname,
      type: 'privacy',
      draft_id: TCooperative.Registry.PrivacyPolicy.registry_id,
      program_id: 0,
    })

    await this.blockchain.makeCoagreement({
      coopname,
      administrator: coopname,
      type: 'coopenomics',
      draft_id: TCooperative.Registry.CoopenomicsAgreement.registry_id,
      program_id: 0,
    })
  }

  async createCooperative(username: string, keys?: Keys) {
    const account = await this.blockchain.generateKeypair(
      username,
      keys,
      'Аккаунт кооператива',
    )
    console.log('Регистрируем аккаунт')

    await this.blockchain.registerAccount2({
      registrator: config.provider,
      coopname: config.provider,
      referer: '',
      username: account.username,
      public_key: account.publicKey,
      meta: '',
    })

    console.log('Регистрируем аккаунт как пользователя')

    await this.blockchain.registerUser({
      registrator: config.provider,
      coopname: config.provider,
      username: account.username,
      type: 'organization',
    })

    console.log('Переводим аккаунт в организации')

    await this.blockchain.registerOrganization({
      registrator: config.provider_chairman,
      username: account.username,
      coopname: account.username,
      params: {
        is_cooperative: true,
        coop_type: 'conscoop',
        announce: 'Тестовый кооператив',
        description: 'Тестовое описание',
        initial: `100.0000 ${config.token.govern_symbol}`,
        minimum: `300.0000 ${config.token.govern_symbol}`,
        org_initial: `1000.0000 ${config.token.govern_symbol}`,
        org_minimum: `3000.0000 ${config.token.govern_symbol}`,
      },
      document,
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

    console.log('создаём программы и соглашения новому кооперативу')
    await this.blockchain.transfer({ from: 'eosio', to: username, quantity: `100.0000 ${SYMBOL}`, memo: '' })
    console.log('Переводим инициализационные токены')

    console.log(`Арендуем ресурсы кооперативу`)

    await this.blockchain.powerup({
      payer: 'eosio',
      receiver: username,
      days: config.powerup.days,
      payment: `100.0000 ${config.token.symbol}`,
      transfer: true,
    })
  }
}

export async function startCoop() {
  // инициализируем инстанс с ключами
  const blockchain = new Blockchain(config.network, config.private_keys)
  const cooperative = new CooperativeClass(blockchain)

  await cooperative.createCooperative('cooperative1', {
    // eslint-disable-next-line node/prefer-global/process
    privateKey: process.env.EOSIO_PRV_KEY!,
    // eslint-disable-next-line node/prefer-global/process
    publicKey: process.env.EOSIO_PUB_KEY!,
  })

  await blockchain.preInit({
    coopname: 'cooperative1',
    username: config.provider,
    status: 'active',
  })

  console.log('Кооператив предварительно подготовлен к установке совета.')
}
