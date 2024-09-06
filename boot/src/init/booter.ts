import { startInfra } from "./infra"
import { startCoop } from "./cooperative"

export async function boot() {
  await startInfra()
  await startCoop()
}
