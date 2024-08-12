export interface Account {
  name: string
  ownerPublicKey: string
  activePublicKey: string
}

export interface Network {
  name: string
  protocol: string
  host: string
  port: string
}

export interface Contract {
  path: string
  name: string
  target: string
}

export interface Feature {
  name: string
  hash: string
}

export interface Keys {
  privateKey: string
  publicKey: string
}
