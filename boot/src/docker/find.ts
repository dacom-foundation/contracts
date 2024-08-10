import Docker from 'dockerode'

const docker = new Docker()

export async function findContainerByName(name: string) {
  const containers = await docker.listContainers({ all: true })
  const containerInfo = containers.find(c => c.Names.includes(`/${name}`))

  if (containerInfo) {
    return docker.getContainer(containerInfo.Id)
  }

  return null
}
