import Docker from 'dockerode'
import { findContainerByName } from './find'

export async function stopContainerByName(name: string) {
  const container = await findContainerByName(name)

  if (container) {
    await container.stop()
    console.log(`Container stopped.`)
  }
}
