/**
 * Генерирует случайное имя аккаунта длиной 12 символов из букв a-z
 */
export function generateRandomUsername(): string {
  const alphabet = 'abcdefghijklmnopqrstuvwxyz'
  let result = ''

  for (let i = 0; i < 12; i++) {
    const randomIndex = Math.floor(Math.random() * alphabet.length)
    result += alphabet[randomIndex]
  }

  return result
}
