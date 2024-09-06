import axios from "axios"
import { Registry } from "coopdoc-generator-ts"
import { DraftContract } from "cooptypes"
import type { Account, Contract } from "../types"
import config from "../configs"
import Blockchain from "../blockchain"
import { sleep } from "../utils"

export async function startInfra() {
  // инициализируем инстанс с ключами
  const blockchain = new Blockchain(config.network, config.private_keys)
  await blockchain.update_pass_instance()

  // регистрируем базовые аккаунты
  for (const account of config.accounts) {
    const { name, ownerPublicKey, activePublicKey } = account as Account
    await blockchain.createStandartAccount(
      "eosio",
      name,
      ownerPublicKey || config.default_public_key,
      activePublicKey || config.default_public_key
    )
  }

  // пре-активируем фичу для запуска
  const url = `${config.network.protocol}://${config.network.host}${config.network.port}`

  try {
    const response = await axios.post(
      `${url}/v1/producer/schedule_protocol_feature_activations`,
      {
        protocol_features_to_activate: [
          "0ec7e080177b2c02b278d5088611686b49d739925a92d9bfcacd7fc6b74053bd",
        ],
      }
    )
    console.log("ok -> init activation: ", response.data)
  } catch (e) {
    console.log("error -> init activation: ", e)
  }

  // чуть ждём
  await sleep(1000)

  // устанавливаем биос
  const bios = config.contracts.find((el) => el.name === "eosio.boot")
  await blockchain.setContract(bios as Contract)

  // чуть ждём
  await sleep(2000)

  // активируем все оставшиеся фичи
  for (const feature of config.features)
    await blockchain.activateFeature(feature)

  // чуть ждём
  await sleep(2000)

  // устанавливаем все оставшиеся контракты
  const filtered_contracts = config.contracts.filter(
    (el) => el.name !== "eosio.boot"
  )
  for (const contract of filtered_contracts)
    await blockchain.setContract(contract)

  await sleep(2000)

  console.log("создаём токен")
  await blockchain.createToken({
    issuer: "eosio",
    maximum_supply: config.token.max_supply,
  })

  await sleep(2000)

  console.log("выпускаем токены")
  for (const allocation of config.allocations) {
    await blockchain.issueToken({
      to: allocation.to,
      quantity: allocation.quantity,
      memo: "",
    })
  }

  await sleep(2000)

  // выдаём кодовые разрешения всем указанным аккаунтам
  for (const account of config.accounts.filter(
    (el) => !!el.code_permissions_to
  )) {
    for (const permission_to of account.code_permissions_to ?? []) {
      await blockchain.updateAccountPermissionsToCode(
        account.name,
        permission_to
      )
    }
  }

  await sleep(1000)

  // инициализируем системный контракт
  await blockchain.initSystem({
    version: 0,
    core: `${config.token.precision},${config.token.symbol}`,
  })

  await sleep(1000)

  // инициализируем эмиссию
  await blockchain.initEmission({
    init_supply: config.emission.left_border,
    tact_duration: config.emission.tact_duration,
    emission_factor: config.emission.emission_factor,
  })

  await sleep(2000)

  await blockchain.initPowerup({
    args: {
      powerup_days: config.powerup.days,
      min_powerup_fee: config.powerup.min_powerup,
    },
  })

  await sleep(2000)

  for (const id in Registry) {
    const template = Registry[(id as unknown) as keyof typeof Registry]

    await blockchain.createDraft({
      scope: DraftContract.contractName.production,
      username: "eosio",
      registry_id: id,
      lang: "ru",
      title: template.Template.title,
      description: template.Template.description,
      context: template.Template.context,
      model: JSON.stringify(template.Template.model),
      translation_data: JSON.stringify(template.Template.translations.ru),
    })
  }

  await blockchain.createDraft({
    scope: config.provider,
    username: config.provider_chairman,
    registry_id: Registry[1].registry_id,
    lang: "ru",
    title: Registry[1].Template.title,
    description: Registry[1].Template.description,
    context: Registry[1].Template.context,
    model: JSON.stringify(Registry[1].Template.model),
    translation_data: JSON.stringify(Registry[1].Template.translations.ru),
  })
}
