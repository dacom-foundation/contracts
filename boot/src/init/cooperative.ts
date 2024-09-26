import axios from "axios"
import { describe, expect, it } from "vitest"
import { Registry } from "coopdoc-generator-ts"
import { Cooperative as TCooperative } from "cooptypes"
import type { Account, Contract, Keys } from "../types"
import config, { GOVERN_SYMBOL } from "../configs"
import Blockchain from "../blockchain"
import { sendPostToCoopbackWithSecret, sleep } from "../utils"

const test_hash =
  "157192b276da23cc84ab078fc8755c051c5f0430bf4802e55718221e6b76c777"
const test_sign =
  "SIG_K1_KmKWPBC8dZGGDGhbKEoZEzPr3h5crRrR2uLdGRF5DJbeibY1MY1bZ9sPwHsgmPfiGFv9psfoCVsXFh9TekcLuvaeuxRKA8"
const test_pkey = "EOS5JhMfxbsNebajHcTEK8yC9uNN9Dit9hEmzE8ri8yMhhzxrLg3J"
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
    const account = await this.blockchain.generateKeypair(
      username,
      keys,
      "Аккаунт кооператива"
    )
    console.log("Регистрируем аккаунт")
    await this.blockchain.registerAccount2({
      registrator: config.provider,
      coopname: config.provider,
      referer: "",
      username: account.username,
      public_key: account.publicKey,
      meta: "",
    })

    console.log("Регистрируем аккаунт как пользователя")

    await this.blockchain.registerUser({
      registrator: config.provider,
      coopname: config.provider,
      username: account.username,
      type: "organization",
    })

    console.log("Переводим аккаунт в организации")

    await this.blockchain.registerOrganization({
      registrator: config.provider_chairman,
      coopname: account.username,
      params: {
        is_cooperative: true,
        coop_type: "conscoop",
        announce: "Тестовый кооператив",
        description: "Тестовое описание",
        initial: `500.0000 ${config.token.govern_symbol}`,
        minimum: `500.0000 ${config.token.govern_symbol}`,
        org_initial: `1000.0000 ${config.token.govern_symbol}`,
        org_minimum: `1000.0000 ${config.token.govern_symbol}`,
      },
    })

    console.log("Отправляем заявление на вступление")

    await this.blockchain.joinCoop({
      registrator: config.provider_chairman,
      coopname: config.provider,
      username: account.username,
      document,
    })

    console.log("Голосуем по решению в провайдере")

    await this.blockchain.votefor({
      coopname: config.provider,
      member: config.provider_chairman,
      decision_id: 1,
    })

    console.log("Утверждаем решение в провайдере")

    await this.blockchain.authorize({
      coopname: config.provider,
      chairman: config.provider_chairman,
      decision_id: 1,
      document,
    })

    console.log("Исполняем решение в провайдере")

    await this.blockchain.exec({
      executer: config.provider_chairman,
      coopname: config.provider,
      decision_id: 1,
    })

    await sleep(2000)

    // const vars: TCooperative.Model.ICovars = {
    //   coopname: 'voskhod',
    //   full_abbr: 'потребительский кооператив',
    //   full_abbr_genitive: 'потребительского кооператива',
    //   full_abbr_dative: 'потребительскому кооперативу',
    //   short_abbr: 'ПК',
    //   website: 'coopenomics.world',
    //   name: 'ВОСХОД',
    //   confidential_link: 'coopenomics.world/privacy',
    //   confidential_email: 'privacy@coopenomics.world',
    //   contact_email: 'contact@coopenomics.world',
    //   wallet_agreement: {
    //     protocol_number: '10-04-2024',
    //     protocol_day_month_year: '10 апреля 2024 г.',
    //   },
    //   signature_agreement: {
    //     protocol_number: '11-04-2024',
    //     protocol_day_month_year: '11 апреля 2024 г.',
    //   },
    //   privacy_agreement: {
    //     protocol_number: '12-04-2024',
    //     protocol_day_month_year: '12 апреля 2024 г.',
    //   },
    //   user_agreement: {
    //     protocol_number: '13-04-2024',
    //     protocol_day_month_year: '13 апреля 2024 г.',
    //   },
    // }

    // console.log('устанавливаем переменные кооператива')
    // await sendPostToCoopbackWithSecret('/v1/system/set-vars', vars)

    console.log("создаём программу кошелька")
    await this.blockchain.createProgram({
      coopname: config.provider,
      username: config.provider_chairman,
      draft_id: TCooperative.Registry.WalletAgreement.registry_id,
      title: "Цифровой Кошелёк",
      announce: "",
      description: "",
      preview: "",
      images: "",
      calculation_type: "free",
      fixed_membership_contribution: `${Number(0).toFixed(4)} ${GOVERN_SYMBOL}`,
      membership_percent_fee: "0",
      meta: "",
    })

    console.log("создаём кооперативное соглашение/положение по кошельку")
    await this.blockchain.makeCoagreement({
      coopname: config.provider,
      administrator: config.provider_chairman,
      type: "wallet",
      draft_id: TCooperative.Registry.WalletAgreement.registry_id,
      program_id: 1,
    })

    await this.blockchain.makeCoagreement({
      coopname: config.provider,
      administrator: config.provider_chairman,
      type: "signature",
      draft_id: TCooperative.Registry.RegulationElectronicSignature.registry_id,
      program_id: 0,
    })
    await this.blockchain.makeCoagreement({
      coopname: config.provider,
      administrator: config.provider_chairman,
      type: "user",
      draft_id: TCooperative.Registry.UserAgreement.registry_id,
      program_id: 0,
    })

    await this.blockchain.makeCoagreement({
      coopname: config.provider,
      administrator: config.provider_chairman,
      type: "privacy",
      draft_id: TCooperative.Registry.PrivacyPolicy.registry_id,
      program_id: 0,
    })

    await this.blockchain.makeCoagreement({
      coopname: config.provider,
      administrator: config.provider_chairman,
      type: "coopenomics",
      draft_id: TCooperative.Registry.CoopenomicsAgreement.registry_id,
      program_id: 0,
    })
  }
}

export async function startCoop() {
  // инициализируем инстанс с ключами
  const blockchain = new Blockchain(config.network, config.private_keys)
  const cooperative = new Cooperative(blockchain)

  await blockchain.powerup({
    payer: "eosio",
    receiver: config.provider,
    days: config.powerup.days,
    payment: `100.0000 ${config.token.symbol}`,
    transfer: true,
  })

  await blockchain.transfer({
    from: "eosio",
    to: config.provider,
    quantity: `100.0000 ${config.token.symbol}`,
    memo: "",
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
