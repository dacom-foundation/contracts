/* eslint-disable node/prefer-global/process */
import path from 'node:path'
import type { Container } from 'dockerode'
import Docker from 'dockerode'
import { config } from 'dotenv'
import { findContainerByName } from './find'

config()

const docker = new Docker()

export async function runContainer(once = false): Promise<Container> {
  const existingContainer = await findContainerByName('node')

  if (existingContainer) {
    return existingContainer
  }

  const container = await docker.createContainer({
    Image: 'dicoop/blockchain:v5.1.0-dev',
    name: 'node',
    Tty: !once,
    HostConfig: {
      PortBindings: {
        '8888/tcp': [{ HostPort: '8888' }],
        '9876/tcp': [{ HostPort: '9876' }],
        '8080/tcp': [{ HostPort: '8080' }],
      },
      Binds: [
        `${path.resolve('../blockchain-data')}:/mnt/dev/data`,
        `${path.resolve('./src/configs')}:/mnt/dev/config`,
        `${path.resolve('../wallet-data')}:/root/eosio-wallet`,
        `${path.resolve('../build/contracts')}:/contracts`,
      ],
      AutoRemove: true,
    },
    ExposedPorts: {
      '8888/tcp': {},
      '9876/tcp': {},
      '8080/tcp': {},
    },
    Env: Object.keys(process.env).map(key => `${key}=${process.env[key]}`),
    Cmd: [
      '/bin/bash',
      '-c',
      '/usr/local/bin/nodeos -d /mnt/dev/data --config-dir /mnt/dev/config --genesis-json /mnt/dev/config/genesis.json',
    ],
  })

  await container.start()

  if (!once) {
    const stream = await container.logs({
      follow: true,
      stdout: true,
      stderr: true,
    })

    // eslint-disable-next-line node/prefer-global/buffer
    stream.on('data', (data: Buffer) => {
      console.log(data.toString())
    })
  }
  return container
}
