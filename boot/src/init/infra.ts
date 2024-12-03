import { randomUUID } from 'node:crypto'
import axios from 'axios'
import { Generator, Registry } from 'coopdoc-generator-ts'
import type { Cooperative } from 'cooptypes'
import { DraftContract } from 'cooptypes'
import type { Account, Contract } from '../types'
import config from '../configs'
import Blockchain from '../blockchain'
import { sleep } from '../utils'
import { CooperativeClass } from './cooperative'

export async function startInfra() {
  // инициализируем инстанс с ключами
  const blockchain = new Blockchain(config.network, config.private_keys)
  await blockchain.update_pass_instance()

  // регистрируем базовые аккаунты
  for (const account of config.accounts) {
    const { name, ownerPublicKey, activePublicKey } = account as Account
    await blockchain.createStandartAccount(
      'eosio',
      name,
      ownerPublicKey || config.default_public_key,
      activePublicKey || config.default_public_key,
    )
  }

  // пре-активируем фичу для запуска
  const url = `${config.network.protocol}://${config.network.host}${config.network.port}`

  try {
    const response = await axios.post(
      `${url}/v1/producer/schedule_protocol_feature_activations`,
      {
        protocol_features_to_activate: [
          '0ec7e080177b2c02b278d5088611686b49d739925a92d9bfcacd7fc6b74053bd',
        ],
      },
    )
    console.log('ok -> init activation: ', response.data)
  }
  catch (e) {
    console.log('error -> init activation: ', e)
  }

  // чуть ждём
  await sleep(1000)

  // устанавливаем биос
  const bios = config.contracts.find(el => el.name === 'eosio.boot')
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
    el => el.name !== 'eosio.boot',
  )
  for (const contract of filtered_contracts)
    await blockchain.setContract(contract)

  await sleep(2000)

  console.log('создаём токен')
  await blockchain.createToken({
    issuer: 'eosio',
    maximum_supply: config.token.max_supply,
  })

  await sleep(2000)

  console.log('выпускаем токены')
  for (const allocation of config.allocations) {
    await blockchain.issueToken({
      to: allocation.to,
      quantity: allocation.quantity,
      memo: '',
    })
  }

  await sleep(2000)

  // выдаём кодовые разрешения всем указанным аккаунтам
  for (const account of config.accounts.filter(
    el => !!el.code_permissions_to,
  )) {
    for (const permission_to of account.code_permissions_to ?? []) {
      await blockchain.updateAccountPermissionsToCode(
        account.name,
        permission_to,
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
      username: 'eosio',
      registry_id: id,
      lang: 'ru',
      title: template.Template.title,
      description: template.Template.description,
      context: template.Template.context,
      model: JSON.stringify(template.Template.model),
      translation_data: JSON.stringify(template.Template.translations.ru),
    })
  }

  const organizationData: Cooperative.Users.IOrganizationData = {
    username: 'voskhod',
    type: 'coop',
    short_name: '"ПК Восход"',
    full_name: 'Потребительский Кооператив "ВОСХОД"',
    represented_by: {
      first_name: 'Алексей',
      last_name: 'Муравьев',
      middle_name: 'Николаевич',
      position: 'Председатель',
      based_on: 'Решение общего собрания №1',
    },
    country: 'Russia',
    city: 'Москва',
    fact_address: '117593 г. Москва, муниципальный округ Ясенево, проезд Соловьиный, дом 1, помещение 1/1',
    full_address:
      '117593 г. Москва, муниципальный округ Ясенево, проезд Соловьиный, дом 1, помещение 1/1',
    email: 'copenomics@yandex.ru',
    phone: '+71234567890',
    details: {
      inn: '9728130611',
      ogrn: '1247700283346',
      kpp: '772801001',
    },

  }

  const generator = new Generator()
  // eslint-disable-next-line node/prefer-global/process
  await generator.connect(process.env.MONGO_URI as string)

  await generator.save('organization', organizationData)
  console.log('Провайдер добавлен: ', organizationData)

  await generator.save('paymentMethod', {
    is_default: true,
    method_id: randomUUID(),
    method_type: 'bank_transfer',
    username: 'voskhod',
    data: {
      account_number: '40703810038000110117',
      currency: 'RUB',
      card_number: '',
      bank_name: 'ПАО Сбербанк',
      details: {
        bik: '044525225',
        corr: '30101810400000000225',
        kpp: '773643001',
      },
    },
  })

  console.log('Создаём программы и соглашения')

  const cooperative = new CooperativeClass(blockchain)
  await cooperative.createProgramsAndAgreements(config.provider)

  console.log(`Арендуем ресурсы провайдеру`)
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

  console.log('Базовая установка завершена')
}
