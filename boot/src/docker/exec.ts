import type { Container } from "dockerode"
import { findContainerByName } from "./find"
import { runContainer } from "./run"

export async function execCommand(command: string[]): Promise<string> {
  const container = await runContainer(true)

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

      let output = ""

      // eslint-disable-next-line node/prefer-global/buffer
      stream.on("data", (data: Buffer) => {
        output += data.toString()
      })

      stream.on("end", () => {
        output = output.replace(/^\d+\s*/, "")
        console.log(output)
        resolve(output)
      })

      stream.on("error", (err: any) => {
        reject(err)
      })
    })
  })
}
