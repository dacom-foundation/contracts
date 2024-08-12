import type { Network } from '../types'

export const networks: Network[] = [
  {
    name: 'local',
    protocol: 'http',
    host: '127.0.0.1',
    port: ':8888',
  },
]
