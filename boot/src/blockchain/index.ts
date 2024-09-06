import { TextDecoder, TextEncoder } from "node:util" // Add missing import
import fs from "node:fs"
import { JsSignatureProvider } from "eosjs/dist/eosjs-jssig"
import { Api, JsonRpc } from "eosjs"
import {
  DraftContract,
  RegistratorContract,
  SovietContract,
  SystemContract,
  TokenContract,
} from "cooptypes"
import config from "../configs"

import EosApi from "eosjs-api"
import ecc from "eosjs-ecc"

import type { Contract, Feature, Keys, Network } from "../types"

import { Serialize } from `eosjs`

export default class Blockchain {
  public signatureProvider: any
  public privateKeys: any[] = []
  public api: any
  public network: Network
  public new_accounts: any[] = []

  constructor(network: Network, privateKeys: string[] = []) {
    this.network = network
    this.privateKeys = privateKeys
  }

  async update_pass_instance() {
    const protocol = this.network.protocol
    const endpoint = this.network.host
    const port = this.network.port
    const res = `${protocol}://${endpoint}${port}`

    const rpc = new JsonRpc(res, { fetch })

    const signatureProvider = new JsSignatureProvider(this.privateKeys)

    this.api = new Api({
      rpc,
      signatureProvider,
      textDecoder: new TextDecoder(),
      textEncoder: new TextEncoder(),
    })
    this.api.read = await EosApi({ httpEndpoint: res })
  }

  generateRandomUsername(): string {
    const characters = "abcdefghijklmnopqrstuvwxyz"
    let username = ""
    for (let i = 0; i < 12; i++) {
      const randomIndex = Math.floor(Math.random() * characters.length)
      username += characters[randomIndex]
    }
    return username
  }

  async generateKeypair(username?: string, keys?: Keys, memo?: string) {
    let privateKey = ""
    let publicKey = ""

    if (!username) username = this.generateRandomUsername()

    if (keys) {
      privateKey = keys.privateKey
      publicKey = keys.publicKey
    } else {
      privateKey = await ecc.randomKey()
      publicKey = await ecc.privateToPublic(privateKey)
    }

    this.privateKeys = [...this.privateKeys, privateKey]

    this.new_accounts.push({ username, privateKey, publicKey, memo })

    await this.update_pass_instance()

    console.log("\tusername: ", username)
    console.log("\tprivateKey: ", privateKey)
    console.log("\tpublicKey: ", publicKey)
    console.log("\tmemo: ", memo)

    return { username, privateKey, publicKey, memo }
  }

  async createStandartAccount(
    creator: string,
    accountName: string,
    ownerPublicKey: string,
    activePublicKey: string
  ) {
    try {
      await this.update_pass_instance()
      const result = await this.api.transact(
        {
          actions: [
            {
              account: "eosio",
              name: "newaccount",
              authorization: [
                {
                  actor: creator,
                  permission: "active",
                },
              ],
              data: {
                creator,
                name: accountName,
                owner: {
                  threshold: 1,
                  keys: [
                    {
                      key: ownerPublicKey,
                      weight: 1,
                    },
                  ],
                  accounts: [],
                  waits: [],
                },
                active: {
                  threshold: 1,
                  keys: [
                    {
                      key: activePublicKey,
                      weight: 1,
                    },
                  ],
                  accounts: [],
                  waits: [],
                },
              },
            },
          ],
        },
        {
          blocksBehind: 3,
          expireSeconds: 30,
        }
      )
      console.log(`Created account: ${accountName}`)
      return result
    } catch (e) {
      console.error(e)
    }
  }

  async setContract(contract: Contract) {
    try {
      console.log("on set: ", contract)
      const wasm_path = `${contract.path}/${contract.name}.wasm`
      const abi_path = `${contract.path}/${contract.name}.abi`

      console.log("wasm_path", wasm_path)
      console.log("abi_path", abi_path)

      const wasm = fs.readFileSync(wasm_path)
      const abi = fs.readFileSync(abi_path)

      const buffer = new Serialize.SerialBuffer({
        textEncoder: this.api.textEncoder,
        textDecoder: this.api.textDecoder,
      })

      const abiDefinitions = this.api.abiTypes.get("abi_def")

      let abiJSON = JSON.parse(abi) // Convert the abi buffer to a string before parsing it as JSON

      abiJSON = abiDefinitions.fields.reduce(
        (acc: { [x: string]: any }, { name: fieldName }: any) =>
          Object.assign(acc, { [fieldName]: acc[fieldName] || [] }),
        abiJSON
      )
      abiDefinitions.serialize(buffer, abiJSON)

      const serializedAbiHexString = Buffer.from(
        buffer.asUint8Array()
      ).toString("hex")

      const data = {
        account: contract.target,
        vmtype: 0,
        vmversion: 0,
        code: wasm,
      }
      // console.log(data)
      // console.log("abi: ", serializedAbiHexString)

      this.api.transact(
        {
          actions: [
            {
              account: "eosio",
              name: "setcode",
              authorization: [
                {
                  actor: contract.target,
                  permission: "active",
                },
              ],
              data,
            },
            {
              account: "eosio",
              name: "setabi",
              authorization: [
                {
                  actor: contract.target,
                  permission: "active",
                },
              ],
              data: {
                account: contract.target,
                abi: serializedAbiHexString,
              },
            },
          ],
        },
        {
          blocksBehind: 3,
          expireSeconds: 30,
        }
      )
      console.log("contract setted: ", contract.target)
    } catch (e) {
      console.log(e)
    }
  }

  async activateFeature(feature: Feature) {
    await this.update_pass_instance()

    await this.api.transact(
      {
        actions: [
          {
            account: "eosio",
            name: "activate",
            authorization: [
              {
                actor: "eosio",
                permission: "active",
              },
            ],
            data: {
              feature_digest: feature.hash,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Фича активирована: ", feature.name)
  }

  async createToken(params: TokenContract.Interfaces.ICreate) {
    await this.update_pass_instance()

    await this.api.transact(
      {
        actions: [
          {
            account: "eosio.token",
            name: TokenContract.Actions.Create.actionName,
            authorization: [
              {
                actor: "eosio.token",
                permission: "active",
              },
            ],
            data: {
              ...params,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Токен создан: ", params)
  }

  async issueToken(params: TokenContract.Interfaces.IIssue) {
    await this.update_pass_instance()

    await this.api.transact(
      {
        actions: [
          {
            account: "eosio.token",
            name: TokenContract.Actions.Issue.actionName,
            authorization: [
              {
                actor: "eosio",
                permission: "active",
              },
            ],
            data: {
              ...params,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Токен выпущен: ", params)
  }

  async initSystem(params: SystemContract.Actions.Init.IInit) {
    await this.update_pass_instance()

    await this.api.transact(
      {
        actions: [
          {
            account: SystemContract.contractName.production,
            name: SystemContract.Actions.Init.actionName,
            authorization: [
              {
                actor: "eosio",
                permission: "active",
              },
            ],
            data: {
              ...params,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Системный контракт инициализирован", params)
  }

  async initEmission(
    params: SystemContract.Actions.InitEmission.IInitEmission
  ) {
    await this.update_pass_instance()

    await this.api.transact(
      {
        actions: [
          {
            account: SystemContract.contractName.production,
            name: SystemContract.Actions.InitEmission.actionName,
            authorization: [
              {
                actor: "eosio",
                permission: "active",
              },
            ],
            data: {
              ...params,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Эмиссия инициализирована", params)
  }

  async initPowerup(params: SystemContract.Actions.InitPowerup.IInitPowerup) {
    await this.update_pass_instance()

    await this.api.transact(
      {
        actions: [
          {
            account: SystemContract.contractName.production,
            name: SystemContract.Actions.InitPowerup.actionName,
            authorization: [
              {
                actor: "eosio",
                permission: "active",
              },
            ],
            data: {
              ...params,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Система аренды инициализирована", params)
  }

  async powerup(params: SystemContract.Actions.Powerup.IPowerup) {
    await this.update_pass_instance()

    await this.api.transact(
      {
        actions: [
          {
            account: SystemContract.contractName.production,
            name: SystemContract.Actions.Powerup.actionName,
            authorization: [
              {
                actor: params.payer,
                permission: "active",
              },
            ],
            data: {
              ...params,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Аренда ресурсов: ", params)
  }

  //TODO change registerOrganization to registerCooperative
  async registerOrganization(
    params: RegistratorContract.Actions.RegisterCooperative.IRegisterCooperative
  ) {
    await this.update_pass_instance()
    console.log(params)
    await this.api.transact(
      {
        actions: [
          {
            account: RegistratorContract.contractName.production,
            name: RegistratorContract.Actions.RegisterCooperative.actionName,
            authorization: [
              {
                actor: params.registrator,
                permission: "active",
              },
            ],
            data: {
              ...params,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Новая организация: ", params)
  }

  async registerAccount2(
    params: RegistratorContract.Actions.CreateAccount.ICreateAccount
  ) {
    await this.update_pass_instance()

    await this.api.transact(
      {
        actions: [
          {
            account: RegistratorContract.contractName.production,
            name: RegistratorContract.Actions.CreateAccount.actionName,
            authorization: [
              {
                actor: params.registrator,
                permission: "active",
              },
            ],
            data: {
              ...params,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Новый аккаунт 2: ", params)
  }

  async registerUser(
    params: RegistratorContract.Actions.RegisterUser.IRegistrerUser
  ) {
    await this.update_pass_instance()

    await this.api.transact(
      {
        actions: [
          {
            account: RegistratorContract.contractName.production,
            name: RegistratorContract.Actions.RegisterUser.actionName,
            authorization: [
              {
                actor: params.registrator,
                permission: "active",
              },
            ],
            data: {
              ...params,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Новый пользователь: ", params)
  }

  async transfer(params: TokenContract.Actions.Transfer.ITransfer) {
    await this.update_pass_instance()

    await this.api.transact(
      {
        actions: [
          {
            account: TokenContract.contractName.production,
            name: TokenContract.Actions.Transfer.actionName,
            authorization: [
              {
                actor: params.from,
                permission: "active",
              },
            ],
            data: {
              ...params,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Перевод токенов: ", params)
  }

  async updateAccountPermissionsToCode(
    account_for_change: string,
    account_for_set_code: string
  ) {
    try {
      await this.update_pass_instance()

      const result = await this.api.transact(
        {
          actions: [
            {
              account: "eosio",
              name: "updateauth",
              authorization: [
                {
                  actor: account_for_change,
                  permission: "active",
                },
              ],
              data: {
                account: account_for_change,
                permission: "active",
                parent: "owner",
                auth: {
                  threshold: 1,
                  keys: [
                    {
                      key:
                        this.new_accounts.find(
                          (account: any) =>
                            account.username === account_for_change
                        )?.publicKey || config.default_public_key,
                      weight: 1,
                    },
                  ],
                  accounts: [
                    {
                      permission: {
                        actor: account_for_set_code,
                        permission: "eosio.code",
                      },
                      weight: 1,
                    },
                  ],
                  waits: [],
                },
              },
            },
          ],
        },
        {
          blocksBehind: 3,
          expireSeconds: 30,
        }
      )

      console.log(
        `Updated account permissions: ${account_for_change} -> ${account_for_set_code}`
      )
      return result
    } catch (e) {
      console.error(e)
    }
  }

  async votefor(
    params: SovietContract.Actions.Decisions.VoteFor.IVoteForDecision
  ) {
    await this.update_pass_instance()

    await this.api.transact(
      {
        actions: [
          {
            account: SovietContract.contractName.production,
            name: SovietContract.Actions.Decisions.VoteFor.actionName,
            authorization: [
              {
                actor: params.member,
                permission: "active",
              },
            ],
            data: {
              ...params,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Голос за решение: ", params)
  }

  async authorize(
    params: SovietContract.Actions.Decisions.Authorize.IAuthorize
  ) {
    await this.update_pass_instance()

    await this.api.transact(
      {
        actions: [
          {
            account: SovietContract.contractName.production,
            name: SovietContract.Actions.Decisions.Authorize.actionName,
            authorization: [
              {
                actor: params.chairman,
                permission: "active",
              },
            ],
            data: {
              ...params,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Решение утверждено: ", params)
  }

  async exec(params: SovietContract.Actions.Decisions.Exec.IExec) {
    await this.update_pass_instance()

    await this.api.transact(
      {
        actions: [
          {
            account: SovietContract.contractName.production,
            name: SovietContract.Actions.Decisions.Exec.actionName,
            authorization: [
              {
                actor: params.executer,
                permission: "active",
              },
            ],
            data: {
              ...params,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Решение исполнено: ", params)
  }

  async joinCoop(
    params: RegistratorContract.Actions.JoinCooperative.IJoinCooperative
  ) {
    await this.update_pass_instance()

    await this.api.transact(
      {
        actions: [
          {
            account: RegistratorContract.contractName.production,
            name: RegistratorContract.Actions.JoinCooperative.actionName,
            authorization: [
              {
                actor: params.registrator,
                permission: "active",
              },
            ],
            data: {
              ...params,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Заявление на вступление отправлено в совет: ", params)
  }

  async createBoard(
    params: SovietContract.Actions.Boards.CreateBoard.ICreateboard
  ) {
    await this.update_pass_instance()

    await this.api.transact(
      {
        actions: [
          {
            account: SovietContract.contractName.production,
            name: SovietContract.Actions.Boards.CreateBoard.actionName,
            authorization: [
              {
                actor: params.chairman,
                permission: "active",
              },
            ],
            data: {
              ...params,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Совет создан: ", params)
  }

  async createDraft(params: DraftContract.Actions.CreateDraft.ICreateDraft) {
    await this.update_pass_instance()

    await this.api.transact(
      {
        actions: [
          {
            account: DraftContract.contractName.production,
            name: DraftContract.Actions.CreateDraft.actionName,
            authorization: [
              {
                actor: params.username,
                permission: "active",
              },
            ],
            data: {
              ...params,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Шаблон создан: ", params)
  }

  async createTranslation(
    params: DraftContract.Actions.CreateTranslation.ICreateTranslation
  ) {
    await this.update_pass_instance()

    await this.api.transact(
      {
        actions: [
          {
            account: DraftContract.contractName.production,
            name: DraftContract.Actions.CreateTranslation.actionName,
            authorization: [
              {
                actor: "eosio",
                permission: "active",
              },
            ],
            data: {
              ...params,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Перевод создан: ", params)
  }

  async createProgram(
    params: SovietContract.Actions.Programs.CreateProgram.ICreateProgram
  ) {
    await this.update_pass_instance()
    console.log("params", params)
    await this.api.transact(
      {
        actions: [
          {
            account: SovietContract.contractName.production,
            name: SovietContract.Actions.Programs.CreateProgram.actionName,
            authorization: [
              {
                actor: params.username,
                permission: "active",
              },
            ],
            data: {
              ...params,
            },
          },
        ],
      },
      {
        blocksBehind: 3,
        expireSeconds: 30,
      }
    )

    console.log("Программа установлена: ", params)
  }
}
