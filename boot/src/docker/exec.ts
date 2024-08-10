import type { Container } from 'dockerode'
import { findContainerByName } from './find'

export async function execCommand(command: string[]): Promise<string> {
  const container = await findContainerByName('node')

  if (!container) {
    throw new Error('Container is not found')
  }

  const exec = await container.exec({
    Cmd: command,
    AttachStdout: true,
    AttachStderr: true,
  })

  return new Promise((resolve, reject) => {
    exec.start({}, (err: any, stream: any) => {
      if (err) {
        return reject(err)
      }

      let output = ''

      // eslint-disable-next-line node/prefer-global/buffer
      stream.on('data', (data: Buffer) => {
        output += data.toString()
      })

      stream.on('end', () => {
        resolve(output)
      })

      stream.on('error', (err: any) => {
        reject(err)
      })
    })
  })
}
