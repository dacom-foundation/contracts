/* eslint-disable node/prefer-global/process */
import path from 'node:path'
import { Command } from 'commander'
import { execCommand } from './docker/exec'
import { stopContainerByName } from './docker/stop'
import { runContainer } from './docker/run'
import { boot } from './init/booter'
import { sleep } from './utils'
import { checkHealth } from './docker/health'
import { clearDirectory } from './docker/purge'

const basePath = path.resolve(process.cwd(), '../blockchain-data')

const program = new Command()

program.version('0.1.0')

// Команда для запуска команды в контейнере
program
  .command('cleos <cmd...>')
  .description('Execute a cleos command in a Node container')
  .allowUnknownOption()
  .action(async (cmd: string[]) => {
    try {
      await execCommand(['cleos', ...cmd])
    }
    catch (error) {
      console.error('Command execution failed:', error)
    }
  })

// Команда для запуска команды в контейнере
program
  .command('stop')
  .description('Stop a Node container')
  .action(async () => {
    try {
      await stopContainerByName('node')
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
      await stopContainerByName('node')
      await clearDirectory(basePath)
      await runContainer()
      await checkHealth()
      await boot()
      console.log('finish')
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
