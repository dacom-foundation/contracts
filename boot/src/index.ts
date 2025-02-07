/* eslint-disable node/prefer-global/process */
import path from 'node:path'
import { Command } from 'commander'
import { config } from 'dotenv'
import { execCommand } from './docker/exec'
import { stopContainerByName } from './docker/stop'
import { runContainer } from './docker/run'
import { boot } from './init/booter'
import { sleep } from './utils'
import { checkHealth } from './docker/health'
import { clearDirectory, deleteFile } from './docker/purge'
import { deployCommand } from './docker/deploy'

config()

const basePath = path.resolve(process.cwd(), '../blockchain-data')
const keosdPath = path.resolve(process.cwd(), '../wallet-data/keosd.sock')

const program = new Command()

program.version('0.1.0')

// Команда для запуска команды в контейнере
program
  .command('cleos <cmd...>')
  .description('Execute a cleos command in a Node container')
  .allowUnknownOption()
  .action(async (cmd: string[]) => {
    try {
      console.log(cmd)
      await execCommand(['cleos', ...cmd])
    }
    catch (error) {
      console.error('Command execution failed:', error)
    }
  })

// Команда для запуска команды в контейнере
program
  .command('deploy <contract_name>')
  .description('Execute a deploy command in a Node container')
  .option('-t, --target <target>', 'Specify the target')
  .option('-n, --network <network>', 'Specify the network', 'local')
  .allowUnknownOption()
  .action(async (cmd: string, options: any) => {
    try {
      const { target, network } = options

      await execCommand([
        'cleos',
        'wallet',
        'unlock',
        '--password',
        process.env.PASSWORD!,
      ])
      await deployCommand(cmd, target, network)
    }
    catch (error) {
      console.error('Command execution failed:', error)
    }
  })

// Команда для получения списка контейнеров
program
  .command('stop')
  .description('Stop blockchain node as is')
  .action(async () => {
    try {
      await stopContainerByName('node')

      console.log('Container is stopped')
    }
    catch (error) {
      console.error('Failed to list containers:', error)
    }
  })

// Команда для запуска команды в контейнере
program
  .command('unlock')
  .description('Unlock a cleos wallet with a .env password')
  .allowUnknownOption()
  .action(async () => {
    try {
      await execCommand([
        'cleos',
        'wallet',
        'unlock',
        '--password',
        process.env.PASSWORD!,
      ])
    }
    catch (error) {
      console.error('Command execution failed:', error)
    }
  })

// Команда для получения списка контейнеров
program
  .command('start')
  .description('Start blockchain node as is')
  .action(async () => {
    try {
      await deleteFile(keosdPath)
      await runContainer()
      console.log('Container is started')
    }
    catch (error) {
      console.error('Failed to list containers:', error)
    }
  })

// Команда для получения списка контейнеров
program
  .command('boot')
  .description('Purge blockchain data and boot a Protocol')
  .action(async () => {
    try {
      await deleteFile(keosdPath)
      await stopContainerByName('node')
      await clearDirectory(basePath)
      await sleep(5000)
      await runContainer()
      await checkHealth()
      await boot()
      console.log('Boot process completed')
    }
    catch (error) {
      console.error('Failed to boot:', error)
    }
  })

// Команда для получения списка контейнеров
program
  .command('clean-launch')
  .description('Purge blockchain data and boot a Protocol')
  .action(async () => {
    try {
      await deleteFile(keosdPath)
      await stopContainerByName('node')
      await clearDirectory(basePath)
      await sleep(5000)
      await runContainer()
      await checkHealth()
    }
    catch (error) {
      console.error('Failed to boot:', error)
    }
  })

// Команда для получения списка контейнеров
program
  .command('only-boot')
  .description('Boot a Protocol')
  .action(async () => {
    try {
      await boot()
    }
    catch (error) {
      console.error('Failed to boot:', error)
    }
  })

program.parse(process.argv) // Пуск парсинга аргументов

async function gracefulShutdown() {
  console.log('Stopping container...')
  await stopContainerByName('node')

  process.exit(0)
}

process.on('SIGINT', gracefulShutdown)
process.on('SIGTERM', gracefulShutdown)
