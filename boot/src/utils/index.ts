import axios from "axios"

export function sleep(ms: number) {
  return new Promise((resolve) => setTimeout(resolve, ms))
}

export async function sendPostToCoopbackWithSecret(url: string, data: any) {
  // eslint-disable-next-line node/prefer-global/process
  const base = process.env.SERVER_URL

  return axios.post(`${base}${url}`, data, {
    headers: {
      // eslint-disable-next-line node/prefer-global/process
      "server-secret": process.env.SERVER_SECRET,
      "Content-Type": "application/json",
    },
  })
}
