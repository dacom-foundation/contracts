export const globalRamStats: Record<string, number> = {} // Хранит RAM по каждому `actor:action`

interface RamDelta {
  account: string
  delta: number
}

interface ActInfo {
  authorization: { actor: string }[] // Содержит `username`
  account: string
  name: string
}

interface ActionTrace {
  act?: ActInfo
  account_ram_delta?: RamDelta
  account_ram_deltas?: RamDelta[]
  inline_traces?: ActionTrace[]
}

interface TransactionTrace {
  processed: {
    action_traces: ActionTrace[]
  }
}
function accumulateRam(trace: ActionTrace, loggedActions = new Set<string>()): number {
  let totalDelta = 0
  const ramDeltas: RamDelta[] = [
    ...(trace.account_ram_delta ? [trace.account_ram_delta] : []),
    ...(trace.account_ram_deltas || []),
  ]

  const contract = trace.act?.account || 'unknown'
  const action = trace.act?.name || 'unknown'
  const key = `${contract}:${action}` // Ключ для фильтрации дублей

  // Если RAM дельта = 0, но такой лог еще не был записан — логируем один раз
  if (ramDeltas.length === 0 && !loggedActions.has(key)) {
    console.log(`@none:${contract}:${action} = 0.00 kb`)
    loggedActions.add(key)
  }

  // Логируем всех реальных плательщиков RAM
  for (const { account, delta } of ramDeltas) {
    totalDelta += delta
    const deltaKb = (delta / 1024).toFixed(2)

    console.log(`@${account}:${contract}:${action} = ${deltaKb} kb`)
  }

  // Рекурсия по inline-экшенам
  if (trace.inline_traces?.length) {
    for (const inlineTrace of trace.inline_traces) {
      totalDelta += accumulateRam(inlineTrace, loggedActions)
    }
  }

  return totalDelta
}

export function getTotalRamUsage(result: TransactionTrace): number {
  if (!result?.processed?.action_traces)
    return 0

  let total = 0
  let topLevelActor = 'unknown'
  let topLevelAction = 'unknown'

  for (const topLevelActionTrace of result.processed.action_traces) {
    if (topLevelActor === 'unknown' && topLevelActionTrace.act) {
      topLevelActor = topLevelActionTrace.act.authorization?.[0]?.actor || 'unknown'
      topLevelAction = topLevelActionTrace.act.name
    }

    total += accumulateRam(topLevelActionTrace)
  }

  // Записываем RAM в глобальную статистику
  const key = `${topLevelActor}:${topLevelAction}`
  globalRamStats[key] = (globalRamStats[key] || 0) + total

  console.log(`RAM used by ${key}: ${(total / 1024).toFixed(2)} kb`)
  return total
}
