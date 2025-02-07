import { createHash, randomBytes } from 'node:crypto'

export function generateRandomSHA256(): string {
  const randomData = randomBytes(32).toString('hex') // 32 байта случайных данных
  return createHash('sha256').update(randomData).digest('hex')
}
