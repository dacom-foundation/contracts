/* eslint-disable node/prefer-global/process */
import path from 'node:path'
import { config } from 'dotenv'

config()

export default [
  {
    name: 'eosio.boot',
    path: path.resolve(process.cwd(), '../build/contracts/system/contracts/eosio.boot'),
    target: 'eosio',
  },
  {
    name: 'eosio.system',
    path: path.resolve(process.cwd(), '../build/contracts/system/contracts/eosio.system'),
    target: 'eosio',
  },
  {
    name: 'eosio.token',
    path: path.resolve(process.cwd(), '../build/contracts/system/contracts/eosio.token'),
    target: 'eosio.token',
  },
  {
    name: 'eosio.msig',
    path: path.resolve(process.cwd(), '../build/contracts/system/contracts/eosio.msig'),
    target: 'eosio.msig',
  },
  {
    name: 'eosio.wrap',
    path: path.resolve(process.cwd(), '../build/contracts/system/contracts/eosio.wrap'),
    target: 'eosio.wrap',
  },
  {
    name: 'registrator',
    path: path.resolve(process.cwd(), '../build/contracts/registrator'),
    target: 'registrator',
  },
  {
    name: 'soviet',
    path: path.resolve(process.cwd(), '../build/contracts/soviet'),
    target: 'soviet',
  },
  {
    name: 'marketplace',
    path: path.resolve(process.cwd(), '../build/contracts/marketplace'),
    target: 'marketplace',
  },
  {
    name: 'draft',
    path: path.resolve(process.cwd(), '../build/contracts/draft'),
    target: 'draft',
  },
  {
    name: 'branch',
    path: path.resolve(process.cwd(), '../build/contracts/branch'),
    target: 'branch',
  },
  {
    name: 'gateway',
    path: path.resolve(process.cwd(), '../build/contracts/gateway'),
    target: 'gateway',
  },
  {
    name: 'fund',
    path: path.resolve(process.cwd(), '../build/contracts/fund'),
    target: 'fund',
  },
  {
    name: 'contributor',
    path: path.resolve(process.cwd(), '../build/contracts/contributor'),
    target: 'contributor',
  },
]
