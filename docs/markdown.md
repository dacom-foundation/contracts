xml   Compound {
  name: undefined,
  compounds: {},
  members: [],
  basecompoundref: [],
  filtered: {}
} {
  doxygenindex: {
    '$': {
      'xmlns:xsi': 'http://www.w3.org/2001/XMLSchema-instance',
      'xsi:noNamespaceSchemaLocation': 'index.xsd',
      version: '1.9.3',
      'xml:lang': 'ru'
    },
    compound: [
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object], [Object], [Object],
      [Object], [Object], [Object], [Object],
      ... 146 more items
    ]
  }
}
# namespace `eosiobios`



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`class `[``bios``](#classeosiobios_1_1bios)    | 
`struct `[``authority``](#structeosiobios_1_1authority)    | 
`struct `[``block_header``](#structeosiobios_1_1block__header)    | 
`struct `[``key_weight``](#structeosiobios_1_1key__weight)    | 
`struct `[``permission_level_weight``](#structeosiobios_1_1permission__level__weight)    | 
`struct `[``wait_weight``](#structeosiobios_1_1wait__weight)    | 

# class `bios` {#classeosiobios_1_1bios}

```
class bios
  : public eosio::contract
```  



The `eosio.bios` is the first sample of system contract provided by `block.one` through the EOSIO platform. It is a minimalist system contract because it only supplies the actions that are absolutely critical to bootstrap a chain and nothing more. This allows for a chain agnostic approach to bootstrapping a chain.

Just like in the `eosio.system` sample contract implementation, there are a few actions which are not implemented at the contract level (`newaccount`, `updateauth`, `deleteauth`, `linkauth`, `unlinkauth`, `canceldelay`, `onerror`, `setabi`, `setcode`), they are just declared in the contract so they will show in the contract's ABI and users will be able to push those actions to the chain via the account holding the `eosio.system` contract, but the implementation is at the EOSIO core level. They are referred to as EOSIO native actions.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`struct `[``abi_hash``](#structeosiobios_1_1bios_1_1abi__hash)        | 
`public inline void newaccount(name creator,name name,ignore< `[`authority`](#structeosiobios_1_1authority)` > owner,ignore< `[`authority`](#structeosiobios_1_1authority)` > active)` | 
`public inline void updateauth(ignore< name > account,ignore< name > permission,ignore< name > parent,ignore< `[`authority`](#structeosiobios_1_1authority)` > auth)` | 
`public inline void deleteauth(ignore< name > account,ignore< name > permission)` | 
`public inline void linkauth(ignore< name > account,ignore< name > code,ignore< name > type,ignore< name > requirement)` | 
`public inline void unlinkauth(ignore< name > account,ignore< name > code,ignore< name > type)` | 
`public inline void canceldelay(ignore< permission_level > canceling_auth,ignore< checksum256 > trx_id)` | 
`public inline void setcode(name account,uint8_t vmtype,uint8_t vmversion,const std::vector< char > & code)` | 
`public void setabi(name account,const std::vector< char > & abi)` | 
`public void onerror(ignore< uint128_t > sender_id,ignore< std::vector< char > > sent_trx)` | 
`public void setpriv(name account,uint8_t is_priv)` | 
`public void setalimits(name account,int64_t ram_bytes,int64_t net_weight,int64_t cpu_weight)` | 
`public void setprods(const std::vector< eosio::producer_authority > & schedule)` | 
`public void setparams(const eosio::blockchain_parameters & params)` | 
`public void reqauth(name from)` | 
`public void activate(const eosio::checksum256 & feature_digest)` | 
`public void reqactivated(const eosio::checksum256 & feature_digest)` | 

## Members

### `struct `[``abi_hash``](#structeosiobios_1_1bios_1_1abi__hash) {#structeosiobios_1_1bios_1_1abi__hash}




### `public inline void newaccount(name creator,name name,ignore< `[`authority`](#structeosiobios_1_1authority)` > owner,ignore< `[`authority`](#structeosiobios_1_1authority)` > active)` {#classeosiobios_1_1bios_1a289a68eec1095060829ae3f3cedba2b1}



New account action, called after a new account is created. This code enforces resource-limits rules for new accounts as well as new account naming conventions.

* accounts cannot contain '.' symbols which forces all acccounts to be 12 characters long without '.' until a future account auction process is implemented which prevents name squatting.


* new accounts must stake a minimal number of tokens (as set in system parameters) therefore, this method will execute an inline buyram from receiver for newacnt in an amount equal to the current new account creation fee.

### `public inline void updateauth(ignore< name > account,ignore< name > permission,ignore< name > parent,ignore< `[`authority`](#structeosiobios_1_1authority)` > auth)` {#classeosiobios_1_1bios_1a6da029e3f9ffbcb2bfc3cc6f555a3bcd}



Update authorization action updates pemission for an account.


#### Parameters
* `account` - the account for which the permission is updated, 


* `pemission` - the permission name which is updated, 


* `parem` - the parent of the permission which is updated, 


* `aut` - the json describing the permission authorization.

### `public inline void deleteauth(ignore< name > account,ignore< name > permission)` {#classeosiobios_1_1bios_1a496491a411e8263ae0d91179526c25a6}



Delete authorization action deletes the authorization for an account's permission.


#### Parameters
* `account` - the account for which the permission authorization is deleted, 


* `permission` - the permission name been deleted.

### `public inline void linkauth(ignore< name > account,ignore< name > code,ignore< name > type,ignore< name > requirement)` {#classeosiobios_1_1bios_1ac88e1fbb2bf9f23fcf351ab4f02bcf23}



Link authorization action assigns a specific action from a contract to a permission you have created. Five system actions can not be linked `updateauth`, `deleteauth`, `linkauth`, `unlinkauth`, and `canceldelay`. This is useful because when doing authorization checks, the EOSIO based blockchain starts with the action needed to be authorized (and the contract belonging to), and looks up which permission is needed to pass authorization validation. If a link is set, that permission is used for authoraization validation otherwise then active is the default, with the exception of `eosio.any`. `eosio.any` is an implicit permission which exists on every account; you can link actions to `eosio.any` and that will make it so linked actions are accessible to any permissions defined for the account.


#### Parameters
* `account` - the permission's owner to be linked and the payer of the RAM needed to store this link, 


* `code` - the owner of the action to be linked, 


* `type` - the action to be linked, 


* `requirement` - the permission to be linked.

### `public inline void unlinkauth(ignore< name > account,ignore< name > code,ignore< name > type)` {#classeosiobios_1_1bios_1a754f09c20691df6b7292dad22b3a80c9}



Unlink authorization action it's doing the reverse of linkauth action, by unlinking the given action.


#### Parameters
* `account` - the owner of the permission to be unlinked and the receiver of the freed RAM, 


* `code` - the owner of the action to be unlinked, 


* `type` - the action to be unlinked.

### `public inline void canceldelay(ignore< permission_level > canceling_auth,ignore< checksum256 > trx_id)` {#classeosiobios_1_1bios_1a7b70dcae7153a74de611cc78e59b0352}



Cancel delay action cancels a deferred transaction.


#### Parameters
* `canceling_auth` - the permission that authorizes this action, 


* `trx_id` - the deferred transaction id to be cancelled.

### `public inline void setcode(name account,uint8_t vmtype,uint8_t vmversion,const std::vector< char > & code)` {#classeosiobios_1_1bios_1afb059bb5862ce9b0cda885bc130a52e3}



Set code action sets the contract code for an account.


#### Parameters
* `account` - the account for which to set the contract code. 


* `vmtype` - reserved, set it to zero. 


* `vmversion` - reserved, set it to zero. 


* `code` - the code content to be set, in the form of a blob binary..

### `public void setabi(name account,const std::vector< char > & abi)` {#classeosiobios_1_1bios_1a0663bbf0563a9ad67960ccbae6222911}



Set abi action sets the abi for contract identified by `account` name. Creates an entry in the abi_hash_table index, with `account` name as key, if it is not already present and sets its value with the abi hash. Otherwise it is updating the current abi hash value for the existing `account` key.


#### Parameters
* `account` - the name of the account to set the abi for 


* `abi` - the abi hash represented as a vector of characters

### `public void onerror(ignore< uint128_t > sender_id,ignore< std::vector< char > > sent_trx)` {#classeosiobios_1_1bios_1a317235869c18964d6d9425eefa533098}



On error action, notification of this action is delivered to the sender of a deferred transaction when an objective error occurs while executing the deferred transaction. This action is not meant to be called directly.


#### Parameters
* `sender_id` - the id for the deferred transaction chosen by the sender, 


* `sent_trx` - the deferred transaction that failed.

### `public void setpriv(name account,uint8_t is_priv)` {#classeosiobios_1_1bios_1ae5de33639414c0c00e090a2ca7418c69}



Set privilege action allows to set privilege status for an account (turn it on/off). 
#### Parameters
* `account` - the account to set the privileged status for. 


* `is_priv` - 0 for false, > 0 for true.

### `public void setalimits(name account,int64_t ram_bytes,int64_t net_weight,int64_t cpu_weight)` {#classeosiobios_1_1bios_1ac7a64a2f4cf193136957f5be5aeca85b}



Sets the resource limits of an account


#### Parameters
* `account` - name of the account whose resource limit to be set 


* `ram_bytes` - ram limit in absolute bytes 


* `net_weight` - fractionally proportionate net limit of available resources based on (weight / total_weight_of_all_accounts) 


* `cpu_weight` - fractionally proportionate cpu limit of available resources based on (weight / total_weight_of_all_accounts)

### `public void setprods(const std::vector< eosio::producer_authority > & schedule)` {#classeosiobios_1_1bios_1aac48a883107aeacbd1b567a2e3fbad12}



Set producers action, sets a new list of active producers, by proposing a schedule change, once the block that contains the proposal becomes irreversible, the schedule is promoted to "pending" automatically. Once the block that promotes the schedule is irreversible, the schedule will become "active".


#### Parameters
* `schedule` - New list of active producers to set

### `public void setparams(const eosio::blockchain_parameters & params)` {#classeosiobios_1_1bios_1ad4047afa380b9eda3ecc232d89bf05a7}



Set params action, sets the blockchain parameters. By tuning these parameters, various degrees of customization can be achieved.


#### Parameters
* `params` - New blockchain parameters to set

### `public void reqauth(name from)` {#classeosiobios_1_1bios_1abfefcfb3d6b2b221052a6e017de18c56}



Require authorization action, checks if the account name `from` passed in as param has authorization to access current action, that is, if it is listed in the action’s allowed permissions vector.


#### Parameters
* `from` - the account name to authorize

### `public void activate(const eosio::checksum256 & feature_digest)` {#classeosiobios_1_1bios_1ad642c358d927b33340654d59f9dd0df9}



Activate action, activates a protocol feature


#### Parameters
* `feature_digest` - hash of the protocol feature to activate.

### `public void reqactivated(const eosio::checksum256 & feature_digest)` {#classeosiobios_1_1bios_1a3f7b5f24b41381cde54901b02ee1793d}



Require activated action, asserts that a protocol feature has been activated


#### Parameters
* `feature_digest` - hash of the protocol feature to check for activation.


# struct `abi_hash` {#structeosiobios_1_1bios_1_1abi__hash}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public name owner` | 
`public checksum256 hash` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public name owner` {#structeosiobios_1_1bios_1_1abi__hash_1a9192f39fbb92a8ef905d1f28056e904d}





### `public checksum256 hash` {#structeosiobios_1_1bios_1_1abi__hash_1aa92ab376c71ad111a260a5b566207656}





### `public inline uint64_t primary_key() const` {#structeosiobios_1_1bios_1_1abi__hash_1aee318ae24137cc1a7c1db705957d9e52}






# struct `authority` {#structeosiobios_1_1authority}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint32_t threshold` | 
`public std::vector< `[`key_weight`](#structeosiobios_1_1key__weight)` > keys` | 
`public std::vector< `[`permission_level_weight`](#structeosiobios_1_1permission__level__weight)` > accounts` | 
`public std::vector< `[`wait_weight`](#structeosiobios_1_1wait__weight)` > waits` | 

## Members

### `public uint32_t threshold` {#structeosiobios_1_1authority_1a39fdbcf135683889becbb79b7f494df0}





### `public std::vector< `[`key_weight`](#structeosiobios_1_1key__weight)` > keys` {#structeosiobios_1_1authority_1ab3a3f653907a5490ab96ac7605198f2c}





### `public std::vector< `[`permission_level_weight`](#structeosiobios_1_1permission__level__weight)` > accounts` {#structeosiobios_1_1authority_1a1f834bcacfd6abd1d2f7aa389990cf5a}





### `public std::vector< `[`wait_weight`](#structeosiobios_1_1wait__weight)` > waits` {#structeosiobios_1_1authority_1a674014ea85ff0ee9d4dcd9a3d1922d61}






# struct `block_header` {#structeosiobios_1_1block__header}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint32_t timestamp` | 
`public name producer` | 
`public uint16_t confirmed` | 
`public checksum256 previous` | 
`public checksum256 transaction_mroot` | 
`public checksum256 action_mroot` | 
`public uint32_t schedule_version` | 
`public std::optional< eosio::producer_schedule > new_producers` | 

## Members

### `public uint32_t timestamp` {#structeosiobios_1_1block__header_1ac25070170e000a8427f2e96d0e3fd7b0}





### `public name producer` {#structeosiobios_1_1block__header_1a0c794f15d473ccc576dc403676689536}





### `public uint16_t confirmed` {#structeosiobios_1_1block__header_1ad0e00f2357c447a2001b9192849aa839}





### `public checksum256 previous` {#structeosiobios_1_1block__header_1aae1cc92b2256891adbd2539efa99cc34}





### `public checksum256 transaction_mroot` {#structeosiobios_1_1block__header_1adb40a37bf4327f68e081a07cc2abe5c8}





### `public checksum256 action_mroot` {#structeosiobios_1_1block__header_1af1c275fdf2db888deff6db96514dfbd8}





### `public uint32_t schedule_version` {#structeosiobios_1_1block__header_1af34fd0dd085bf489c10b02be5f4dd209}





### `public std::optional< eosio::producer_schedule > new_producers` {#structeosiobios_1_1block__header_1a75189b8a7b99f93b14f9b775bf343396}






# struct `key_weight` {#structeosiobios_1_1key__weight}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::public_key key` | 
`public uint16_t weight` | 

## Members

### `public eosio::public_key key` {#structeosiobios_1_1key__weight_1ac2a44ff06f540ec05005ea808278b398}





### `public uint16_t weight` {#structeosiobios_1_1key__weight_1a712f1942b6ad8deec868c03a156e609c}






# struct `permission_level_weight` {#structeosiobios_1_1permission__level__weight}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public permission_level permission` | 
`public uint16_t weight` | 

## Members

### `public permission_level permission` {#structeosiobios_1_1permission__level__weight_1a541c2041fbf4a2847144a156531ed3bb}





### `public uint16_t weight` {#structeosiobios_1_1permission__level__weight_1ad0a6f4cb52b93d3673b1fc22486880fa}






# struct `wait_weight` {#structeosiobios_1_1wait__weight}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint32_t wait_sec` | 
`public uint16_t weight` | 

## Members

### `public uint32_t wait_sec` {#structeosiobios_1_1wait__weight_1ad5cbfb9e4e061f20ad06134ff7c7a8e9}





### `public uint16_t weight` {#structeosiobios_1_1wait__weight_1a9a3d73af3abe1105cbb23ea8cbdecc5c}






# namespace `eosiosystem`



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`namespace `[``block_info``](#namespaceeosiosystem_1_1block__info)    | 
`class `[``system_contract``](#classeosiosystem_1_1system__contract)    | 
`class `[``native``](#classeosiosystem_1_1native)    | 
`struct `[``abi_hash``](#structeosiosystem_1_1abi__hash)    | 
`struct `[``authority``](#structeosiosystem_1_1authority)    | 
`struct `[``bid_refund``](#structeosiosystem_1_1bid__refund)    | 
`struct `[``block_header``](#structeosiosystem_1_1block__header)    | 
`struct `[``exchange_state``](#structeosiosystem_1_1exchange__state)    | 
`struct `[``delegated_bandwidth``](#structeosiosystem_1_1delegated__bandwidth)    | 
`struct `[``emission_state``](#structeosiosystem_1_1emission__state)    | 
`struct `[``eosio_global_state``](#structeosiosystem_1_1eosio__global__state)    | 
`struct `[``eosio_global_state2``](#structeosiosystem_1_1eosio__global__state2)    | 
`struct `[``eosio_global_state3``](#structeosiosystem_1_1eosio__global__state3)    | 
`struct `[``eosio_global_state4``](#structeosiosystem_1_1eosio__global__state4)    | 
`struct `[``key_weight``](#structeosiosystem_1_1key__weight)    | 
`struct `[``limit_auth_change``](#structeosiosystem_1_1limit__auth__change)    | 
`struct `[``name_bid``](#structeosiosystem_1_1name__bid)    | 
`struct `[``pair_time_point_sec_int64``](#structeosiosystem_1_1pair__time__point__sec__int64)    | 
`struct `[``permission_level_weight``](#structeosiosystem_1_1permission__level__weight)    | 
`struct `[``powerup_config``](#structeosiosystem_1_1powerup__config)    | 
`struct `[``powerup_order``](#structeosiosystem_1_1powerup__order)    | 
`struct `[``powerup_state``](#structeosiosystem_1_1powerup__state)    | 
`struct `[``powerup_state_resource``](#structeosiosystem_1_1powerup__state__resource)    | 
`struct `[``producer_info``](#structeosiosystem_1_1producer__info)    | 
`struct `[``producer_info2``](#structeosiosystem_1_1producer__info2)    | 
`struct `[``ram_debt_record``](#structeosiosystem_1_1ram__debt__record)    | 
`struct `[``refund_request``](#structeosiosystem_1_1refund__request)    | 
`struct `[``rex_balance``](#structeosiosystem_1_1rex__balance)    | 
`struct `[``rex_fund``](#structeosiosystem_1_1rex__fund)    | 
`struct `[``rex_loan``](#structeosiosystem_1_1rex__loan)    | 
`struct `[``rex_order``](#structeosiosystem_1_1rex__order)    | 
`struct `[``rex_order_outcome``](#structeosiosystem_1_1rex__order__outcome)    | 
`struct `[``rex_pool``](#structeosiosystem_1_1rex__pool)    | 
`struct `[``rex_return_buckets``](#structeosiosystem_1_1rex__return__buckets)    | 
`struct `[``rex_return_pool``](#structeosiosystem_1_1rex__return__pool)    | 
`struct `[``user_resources``](#structeosiosystem_1_1user__resources)    | 
`struct `[``voter_info``](#structeosiosystem_1_1voter__info)    | 
`struct `[``wait_weight``](#structeosiosystem_1_1wait__weight)    | 

# namespace `block_info`



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`struct `[``block_batch_info``](#structeosiosystem_1_1block__info_1_1block__batch__info)    | 
`struct `[``block_info_record``](#structeosiosystem_1_1block__info_1_1block__info__record)    | 
`struct `[``latest_block_batch_info_result``](#structeosiosystem_1_1block__info_1_1latest__block__batch__info__result)    | 

# struct `block_batch_info` {#structeosiosystem_1_1block__info_1_1block__batch__info}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint32_t batch_start_height` | 
`public eosio::time_point batch_start_timestamp` | 
`public uint32_t batch_current_end_height` | 
`public eosio::time_point batch_current_end_timestamp` | 

## Members

### `public uint32_t batch_start_height` {#structeosiosystem_1_1block__info_1_1block__batch__info_1aabf1478c4a361b8fc044f77c4617e32b}





### `public eosio::time_point batch_start_timestamp` {#structeosiosystem_1_1block__info_1_1block__batch__info_1af7f9f6a56f32f91ec80622736fe86478}





### `public uint32_t batch_current_end_height` {#structeosiosystem_1_1block__info_1_1block__batch__info_1a54af69850d006e2a43fdea538f5c0695}





### `public eosio::time_point batch_current_end_timestamp` {#structeosiosystem_1_1block__info_1_1block__batch__info_1aef63da736d0ed2568917a4e9aeb44931}






# struct `block_info_record` {#structeosiosystem_1_1block__info_1_1block__info__record}




The blockinfo table holds a rolling window of records containing information for recent blocks.

Each record stores the height and timestamp of the correspond block. A record is added for a new block through the onblock action. The onblock action also erases up to two old records at a time in an attempt to keep the table consisting of only records for blocks going back a particular block height difference backward from the most recent block. Currently that block height difference is hardcoded to 10.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint8_t version` | 
`public uint32_t block_height` | 
`public eosio::time_point block_timestamp` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint8_t version` {#structeosiosystem_1_1block__info_1_1block__info__record_1a9561691091de21a997db569d84d53a0d}





### `public uint32_t block_height` {#structeosiosystem_1_1block__info_1_1block__info__record_1a7e0b0f4b78c5dcdff23495596e7cb488}





### `public eosio::time_point block_timestamp` {#structeosiosystem_1_1block__info_1_1block__info__record_1afaa04cfe4be674be7f3d4d497e90c072}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1block__info_1_1block__info__record_1abbe8ecea1c14ab583d956b76a9be0407}






# struct `latest_block_batch_info_result` {#structeosiosystem_1_1block__info_1_1latest__block__batch__info__result}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public std::optional< `[`block_batch_info`](#structeosiosystem_1_1block__info_1_1block__batch__info)` > result` | 
`public `[`error_code_enum`](#structeosiosystem_1_1block__info_1_1latest__block__batch__info__result_1aae67f275b3852dbc62dcfbc8be6e7e8c)` error_code` | 

## Members

### `public std::optional< `[`block_batch_info`](#structeosiosystem_1_1block__info_1_1block__batch__info)` > result` {#structeosiosystem_1_1block__info_1_1latest__block__batch__info__result_1ad6bb99259a46f73549ee37819affcc1a}





### `public `[`error_code_enum`](#structeosiosystem_1_1block__info_1_1latest__block__batch__info__result_1aae67f275b3852dbc62dcfbc8be6e7e8c)` error_code` {#structeosiosystem_1_1block__info_1_1latest__block__batch__info__result_1ae64b5eb7efe7ddafb9aa3e260dc4fa8e}






# class `system_contract` {#classeosiosystem_1_1system__contract}

```
class system_contract
  : public eosiosystem::native
```  



The `eosio.system` smart contract is provided by `block.one` as a sample system contract, and it defines the structures and actions needed for blockchain's core functionality.

Just like in the `eosio.bios` sample contract implementation, there are a few actions which are not implemented at the contract level (`newaccount`, `updateauth`, `deleteauth`, `linkauth`, `unlinkauth`, `canceldelay`, `onerror`, `setabi`, `setcode`), they are just declared in the contract so they will show in the contract's ABI and users will be able to push those actions to the chain via the account holding the `eosio.system` contract, but the implementation is at the EOSIO core level. They are referred to as EOSIO native actions.



* Users can stake tokens for CPU and Network bandwidth, and then vote for producers or delegate their vote to a proxy.


* Producers register in order to be voted for, and can claim per-block and per-vote rewards.


* Users can buy and sell RAM at a market-determined price.


* Users can bid on premium names.


* A resource exchange system (REX) allows token holders to lend their tokens, and users to rent CPU and Network resources in return for a market-determined fee.


* A resource market separate from REX: `power`.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`class `[``registration``](#classeosiosystem_1_1system__contract_1_1registration)        | 
`public  system_contract(name s,name code,datastream< const char * > ds)` | 
`public  ~system_contract()` | 
`public void init(unsigned_int version,const symbol & core)` | 
`public void initemission(eosio::asset init_supply,uint64_t tact_duration,double emission_factor)` | 
`public void onblock(ignore< `[`block_header`](#structeosiosystem_1_1block__header)` > header)` | 
`public void setalimits(const name & account,int64_t ram_bytes,int64_t net_weight,int64_t cpu_weight)` | 
`public void setacctram(const name & account,const std::optional< int64_t > & ram_bytes)` | 
`public void setacctnet(const name & account,const std::optional< int64_t > & net_weight)` | 
`public void setacctcpu(const name & account,const std::optional< int64_t > & cpu_weight)` | 
`public void activate(const eosio::checksum256 & feature_digest)` | 
`public void delegatebw(const name & from,const name & receiver,const asset & stake_net_quantity,const asset & stake_cpu_quantity,bool transfer)` | 
`public void setrex(const asset & balance)` | 
`public void deposit(const name & owner,const asset & amount)` | 
`public void withdraw(const name & owner,const asset & amount)` | 
`public void buyrex(const name & from,const asset & amount)` | 
`public void unstaketorex(const name & owner,const name & receiver,const asset & from_net,const asset & from_cpu)` | 
`public void sellrex(const name & from,const asset & rex)` | 
`public void cnclrexorder(const name & owner)` | 
`public void rentcpu(const name & from,const name & receiver,const asset & loan_payment,const asset & loan_fund)` | 
`public void rentnet(const name & from,const name & receiver,const asset & loan_payment,const asset & loan_fund)` | 
`public void fundcpuloan(const name & from,uint64_t loan_num,const asset & payment)` | 
`public void fundnetloan(const name & from,uint64_t loan_num,const asset & payment)` | 
`public void defcpuloan(const name & from,uint64_t loan_num,const asset & amount)` | 
`public void defnetloan(const name & from,uint64_t loan_num,const asset & amount)` | 
`public void updaterex(const name & owner)` | 
`public void rexexec(const name & user,uint16_t max)` | 
`public void consolidate(const name & owner)` | 
`public void mvtosavings(const name & owner,const asset & rex)` | 
`public void mvfrsavings(const name & owner,const asset & rex)` | 
`public void closerex(const name & owner)` | 
`public void undelegatebw(const name & from,const name & receiver,const asset & unstake_net_quantity,const asset & unstake_cpu_quantity)` | 
`public void buyram(const name & payer,const name & receiver,const asset & quant)` | 
`public void sellram(const name & account,int64_t bytes)` | 
`public void buyrambytes(const name & payer,const name & receiver,uint32_t bytes)` | 
`public void refund(const name & owner)` | 
`public void regproducer(const name & producer,const public_key & producer_key,const std::string & url,uint16_t location)` | 
`public void regproducer2(const name & producer,const eosio::block_signing_authority & producer_authority,const std::string & url,uint16_t location)` | 
`public void unregprod(const name & producer)` | 
`public void setram(uint64_t max_ram_size)` | 
`public void setramrate(uint16_t bytes_per_block)` | 
`public void voteproducer(const name & voter,const name & proxy,const std::vector< name > & producers)` | 
`public void voteupdate(const name & voter_name)` | 
`public void regproxy(const name & proxy,bool isproxy)` | 
`public void setparams(const `[`blockchain_parameters_t`](#namespaceeosiosystem_1a5e1fbfc9229b1616bfc70a519e26e9c2)` & params)` | 
`public void claimrewards(const name & owner)` | 
`public void setpriv(const name & account,uint8_t is_priv)` | 
`public void rmvproducer(const name & producer)` | 
`public void updtrevision(uint8_t revision)` | 
`public void bidname(const name & bidder,const name & newname,const asset & bid)` | 
`public void bidrefund(const name & bidder,const name & newname)` | 
`public void setinflation(int64_t annual_rate,int64_t inflation_pay_factor,int64_t votepay_factor)` | 
`public void cfgpowerup(`[`powerup_config`](#structeosiosystem_1_1powerup__config)` & args)` | 
`public void powerupexec(const name & user,uint16_t max)` | 
`public void powerup(const name & payer,const name & receiver,uint32_t days,const asset & payment,const bool transfer)` | 
`public void limitauthchg(const name & account,const std::vector< name > & allow_perms,const std::vector< name > & disallow_perms)` | 

## Members

### `class `[``registration``](#classeosiosystem_1_1system__contract_1_1registration) {#classeosiosystem_1_1system__contract_1_1registration}




### `public  system_contract(name s,name code,datastream< const char * > ds)` {#classeosiosystem_1_1system__contract_1ae1a0434a7adf53cc3a168f1b2f947ebd}





### `public  ~system_contract()` {#classeosiosystem_1_1system__contract_1ae0264c3f2d2a640d9a60a4c9c9ca089c}





### `public void init(unsigned_int version,const symbol & core)` {#classeosiosystem_1_1system__contract_1a63afe62bd82d52230be1e0ded1bdb644}



The Init action initializes the system contract for a version and a symbol. Only succeeds when:

* version is 0 and


* symbol is found and


* system token supply is greater than 0,


* and system contract wasn’t already been initialized.






#### Parameters
* `version` - the version, has to be 0, 


* `core` - the system symbol.

### `public void initemission(eosio::asset init_supply,uint64_t tact_duration,double emission_factor)` {#classeosiosystem_1_1system__contract_1af6ed697e2d198ca5bce29cbacb682490}



Инициализация тактовой эмиссии 
#### Parameters
* `tact_duration` - продолжительность такта в секундах, 


* `emission_factor` - множитель эмиссии от 0 до 2.618.

### `public void onblock(ignore< `[`block_header`](#structeosiosystem_1_1block__header)` > header)` {#classeosiosystem_1_1system__contract_1ae75458e3f898b3bc834311fa54abc6a0}



On block action. This special action is triggered when a block is applied by the given producer and cannot be generated from any other source. It is used to pay producers and calculate missed blocks of other producers. Producer pay is deposited into the producer's stake balance and can be withdrawn over time. Once a minute, it may update the active producer config from the producer votes. The action also populates the blockinfo table.


#### Parameters
* `header` - the block header produced.

### `public void setalimits(const name & account,int64_t ram_bytes,int64_t net_weight,int64_t cpu_weight)` {#classeosiosystem_1_1system__contract_1ad5b9178dfbe60ba6059c52d488ecf303}



Set account limits action sets the resource limits of an account


#### Parameters
* `account` - name of the account whose resource limit to be set, 


* `ram_bytes` - ram limit in absolute bytes, 


* `net_weight` - fractionally proportionate net limit of available resources based on (weight / total_weight_of_all_accounts), 


* `cpu_weight` - fractionally proportionate cpu limit of available resources based on (weight / total_weight_of_all_accounts).

### `public void setacctram(const name & account,const std::optional< int64_t > & ram_bytes)` {#classeosiosystem_1_1system__contract_1acdd7c75402f43876338d33649d2e6903}



Set account RAM limits action, which sets the RAM limits of an account


#### Parameters
* `account` - name of the account whose resource limit to be set, 


* `ram_bytes` - ram limit in absolute bytes.

### `public void setacctnet(const name & account,const std::optional< int64_t > & net_weight)` {#classeosiosystem_1_1system__contract_1aa4559928f468c62867916d1251fa33b3}



Set account NET limits action, which sets the NET limits of an account


#### Parameters
* `account` - name of the account whose resource limit to be set, 


* `net_weight` - fractionally proportionate net limit of available resources based on (weight / total_weight_of_all_accounts).

### `public void setacctcpu(const name & account,const std::optional< int64_t > & cpu_weight)` {#classeosiosystem_1_1system__contract_1a9a005d230746b68cafa934a067f287af}



Set account CPU limits action, which sets the CPU limits of an account


#### Parameters
* `account` - name of the account whose resource limit to be set, 


* `cpu_weight` - fractionally proportionate cpu limit of available resources based on (weight / total_weight_of_all_accounts).

### `public void activate(const eosio::checksum256 & feature_digest)` {#classeosiosystem_1_1system__contract_1a1873247483b285e19010b7469419a60d}



The activate action, activates a protocol feature


#### Parameters
* `feature_digest` - hash of the protocol feature to activate.

### `public void delegatebw(const name & from,const name & receiver,const asset & stake_net_quantity,const asset & stake_cpu_quantity,bool transfer)` {#classeosiosystem_1_1system__contract_1aa54b498595abb1a3ad9c62c597d4f7b5}



Delegate bandwidth and/or cpu action. Stakes SYS from the balance of `from` for the benefit of `receiver`.


#### Parameters
* `from` - the account to delegate bandwidth from, that is, the account holding tokens to be staked, 


* `receiver` - the account to delegate bandwidth to, that is, the account to whose resources staked tokens are added 


* `stake_net_quantity` - tokens staked for NET bandwidth, 


* `stake_cpu_quantity` - tokens staked for CPU bandwidth, 


* `transfer` - if true, ownership of staked tokens is transferred to `receiver`.




All producers `from` account has voted for will have their votes updated immediately.

### `public void setrex(const asset & balance)` {#classeosiosystem_1_1system__contract_1a1b875a922b7ab8079b742eb8e714ae48}



Setrex action, sets total_rent balance of REX pool to the passed value. 
#### Parameters
* `balance` - amount to set the REX pool balance.

### `public void deposit(const name & owner,const asset & amount)` {#classeosiosystem_1_1system__contract_1a99e6b1b401c6a8a023f61ed2b52e98d7}



Deposit to REX fund action. Deposits core tokens to user REX fund. All proceeds and expenses related to REX are added to or taken out of this fund. An inline transfer from 'owner' liquid balance is executed. All REX-related costs and proceeds are deducted from and added to 'owner' REX fund, with one exception being buying REX using staked tokens. Storage change is billed to 'owner'.


#### Parameters
* `owner` - REX fund owner account, 


* `amount` - amount of tokens to be deposited.

### `public void withdraw(const name & owner,const asset & amount)` {#classeosiosystem_1_1system__contract_1a45eea3c4ce35174eb1c3ad1e64954d5e}



Withdraw from REX fund action, withdraws core tokens from user REX fund. An inline token transfer to user balance is executed.


#### Parameters
* `owner` - REX fund owner account, 


* `amount` - amount of tokens to be withdrawn.

### `public void buyrex(const name & from,const asset & amount)` {#classeosiosystem_1_1system__contract_1a36aebc85b90ecf7e5389f8daea64592a}



Buyrex action, buys REX in exchange for tokens taken out of user's REX fund by transferring core tokens from user REX fund and converts them to REX stake. By buying REX, user is lending tokens in order to be rented as CPU or NET resources. Storage change is billed to 'from' account.


#### Parameters
* `from` - owner account name, 


* `amount` - amount of tokens taken out of 'from' REX fund.




A voting requirement must be satisfied before action can be executed. 

User must vote for at least 21 producers or delegate vote to proxy before buying REX.

User votes are updated following this action. 

Tokens used in purchase are added to user's voting power. 

Bought REX cannot be sold before 4 days counting from end of day of purchase.

### `public void unstaketorex(const name & owner,const name & receiver,const asset & from_net,const asset & from_cpu)` {#classeosiosystem_1_1system__contract_1a94675167323c97d0dcdc92c5b7f0df60}



Unstaketorex action, uses staked core tokens to buy REX. Storage change is billed to 'owner' account.


#### Parameters
* `owner` - owner of staked tokens, 


* `receiver` - account name that tokens have previously been staked to, 


* `from_net` - amount of tokens to be unstaked from NET bandwidth and used for REX purchase, 


* `from_cpu` - amount of tokens to be unstaked from CPU bandwidth and used for REX purchase.




A voting requirement must be satisfied before action can be executed. 

User must vote for at least 21 producers or delegate vote to proxy before buying REX.

User votes are updated following this action. 

Tokens used in purchase are added to user's voting power. 

Bought REX cannot be sold before 4 days counting from end of day of purchase.

### `public void sellrex(const name & from,const asset & rex)` {#classeosiosystem_1_1system__contract_1a9ee2d79b7e9c1432f312535936e4848f}



Sellrex action, sells REX in exchange for core tokens by converting REX stake back into core tokens at current exchange rate. If order cannot be processed, it gets queued until there is enough in REX pool to fill order, and will be processed within 30 days at most. If successful, user votes are updated, that is, proceeds are deducted from user's voting power. In case sell order is queued, storage change is billed to 'from' account.


#### Parameters
* `from` - owner account of REX, 


* `rex` - amount of REX to be sold.

### `public void cnclrexorder(const name & owner)` {#classeosiosystem_1_1system__contract_1ac4bc1aa3fe5d72c9b1ed8af258c9f935}



Cnclrexorder action, cancels unfilled REX sell order by owner if one exists.


#### Parameters
* `owner` - owner account name.




Order cannot be cancelled once it's been filled.

### `public void rentcpu(const name & from,const name & receiver,const asset & loan_payment,const asset & loan_fund)` {#classeosiosystem_1_1system__contract_1a2babe24ecc07b5576ce48c0b7220e7f4}



Rentcpu action, uses payment to rent as many SYS tokens as possible as determined by market price and stake them for CPU for the benefit of receiver, after 30 days the rented core delegation of CPU will expire. At expiration, if balance is greater than or equal to `loan_payment`, `loan_payment` is taken out of loan balance and used to renew the loan. Otherwise, the loan is closed and user is refunded any remaining balance. Owner can fund or refund a loan at any time before its expiration. All loan expenses and refunds come out of or are added to owner's REX fund.


#### Parameters
* `from` - account creating and paying for CPU loan, 'from' account can add tokens to loan balance using action `fundcpuloan` and withdraw from loan balance using `defcpuloan`


* `receiver` - account receiving rented CPU resources, 


* `loan_payment` - tokens paid for the loan, it has to be greater than zero, amount of rented resources is calculated from `loan_payment`, 


* `loan_fund` - additional tokens can be zero, and is added to loan balance. Loan balance represents a reserve that is used at expiration for automatic loan renewal.

### `public void rentnet(const name & from,const name & receiver,const asset & loan_payment,const asset & loan_fund)` {#classeosiosystem_1_1system__contract_1a89b5465c60b08ecb06538ea162751319}



Rentnet action, uses payment to rent as many SYS tokens as possible as determined by market price and stake them for NET for the benefit of receiver, after 30 days the rented core delegation of NET will expire. At expiration, if balance is greater than or equal to `loan_payment`, `loan_payment` is taken out of loan balance and used to renew the loan. Otherwise, the loan is closed and user is refunded any remaining balance. Owner can fund or refund a loan at any time before its expiration. All loan expenses and refunds come out of or are added to owner's REX fund.


#### Parameters
* `from` - account creating and paying for NET loan, 'from' account can add tokens to loan balance using action `fundnetloan` and withdraw from loan balance using `defnetloan`, 


* `receiver` - account receiving rented NET resources, 


* `loan_payment` - tokens paid for the loan, it has to be greater than zero, amount of rented resources is calculated from `loan_payment`, 


* `loan_fund` - additional tokens can be zero, and is added to loan balance. Loan balance represents a reserve that is used at expiration for automatic loan renewal.

### `public void fundcpuloan(const name & from,uint64_t loan_num,const asset & payment)` {#classeosiosystem_1_1system__contract_1aed60ee281d5277819ed1335322b2d53f}



Fundcpuloan action, transfers tokens from REX fund to the fund of a specific CPU loan in order to be used for loan renewal at expiry.


#### Parameters
* `from` - loan creator account, 


* `loan_num` - loan id, 


* `payment` - tokens transferred from REX fund to loan fund.

### `public void fundnetloan(const name & from,uint64_t loan_num,const asset & payment)` {#classeosiosystem_1_1system__contract_1a8e99bf65da50dc92089140618112db27}



Fundnetloan action, transfers tokens from REX fund to the fund of a specific NET loan in order to be used for loan renewal at expiry.


#### Parameters
* `from` - loan creator account, 


* `loan_num` - loan id, 


* `payment` - tokens transferred from REX fund to loan fund.

### `public void defcpuloan(const name & from,uint64_t loan_num,const asset & amount)` {#classeosiosystem_1_1system__contract_1a94e2f4698dd0ad80e4252a772d424481}



Defcpuloan action, withdraws tokens from the fund of a specific CPU loan and adds them to REX fund.


#### Parameters
* `from` - loan creator account, 


* `loan_num` - loan id, 


* `amount` - tokens transferred from CPU loan fund to REX fund.

### `public void defnetloan(const name & from,uint64_t loan_num,const asset & amount)` {#classeosiosystem_1_1system__contract_1a56a54ec0ca8f37271fbcbc9cc5181145}



Defnetloan action, withdraws tokens from the fund of a specific NET loan and adds them to REX fund.


#### Parameters
* `from` - loan creator account, 


* `loan_num` - loan id, 


* `amount` - tokens transferred from NET loan fund to REX fund.

### `public void updaterex(const name & owner)` {#classeosiosystem_1_1system__contract_1a33a33fedadd8b07f8b9fcee22e1a966d}



Updaterex action, updates REX owner vote weight to current value of held REX tokens.


#### Parameters
* `owner` - REX owner account.

### `public void rexexec(const name & user,uint16_t max)` {#classeosiosystem_1_1system__contract_1a21d675936f040b2dcdcebff3765a60a7}



Rexexec action, processes max CPU loans, max NET loans, and max queued sellrex orders. Action does not execute anything related to a specific user.


#### Parameters
* `user` - any account can execute this action, 


* `max` - number of each of CPU loans, NET loans, and sell orders to be processed.

### `public void consolidate(const name & owner)` {#classeosiosystem_1_1system__contract_1ac6c59d9470ffe96336455b92804aa4de}



Consolidate action, consolidates REX maturity buckets into one bucket that can be sold after 4 days starting from the end of the day.


#### Parameters
* `owner` - REX owner account name.

### `public void mvtosavings(const name & owner,const asset & rex)` {#classeosiosystem_1_1system__contract_1ac673c662e873032e4a67b1d888b23e6f}



Mvtosavings action, moves a specified amount of REX into savings bucket. REX savings bucket never matures. In order for it to be sold, it has to be moved explicitly out of that bucket. Then the moved amount will have the regular maturity period of 4 days starting from the end of the day.


#### Parameters
* `owner` - REX owner account name. 


* `rex` - amount of REX to be moved.

### `public void mvfrsavings(const name & owner,const asset & rex)` {#classeosiosystem_1_1system__contract_1a975cbc9b4670f245c76b0e3f3c86136f}



Mvfrsavings action, moves a specified amount of REX out of savings bucket. The moved amount will have the regular REX maturity period of 4 days.


#### Parameters
* `owner` - REX owner account name. 


* `rex` - amount of REX to be moved.

### `public void closerex(const name & owner)` {#classeosiosystem_1_1system__contract_1a0a1b64d68fa08a63ee570a58d45e5626}



Closerex action, deletes owner records from REX tables and frees used RAM. Owner must not have an outstanding REX balance.


#### Parameters
* `owner` - user account name.




If owner has a non-zero REX balance, the action fails; otherwise, owner REX balance entry is deleted. 

If owner has no outstanding loans and a zero REX fund balance, REX fund entry is deleted.

### `public void undelegatebw(const name & from,const name & receiver,const asset & unstake_net_quantity,const asset & unstake_cpu_quantity)` {#classeosiosystem_1_1system__contract_1a4db0be2c90d342ecd269fdbb57c51091}



Undelegate bandwidth action, decreases the total tokens delegated by `from` to `receiver` and/or frees the memory associated with the delegation if there is nothing left to delegate. This will cause an immediate reduction in net/cpu bandwidth of the receiver. A transaction is scheduled to send the tokens back to `from` after the staking period has passed. If existing transaction is scheduled, it will be canceled and a new transaction issued that has the combined undelegated amount. The `from` account loses voting power as a result of this call and all producer tallies are updated.


#### Parameters
* `from` - the account to undelegate bandwidth from, that is, the account whose tokens will be unstaked, 


* `receiver` - the account to undelegate bandwidth to, that is, the account to whose benefit tokens have been staked, 


* `unstake_net_quantity` - tokens to be unstaked from NET bandwidth, 


* `unstake_cpu_quantity` - tokens to be unstaked from CPU bandwidth,




Unstaked tokens are transferred to `from` liquid balance via a deferred transaction with a delay of 3 days. 

If called during the delay period of a previous `undelegatebw` action, pending action is canceled and timer is reset. 

All producers `from` account has voted for will have their votes updated immediately. 

Bandwidth and storage for the deferred transaction are billed to `from`.

### `public void buyram(const name & payer,const name & receiver,const asset & quant)` {#classeosiosystem_1_1system__contract_1a04be77ea83f87d1533606390c37cf5d1}



Buy ram action, increases receiver's ram quota based upon current price and quantity of tokens provided. An inline transfer from receiver to system contract of tokens will be executed.


#### Parameters
* `payer` - the ram buyer, 


* `receiver` - the ram receiver, 


* `quant` - the quantity of tokens to buy ram with.





When buying ram the payer irreversibly transfers quant to system contract and only the receiver may reclaim the tokens via the sellram action. The receiver pays for the storage of all database records associated with this action.

RAM is a scarce resource whose supply is defined by global properties max_ram_size. RAM is priced using the bancor algorithm such that price-per-byte with a constant reserve ratio of 100:1.

### `public void sellram(const name & account,int64_t bytes)` {#classeosiosystem_1_1system__contract_1a6b00b900a128d2718244ac8a408f65d3}





### `public void buyrambytes(const name & payer,const name & receiver,uint32_t bytes)` {#classeosiosystem_1_1system__contract_1a93950629ac6f931b47542727a5411097}



Buy a specific amount of ram bytes action. Increases receiver's ram in quantity of bytes provided. An inline transfer from receiver to system contract of tokens will be executed.


#### Parameters
* `payer` - the ram buyer, 


* `receiver` - the ram receiver, 


* `bytes` - the quantity of ram to buy specified in bytes.





This action will buy an exact amount of ram and bill the payer the current market price.

### `public void refund(const name & owner)` {#classeosiosystem_1_1system__contract_1a10a63e006f9869caeaf2592bdfbccc42}



Refund action, this action is called after the delegation-period to claim all pending unstaked tokens belonging to owner.


#### Parameters
* `owner` - the owner of the tokens claimed.

### `public void regproducer(const name & producer,const public_key & producer_key,const std::string & url,uint16_t location)` {#classeosiosystem_1_1system__contract_1a252441c7b5811068d336f37ab627ecd9}



Register producer action, indicates that a particular account wishes to become a producer, this action will create a `producer_config` and a `[producer_info](#structeosiosystem_1_1producer__info)` object for `producer` scope in producers tables.


#### Parameters
* `producer` - account registering to be a producer candidate, 


* `producer_key` - the public key of the block producer, this is the key used by block producer to sign blocks, 


* `url` - the url of the block producer, normally the url of the block producer presentation website, 


* `location` - is the country code as defined in the ISO 3166, [https://en.wikipedia.org/wiki/List_of_ISO_3166_country_codes](https://en.wikipedia.org/wiki/List_of_ISO_3166_country_codes)




Producer to register is an account 

Authority of producer to register

### `public void regproducer2(const name & producer,const eosio::block_signing_authority & producer_authority,const std::string & url,uint16_t location)` {#classeosiosystem_1_1system__contract_1a6c1143de786b7d7601ac0b0968cb0f19}



Register producer action, indicates that a particular account wishes to become a producer, this action will create a `producer_config` and a `[producer_info](#structeosiosystem_1_1producer__info)` object for `producer` scope in producers tables.


#### Parameters
* `producer` - account registering to be a producer candidate, 


* `producer_authority` - the weighted threshold multisig block signing authority of the block producer used to sign blocks, 


* `url` - the url of the block producer, normally the url of the block producer presentation website, 


* `location` - is the country code as defined in the ISO 3166, [https://en.wikipedia.org/wiki/List_of_ISO_3166_country_codes](https://en.wikipedia.org/wiki/List_of_ISO_3166_country_codes)




Producer to register is an account 

Authority of producer to register

### `public void unregprod(const name & producer)` {#classeosiosystem_1_1system__contract_1a0b8c45d3226929472bc5785742c05152}



Unregister producer action, deactivates the block producer with account name `producer`.

Deactivate the block producer with account name `producer`. 
#### Parameters
* `producer` - the block producer account to unregister.

### `public void setram(uint64_t max_ram_size)` {#classeosiosystem_1_1system__contract_1ad8a6eb47fd97c9f243019d93916ff78f}



Set ram action sets the ram supply. 
#### Parameters
* `max_ram_size` - the amount of ram supply to set.

### `public void setramrate(uint16_t bytes_per_block)` {#classeosiosystem_1_1system__contract_1a7d984add31b6060bb59d472f9f1bd1a1}



Set ram rate action, sets the rate of increase of RAM in bytes per block. It is capped by the uint16_t to a maximum rate of 3 TB per year. If update_ram_supply hasn't been called for the most recent block, then new ram will be allocated at the old rate up to the present block before switching the rate.


#### Parameters
* `bytes_per_block` - the amount of bytes per block increase to set.

### `public void voteproducer(const name & voter,const name & proxy,const std::vector< name > & producers)` {#classeosiosystem_1_1system__contract_1a19d51a2b9b57ffb656612c280ee4ab4e}



Vote producer action, votes for a set of producers. This action updates the list of `producers` voted for, for `voter` account. If voting for a `proxy`, the producer votes will not change until the proxy updates their own vote. Voter can vote for a proxy **or** a list of at most 30 producers. Storage change is billed to `voter`.


#### Parameters
* `voter` - the account to change the voted producers for, 


* `proxy` - the proxy to change the voted producers for, 


* `producers` - the list of producers to vote for, a maximum of 30 producers is allowed.




Producers must be sorted from lowest to highest and must be registered and active 

If proxy is set then no producers can be voted for 

If proxy is set then proxy account must exist and be registered as a proxy 

Every listed producer or proxy must have been previously registered 

Voter must authorize this action 

Voter must have previously staked some EOS for voting 

Voter->staked must be up to date

Every producer previously voted for will have vote reduced by previous vote weight 

Every producer newly voted for will have vote increased by new vote amount 

Prior proxy will proxied_vote_weight decremented by previous vote weight 

New proxy will proxied_vote_weight incremented by new vote weight

### `public void voteupdate(const name & voter_name)` {#classeosiosystem_1_1system__contract_1afd8f9ea7890c647010bc9bccdcda6ba9}



Update the vote weight for the producers or proxy `voter_name` currently votes for. This will also update the `staked` value for the `voter_name` by checking `rexbal` and all delegated NET and CPU.


#### Parameters
* `voter_name` - the account to update the votes for,




the voter.staked will be updated 

previously voted for producers vote weight will be updated with new weight 

previously voted for proxy vote weight will be updated with new weight

### `public void regproxy(const name & proxy,bool isproxy)` {#classeosiosystem_1_1system__contract_1a7192c64e5a9d0b9b202390dbfade0858}



Register proxy action, sets `proxy` account as proxy. An account marked as a proxy can vote with the weight of other accounts which have selected it as a proxy. Other accounts must refresh their voteproducer to update the proxy's weight. Storage change is billed to `proxy`.


#### Parameters
* `proxy` - the account registering as voter proxy (or unregistering), 


* `isproxy` - if true, proxy is registered; if false, proxy is unregistered.




Proxy must have something staked (existing row in voters table) 

New state must be different than current state

### `public void setparams(const `[`blockchain_parameters_t`](#namespaceeosiosystem_1a5e1fbfc9229b1616bfc70a519e26e9c2)` & params)` {#classeosiosystem_1_1system__contract_1aa4ea1b93cfb3d99fcaaabb994c711bee}



Set the blockchain parameters. By tunning these parameters a degree of customization can be achieved. 
#### Parameters
* `params` - New blockchain parameters to set.

### `public void claimrewards(const name & owner)` {#classeosiosystem_1_1system__contract_1a34b35d1f073383e599d4e2bb56e20524}



Claim rewards action, claims block producing and vote rewards. 
#### Parameters
* `owner` - producer account claiming per-block and per-vote rewards.

### `public void setpriv(const name & account,uint8_t is_priv)` {#classeosiosystem_1_1system__contract_1a648ec27a9bc3bf7e36c5c88f6932b9ca}



Set privilege status for an account. Allows to set privilege status for an account (turn it on/off). 
#### Parameters
* `account` - the account to set the privileged status for. 


* `is_priv` - 0 for false, > 0 for true.

### `public void rmvproducer(const name & producer)` {#classeosiosystem_1_1system__contract_1a2a41dc1871059a536a55626b8c76f5b9}



Remove producer action, deactivates a producer by name, if not found asserts. 
#### Parameters
* `producer` - the producer account to deactivate.

### `public void updtrevision(uint8_t revision)` {#classeosiosystem_1_1system__contract_1a0af4a55018801131ae7dc7a923bead0b}



Update revision action, updates the current revision. 
#### Parameters
* `revision` - it has to be incremented by 1 compared with current revision.




Current revision can not be higher than 254, and has to be smaller than or equal 1 (“set upper bound to greatest revision supported in the code”).

### `public void bidname(const name & bidder,const name & newname,const asset & bid)` {#classeosiosystem_1_1system__contract_1af1c329fbf0e3e45b62b729e6ee743c62}



Bid name action, allows an account `bidder` to place a bid for a name `newname`. 
#### Parameters
* `bidder` - the account placing the bid, 


* `newname` - the name the bid is placed for, 


* `bid` - the amount of system tokens payed for the bid.




Bids can be placed only on top-level suffix, 

Non empty name, 

Names longer than 12 chars are not allowed, 

Names equal with 12 chars can be created without placing a bid, 

Bid has to be bigger than zero, 

Bid's symbol must be system token, 

Bidder account has to be different than current highest bidder, 

Bid must increase current bid by 10%, 

Auction must still be opened.

### `public void bidrefund(const name & bidder,const name & newname)` {#classeosiosystem_1_1system__contract_1a1ad9f05ad7242412bf5ad87706f923ff}



Bid refund action, allows the account `bidder` to get back the amount it bid so far on a `newname` name.


#### Parameters
* `bidder` - the account that gets refunded, 


* `newname` - the name for which the bid was placed and now it gets refunded for.

### `public void setinflation(int64_t annual_rate,int64_t inflation_pay_factor,int64_t votepay_factor)` {#classeosiosystem_1_1system__contract_1ac2cab12a3c2a1d20a3215124100cf3e1}



Change the annual inflation rate of the core token supply and specify how the new issued tokens will be distributed based on the following structure.


#### Parameters
* `annual_rate` - Annual inflation rate of the core token supply. (eg. For 5% Annual inflation => annual_rate=500 For 1.5% Annual inflation => annual_rate=150 


* `inflation_pay_factor` - Inverse of the fraction of the inflation used to reward block producers. The remaining inflation will be sent to the `eosio.saving` account. (eg. For 20% of inflation going to block producer rewards => inflation_pay_factor = 50000 For 100% of inflation going to block producer rewards => inflation_pay_factor = 10000). 


* `votepay_factor` - Inverse of the fraction of the block producer rewards to be distributed proportional to blocks produced. The remaining rewards will be distributed proportional to votes received. (eg. For 25% of block producer rewards going towards block pay => votepay_factor = 40000 For 75% of block producer rewards going towards block pay => votepay_factor = 13333).

### `public void cfgpowerup(`[`powerup_config`](#structeosiosystem_1_1powerup__config)` & args)` {#classeosiosystem_1_1system__contract_1ae71212071f747832a2a56eb4868d9c81}



Configure the `power` market. The market becomes available the first time this action is invoked.

### `public void powerupexec(const name & user,uint16_t max)` {#classeosiosystem_1_1system__contract_1aa573d628b237c167aa99fc71cb150736}



Process power queue and update state. Action does not execute anything related to a specific user.


#### Parameters
* `user` - any account can execute this action 


* `max` - number of queue items to process

### `public void powerup(const name & payer,const name & receiver,uint32_t days,const asset & payment,const bool transfer)` {#classeosiosystem_1_1system__contract_1ab3d6d9e77d6b9ac63b79e6d8104d8312}



Powerup NET and CPU resources by percentage


#### Parameters
* `payer` - the resource buyer 


* `receiver` - the resource receiver 


* `days` - number of days of resource availability. Must match market configuration. 


* `net_frac` - fraction of net (100% = 10^15) managed by this market 


* `cpu_frac` - fraction of cpu (100% = 10^15) managed by this market 


* `max_payment` - the maximum amount `payer` is willing to pay. Tokens are withdrawn from `payer`'s token balance.

### `public void limitauthchg(const name & account,const std::vector< name > & allow_perms,const std::vector< name > & disallow_perms)` {#classeosiosystem_1_1system__contract_1ab940f454d4d3d0018f22bf472caf3b52}



limitauthchg opts into or out of restrictions on updateauth, deleteauth, linkauth, and unlinkauth.

If either allow_perms or disallow_perms is non-empty, then opts into restrictions. If allow_perms is non-empty, then the authorized_by argument of the restricted actions must be in the vector, or the actions will abort. If disallow_perms is non-empty, then the authorized_by argument of the restricted actions must not be in the vector, or the actions will abort.

If both allow_perms and disallow_perms are empty, then opts out of the restrictions. limitauthchg aborts if both allow_perms and disallow_perms are non-empty.


#### Parameters
* `account` - account to change 


* `allow_perms` - permissions which may use the restricted actions 


* `disallow_perms` - permissions which may not use the restricted actions


# class `registration` {#classeosiosystem_1_1system__contract_1_1registration}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`struct `[``for_each``](#structeosiosystem_1_1system__contract_1_1registration_1_1for__each)        | 
`struct `[``for_each< P >``](#structeosiosystem_1_1system__contract_1_1registration_1_1for__each_3_01_p_01_4)        | 
`public `[`system_contract`](#classeosiosystem_1_1system__contract)` * this_contract` | 
`public template<typename... Args>`  <br/>`inline constexpr void operator()(Args &&... args)` | 

## Members

### `struct `[``for_each``](#structeosiosystem_1_1system__contract_1_1registration_1_1for__each) {#structeosiosystem_1_1system__contract_1_1registration_1_1for__each}




### `struct `[``for_each< P >``](#structeosiosystem_1_1system__contract_1_1registration_1_1for__each_3_01_p_01_4) {#structeosiosystem_1_1system__contract_1_1registration_1_1for__each_3_01_p_01_4}




### `public `[`system_contract`](#classeosiosystem_1_1system__contract)` * this_contract` {#classeosiosystem_1_1system__contract_1_1registration_1a86e430555979d16ce46d11fbfc4c5673}





### `public template<typename... Args>`  <br/>`inline constexpr void operator()(Args &&... args)` {#classeosiosystem_1_1system__contract_1_1registration_1a766e47b24eb13d3c1f1cc234cbfdb06a}






# struct `for_each` {#structeosiosystem_1_1system__contract_1_1registration_1_1for__each}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------

## Members


# struct `for_each< P >` {#structeosiosystem_1_1system__contract_1_1registration_1_1for__each_3_01_p_01_4}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------

## Members


# class `native` {#classeosiosystem_1_1native}

```
class native
  : public eosio::contract
```  



The EOSIO core `native` contract that governs authorization and contracts' abi.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public void newaccount(const name & creator,const name & name,ignore< `[`authority`](#structeosiosystem_1_1authority)` > owner,ignore< `[`authority`](#structeosiosystem_1_1authority)` > active)` | 
`public inline void updateauth(name account,name permission,name parent,`[`authority`](#structeosiosystem_1_1authority)` auth,binary_extension< name > authorized_by)` | 
`public inline void deleteauth(name account,name permission,binary_extension< name > authorized_by)` | 
`public inline void linkauth(name account,name code,name type,name requirement,binary_extension< name > authorized_by)` | 
`public inline void unlinkauth(name account,name code,name type,binary_extension< name > authorized_by)` | 
`public inline void canceldelay(ignore< permission_level > canceling_auth,ignore< checksum256 > trx_id)` | 
`public void onerror(ignore< uint128_t > sender_id,ignore< std::vector< char > > sent_trx)` | 
`public void setabi(const name & account,const std::vector< char > & abi,const binary_extension< std::string > & memo)` | 
`public inline void setcode(const name & account,uint8_t vmtype,uint8_t vmversion,const std::vector< char > & code,const binary_extension< std::string > & memo)` | 

## Members

### `public void newaccount(const name & creator,const name & name,ignore< `[`authority`](#structeosiosystem_1_1authority)` > owner,ignore< `[`authority`](#structeosiosystem_1_1authority)` > active)` {#classeosiosystem_1_1native_1ab81dda3be834b68780bbae5c2e8368e0}



These actions map one-on-one with the ones defined in core layer of EOSIO, that's where their implementation actually is done. They are present here only so they can show up in the abi file and thus user can send them to this contract, but they have no specific implementation at this contract level, they will execute the implementation at the core layer and nothing else. New account action is called after a new account is created. This code enforces resource-limits rules for new accounts as well as new account naming conventions.

Called after a new account is created. This code enforces resource-limits rules for new accounts as well as new account naming conventions.

Account names containing '.' symbols must have a suffix equal to the name of the creator. This allows users who buy a premium name (shorter than 12 characters with no dots) to be the only ones who can create accounts with the creator's name as a suffix.

### `public inline void updateauth(name account,name permission,name parent,`[`authority`](#structeosiosystem_1_1authority)` auth,binary_extension< name > authorized_by)` {#classeosiosystem_1_1native_1a2bbeca58b24029a5ab0fc90832dde56b}



Update authorization action updates permission for an account.

This contract enforces additional rules:

* If authorized_by is present and not "", then the contract does a require_auth2(account, authorized_by).


* If the account has opted into limitauthchg, then authorized_by must be present and not "".


* If the account has opted into limitauthchg, and allow_perms is not empty, then authorized_by must be in the array.


* If the account has opted into limitauthchg, and disallow_perms is not empty, then authorized_by must not be in the array.





#### Parameters
* `account` - the account for which the permission is updated 


* `permission` - the permission name which is updated 


* `parent` - the parent of the permission which is updated 


* `auth` - the json describing the permission authorization 


* `authorized_by` - the permission which is authorizing this change

### `public inline void deleteauth(name account,name permission,binary_extension< name > authorized_by)` {#classeosiosystem_1_1native_1a49cc638f16ddc898952a8f032ffd6c2c}



Delete authorization action deletes the authorization for an account's permission.

This contract enforces additional rules:

* If authorized_by is present and not "", then the contract does a require_auth2(account, authorized_by).


* If the account has opted into limitauthchg, then authorized_by must be present and not "".


* If the account has opted into limitauthchg, and allow_perms is not empty, then authorized_by must be in the array.


* If the account has opted into limitauthchg, and disallow_perms is not empty, then authorized_by must not be in the array.





#### Parameters
* `account` - the account for which the permission authorization is deleted, 


* `permission` - the permission name been deleted. 


* `authorized_by` - the permission which is authorizing this change

### `public inline void linkauth(name account,name code,name type,name requirement,binary_extension< name > authorized_by)` {#classeosiosystem_1_1native_1a35b93b9e7ae3eaa60619189c7694499e}



Link authorization action assigns a specific action from a contract to a permission you have created. Five system actions can not be linked `updateauth`, `deleteauth`, `linkauth`, `unlinkauth`, and `canceldelay`. This is useful because when doing authorization checks, the EOSIO based blockchain starts with the action needed to be authorized (and the contract belonging to), and looks up which permission is needed to pass authorization validation. If a link is set, that permission is used for authorization validation otherwise then active is the default, with the exception of `eosio.any`. `eosio.any` is an implicit permission which exists on every account; you can link actions to `eosio.any` and that will make it so linked actions are accessible to any permissions defined for the account.

This contract enforces additional rules:

* If authorized_by is present and not "", then the contract does a require_auth2(account, authorized_by).


* If the account has opted into limitauthchg, then authorized_by must be present and not "".


* If the account has opted into limitauthchg, and allow_perms is not empty, then authorized_by must be in the array.


* If the account has opted into limitauthchg, and disallow_perms is not empty, then authorized_by must not be in the array.





#### Parameters
* `account` - the permission's owner to be linked and the payer of the RAM needed to store this link, 


* `code` - the owner of the action to be linked, 


* `type` - the action to be linked, 


* `requirement` - the permission to be linked. 


* `authorized_by` - the permission which is authorizing this change

### `public inline void unlinkauth(name account,name code,name type,binary_extension< name > authorized_by)` {#classeosiosystem_1_1native_1aa1e2097e289d4da3acc2876cc24e8897}



Unlink authorization action it's doing the reverse of linkauth action, by unlinking the given action.

This contract enforces additional rules:

* If authorized_by is present and not "", then the contract does a require_auth2(account, authorized_by).


* If the account has opted into limitauthchg, then authorized_by must be present and not "".


* If the account has opted into limitauthchg, and allow_perms is not empty, then authorized_by must be in the array.


* If the account has opted into limitauthchg, and disallow_perms is not empty, then authorized_by must not be in the array.





#### Parameters
* `account` - the owner of the permission to be unlinked and the receiver of the freed RAM, 


* `code` - the owner of the action to be unlinked, 


* `type` - the action to be unlinked. 


* `authorized_by` - the permission which is authorizing this change

### `public inline void canceldelay(ignore< permission_level > canceling_auth,ignore< checksum256 > trx_id)` {#classeosiosystem_1_1native_1ac2da52c0f6c1698bbe76a84f23917feb}



Cancel delay action cancels a deferred transaction.


#### Parameters
* `canceling_auth` - the permission that authorizes this action, 


* `trx_id` - the deferred transaction id to be cancelled.

### `public void onerror(ignore< uint128_t > sender_id,ignore< std::vector< char > > sent_trx)` {#classeosiosystem_1_1native_1a379fe8f4ca5a42a6011dbf9c8f30b6ff}



On error action, notification of this action is delivered to the sender of a deferred transaction when an objective error occurs while executing the deferred transaction. This action is not meant to be called directly.


#### Parameters
* `sender_id` - the id for the deferred transaction chosen by the sender, 


* `sent_trx` - the deferred transaction that failed.

### `public void setabi(const name & account,const std::vector< char > & abi,const binary_extension< std::string > & memo)` {#classeosiosystem_1_1native_1aff24a543859381c1670183f5b4d343aa}



Set abi action sets the contract abi for an account.


#### Parameters
* `account` - the account for which to set the contract abi. 


* `abi` - the abi content to be set, in the form of a blob binary. 


* `memo` - may be omitted

### `public inline void setcode(const name & account,uint8_t vmtype,uint8_t vmversion,const std::vector< char > & code,const binary_extension< std::string > & memo)` {#classeosiosystem_1_1native_1a225a51ac200eb80061ecbea5980e0b7b}



Set code action sets the contract code for an account.


#### Parameters
* `account` - the account for which to set the contract code. 


* `vmtype` - reserved, set it to zero. 


* `vmversion` - reserved, set it to zero. 


* `code` - the code content to be set, in the form of a blob binary.. 


* `memo` - may be omitted


# struct `abi_hash` {#structeosiosystem_1_1abi__hash}




[abi_hash](#structeosiosystem_1_1abi__hash) is the structure underlying the abihash table and consists of:

* `owner`: the account owner of the contract's abi


* `hash`: is the sha256 hash of the abi/binary

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public name owner` | 
`public checksum256 hash` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public name owner` {#structeosiosystem_1_1abi__hash_1a4fb47566cda77ad38ad41fcf4fa3616d}





### `public checksum256 hash` {#structeosiosystem_1_1abi__hash_1aa4a25380a39a47af422f17cbea44ba7a}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1abi__hash_1ad54c525ed4f04c7729f5d46b8e641e30}






# struct `authority` {#structeosiosystem_1_1authority}




Blockchain authority.

An authority is defined by:

* a vector of key_weights (a [key_weight](#structeosiosystem_1_1key__weight) is a public key plus a weight),


* a vector of permission_level_weights, (a permission_level is an account name plus a permission name)


* a vector of wait_weights (a [wait_weight](#structeosiosystem_1_1wait__weight) is defined by a number of seconds to wait and a weight)


* a threshold value

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint32_t threshold` | 
`public std::vector< `[`key_weight`](#structeosiosystem_1_1key__weight)` > keys` | 
`public std::vector< `[`permission_level_weight`](#structeosiosystem_1_1permission__level__weight)` > accounts` | 
`public std::vector< `[`wait_weight`](#structeosiosystem_1_1wait__weight)` > waits` | 

## Members

### `public uint32_t threshold` {#structeosiosystem_1_1authority_1a81af9d5da4986a2630f58039f1b42848}





### `public std::vector< `[`key_weight`](#structeosiosystem_1_1key__weight)` > keys` {#structeosiosystem_1_1authority_1a9143e8ec444196d3e3fd99a47a2df4c8}





### `public std::vector< `[`permission_level_weight`](#structeosiosystem_1_1permission__level__weight)` > accounts` {#structeosiosystem_1_1authority_1ad24536a9a087da188510fec55141ef37}





### `public std::vector< `[`wait_weight`](#structeosiosystem_1_1wait__weight)` > waits` {#structeosiosystem_1_1authority_1a7015a1a5d7dd1bb6a723b7f121f4870f}






# struct `bid_refund` {#structeosiosystem_1_1bid__refund}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public name bidder` | 
`public asset amount` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public name bidder` {#structeosiosystem_1_1bid__refund_1ae6bd67f412850f81514ea8b5a5ad4abd}





### `public asset amount` {#structeosiosystem_1_1bid__refund_1aec14d41b70f2332af3e94d84f480077f}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1bid__refund_1a3a7f9c7f81457c17f15ade541c402af1}






# struct `block_header` {#structeosiosystem_1_1block__header}




Blockchain block header.

A block header is defined by:

* a timestamp,


* the producer that created it,


* a confirmed flag default as zero,


* a link to previous block,


* a link to the transaction merkel root,


* a link to action root,


* a schedule version,


* and a producers' schedule.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint32_t timestamp` | 
`public name producer` | 
`public uint16_t confirmed` | 
`public checksum256 previous` | 
`public checksum256 transaction_mroot` | 
`public checksum256 action_mroot` | 
`public uint32_t schedule_version` | 
`public std::optional< eosio::producer_schedule > new_producers` | 

## Members

### `public uint32_t timestamp` {#structeosiosystem_1_1block__header_1a9c9c9cbc1cd8ef72b0fce4d6e81002ec}





### `public name producer` {#structeosiosystem_1_1block__header_1aaa922bddcad01318dc6fc11dc13c5e79}





### `public uint16_t confirmed` {#structeosiosystem_1_1block__header_1a4982fd9b60cbc7a50b5b533af097c373}





### `public checksum256 previous` {#structeosiosystem_1_1block__header_1a777160bdbc82a251b0bfb9c7d6e2e4a8}





### `public checksum256 transaction_mroot` {#structeosiosystem_1_1block__header_1abdae1afa7aa05897fe48322c0973fe7a}





### `public checksum256 action_mroot` {#structeosiosystem_1_1block__header_1a743915ddc889a572f74dbd4f20be28c4}





### `public uint32_t schedule_version` {#structeosiosystem_1_1block__header_1a5a58fc892253b39a20815926440e0637}





### `public std::optional< eosio::producer_schedule > new_producers` {#structeosiosystem_1_1block__header_1a209c1b178d315a0f672d628675f065ac}






# struct `exchange_state` {#structeosiosystem_1_1exchange__state}




Uses Bancor math to create a 50/50 relay between two asset types. The state of the bancor exchange is entirely contained within this struct. There are no external side effects associated with using this API.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`struct `[``connector``](#structeosiosystem_1_1exchange__state_1_1connector)        | 
`public asset supply` | 
`public `[`connector`](#structeosiosystem_1_1exchange__state_1_1connector)` base` | 
`public `[`connector`](#structeosiosystem_1_1exchange__state_1_1connector)` quote` | 
`public inline asset convert_to_exchange(`[`connector`](#structeosiosystem_1_1exchange__state_1_1connector)` & c,asset in)` | 
`public inline asset convert_from_exchange(`[`connector`](#structeosiosystem_1_1exchange__state_1_1connector)` & c,asset in)` | 
`public inline asset convert(asset from,const symbol & to)` | 
`public inline uint64_t primary_key() const` | 
`public asset convert_to_exchange(`[`connector`](#structeosiosystem_1_1exchange__state_1_1connector)` & reserve,const asset & payment)` | 
`public asset convert_from_exchange(`[`connector`](#structeosiosystem_1_1exchange__state_1_1connector)` & reserve,const asset & tokens)` | 
`public asset convert(const asset & from,const symbol & to)` | 
`public asset direct_convert(const asset & from,const symbol & to)` | 

## Members

### `struct `[``connector``](#structeosiosystem_1_1exchange__state_1_1connector) {#structeosiosystem_1_1exchange__state_1_1connector}




### `public asset supply` {#structeosiosystem_1_1exchange__state_1a46dd436b7c53a2cc5fb718f18176f55f}





### `public `[`connector`](#structeosiosystem_1_1exchange__state_1_1connector)` base` {#structeosiosystem_1_1exchange__state_1a64022b0822ac4b53ff484590f76ef021}





### `public `[`connector`](#structeosiosystem_1_1exchange__state_1_1connector)` quote` {#structeosiosystem_1_1exchange__state_1a193594141203ddbb6a37d7cf56201f7f}





### `public inline asset convert_to_exchange(`[`connector`](#structeosiosystem_1_1exchange__state_1_1connector)` & c,asset in)` {#structeosiosystem_1_1exchange__state_1a7c029ed83a27b04524911cf1c3f647c8}





### `public inline asset convert_from_exchange(`[`connector`](#structeosiosystem_1_1exchange__state_1_1connector)` & c,asset in)` {#structeosiosystem_1_1exchange__state_1ade7bfd99be01e15093f27d95a3013ba7}





### `public inline asset convert(asset from,const symbol & to)` {#structeosiosystem_1_1exchange__state_1a9b44e4142697e729fe05dc028b932c0e}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1exchange__state_1a124e84f589746ad2bada2ea98af5b5e2}





### `public asset convert_to_exchange(`[`connector`](#structeosiosystem_1_1exchange__state_1_1connector)` & reserve,const asset & payment)` {#structeosiosystem_1_1exchange__state_1aa6e00de0427eba26f16acc1d0c85e5a7}





### `public asset convert_from_exchange(`[`connector`](#structeosiosystem_1_1exchange__state_1_1connector)` & reserve,const asset & tokens)` {#structeosiosystem_1_1exchange__state_1a7056cfffba84abdf75b38f2d81d95ccd}





### `public asset convert(const asset & from,const symbol & to)` {#structeosiosystem_1_1exchange__state_1aff0f484e4eac6b51e3b01839a884e12e}





### `public asset direct_convert(const asset & from,const symbol & to)` {#structeosiosystem_1_1exchange__state_1a048b1798df129a50aade23df905cd87b}






# struct `connector` {#structeosiosystem_1_1exchange__state_1_1connector}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public asset balance` | 
`public double weight` | 

## Members

### `public asset balance` {#structeosiosystem_1_1exchange__state_1_1connector_1aa0c34b770479927ae7d4f0760bc0e574}





### `public double weight` {#structeosiosystem_1_1exchange__state_1_1connector_1a0132eb8bc9c7a9fe1958c98a436e4262}






# struct `delegated_bandwidth` {#structeosiosystem_1_1delegated__bandwidth}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public name from` | 
`public name to` | 
`public asset net_weight` | 
`public asset cpu_weight` | 
`public inline bool is_empty() const` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public name from` {#structeosiosystem_1_1delegated__bandwidth_1a390e64c81acb62584280ecdf77e67882}





### `public name to` {#structeosiosystem_1_1delegated__bandwidth_1a45fe0c03daff4ca74728a490a1973f6d}





### `public asset net_weight` {#structeosiosystem_1_1delegated__bandwidth_1afb71866724fce6fda77786d5f1d4eb6a}





### `public asset cpu_weight` {#structeosiosystem_1_1delegated__bandwidth_1a62526659e054a4e788ecfd334056dfdf}





### `public inline bool is_empty() const` {#structeosiosystem_1_1delegated__bandwidth_1a445d41ecfce48fe5e7b73de079a889d8}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1delegated__bandwidth_1acc330b16cef3555d2b8bb1a1547f58dc}






# struct `emission_state` {#structeosiosystem_1_1emission__state}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t tact_number` | 
`public uint64_t tact_duration` | 
`public double emission_factor` | 
`public asset current_supply` | 
`public eosio::time_point_sec tact_open_at` | 
`public eosio::time_point_sec tact_close_at` | 
`public asset tact_fees` | 
`public asset back_from_producers` | 
`public asset tact_emission` | 
`public asset emission_start` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint64_t tact_number` {#structeosiosystem_1_1emission__state_1a9c4d00c5a36852556fbe9e83101b2e00}





### `public uint64_t tact_duration` {#structeosiosystem_1_1emission__state_1a26bf3ccc4fe249c2942f01dcd7eccd27}





### `public double emission_factor` {#structeosiosystem_1_1emission__state_1aa8f63ed17d431571d06f606ee145cda4}





### `public asset current_supply` {#structeosiosystem_1_1emission__state_1aa9e84158684b5b7099a9f9a62ce68696}





### `public eosio::time_point_sec tact_open_at` {#structeosiosystem_1_1emission__state_1a1baf62f98563e7c24c38d655f4f8a315}





### `public eosio::time_point_sec tact_close_at` {#structeosiosystem_1_1emission__state_1a6b88f044bf9958e9d172cc45779b36b9}





### `public asset tact_fees` {#structeosiosystem_1_1emission__state_1aa6924a38f97224ec4b2c2e6dcb765273}





### `public asset back_from_producers` {#structeosiosystem_1_1emission__state_1a11f8ea9b43b4483e667bc2359e30a710}





### `public asset tact_emission` {#structeosiosystem_1_1emission__state_1afd847a2833d395f5557de05b87610cce}





### `public asset emission_start` {#structeosiosystem_1_1emission__state_1aabed9cdc75418feb33d5870f8a59b25d}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1emission__state_1a682bcd04021990275ffb96e79db47c5e}






# struct `eosio_global_state` {#structeosiosystem_1_1eosio__global__state}

```
struct eosio_global_state
  : public eosio::blockchain_parameters
```  





## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t max_ram_size` | 
`public uint64_t total_ram_bytes_reserved` | 
`public int64_t total_ram_stake` | 
`public block_timestamp last_producer_schedule_update` | 
`public time_point last_pervote_bucket_fill` | 
`public int64_t pervote_bucket` | 
`public int64_t perblock_bucket` | 
`public uint32_t total_unpaid_blocks` | 
`public int64_t total_activated_stake` | all blocks which have been produced but not paid
`public time_point thresh_activated_stake_time` | 
`public uint16_t last_producer_schedule_size` | 
`public double total_producer_vote_weight` | 
`public block_timestamp last_name_close` | the sum of all producer votes
`public inline uint64_t free_ram() const` | 

## Members

### `public uint64_t max_ram_size` {#structeosiosystem_1_1eosio__global__state_1a2a2f2607dd31f0623a7d5185bf967c4d}





### `public uint64_t total_ram_bytes_reserved` {#structeosiosystem_1_1eosio__global__state_1ab11f6efb709f15f9d31c3e8a20897149}





### `public int64_t total_ram_stake` {#structeosiosystem_1_1eosio__global__state_1a491a74bf4176ddd4485941a841fff13a}





### `public block_timestamp last_producer_schedule_update` {#structeosiosystem_1_1eosio__global__state_1a00d9962c27103495ffdae8b14ca89fb4}





### `public time_point last_pervote_bucket_fill` {#structeosiosystem_1_1eosio__global__state_1afabbd0323315bef9ad7b5d2f953a0340}





### `public int64_t pervote_bucket` {#structeosiosystem_1_1eosio__global__state_1a1533ba61f8191c5323d28a19684fe0bd}





### `public int64_t perblock_bucket` {#structeosiosystem_1_1eosio__global__state_1ab7c0fbe51aa0335e5048be97bf453db7}





### `public uint32_t total_unpaid_blocks` {#structeosiosystem_1_1eosio__global__state_1af7d50c559ad0cfffaa68749e16aed7aa}





### `public int64_t total_activated_stake` {#structeosiosystem_1_1eosio__global__state_1a08c4b026bea40e1d1ee500d289e786ad}

all blocks which have been produced but not paid



### `public time_point thresh_activated_stake_time` {#structeosiosystem_1_1eosio__global__state_1a67107f34cfdffa85a1195ed0e64612db}





### `public uint16_t last_producer_schedule_size` {#structeosiosystem_1_1eosio__global__state_1a4e768096f99af94ce4048446c90075a8}





### `public double total_producer_vote_weight` {#structeosiosystem_1_1eosio__global__state_1a40342aff032242dd2a54723c60587d29}





### `public block_timestamp last_name_close` {#structeosiosystem_1_1eosio__global__state_1aa4f0ee20aedd1f86a3f984c57270bfd7}

the sum of all producer votes



### `public inline uint64_t free_ram() const` {#structeosiosystem_1_1eosio__global__state_1ab13e31d124330a1ba278dfb46fdee173}






# struct `eosio_global_state2` {#structeosiosystem_1_1eosio__global__state2}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint16_t new_ram_per_block` | 
`public block_timestamp last_ram_increase` | 
`public block_timestamp last_block_num` | 
`public double total_producer_votepay_share` | 
`public uint8_t revision` | used to track version updates in the future.
`public inline  eosio_global_state2()` | 

## Members

### `public uint16_t new_ram_per_block` {#structeosiosystem_1_1eosio__global__state2_1ad29c5ed6cc975beb7578f57aaf20c98f}





### `public block_timestamp last_ram_increase` {#structeosiosystem_1_1eosio__global__state2_1a7b233e1f64b6da410d227d5891ca399c}





### `public block_timestamp last_block_num` {#structeosiosystem_1_1eosio__global__state2_1a5efff2639a098649fc9f3478a5e7115a}





### `public double total_producer_votepay_share` {#structeosiosystem_1_1eosio__global__state2_1a23b66248b9c355c2fa9d2a3c8eba11b5}





### `public uint8_t revision` {#structeosiosystem_1_1eosio__global__state2_1aa14325e22eebf822987145b396e7fb60}

used to track version updates in the future.



### `public inline  eosio_global_state2()` {#structeosiosystem_1_1eosio__global__state2_1ab1a68ec079eab13d8d945ff9db03cb1f}






# struct `eosio_global_state3` {#structeosiosystem_1_1eosio__global__state3}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public time_point last_vpay_state_update` | 
`public double total_vpay_share_change_rate` | 
`public inline  eosio_global_state3()` | 

## Members

### `public time_point last_vpay_state_update` {#structeosiosystem_1_1eosio__global__state3_1accd351e80b36cfc0ed6db8eebef527d2}





### `public double total_vpay_share_change_rate` {#structeosiosystem_1_1eosio__global__state3_1a346f3101b6b89488cba4d6d4e67d9273}





### `public inline  eosio_global_state3()` {#structeosiosystem_1_1eosio__global__state3_1a8580bac2c916643fb0e5ffd33185419d}






# struct `eosio_global_state4` {#structeosiosystem_1_1eosio__global__state4}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public double continuous_rate` | 
`public int64_t inflation_pay_factor` | 
`public int64_t votepay_factor` | 
`public inline  eosio_global_state4()` | 

## Members

### `public double continuous_rate` {#structeosiosystem_1_1eosio__global__state4_1aea57a6e1843364aeed35c5fc72c51376}





### `public int64_t inflation_pay_factor` {#structeosiosystem_1_1eosio__global__state4_1a5978c2508b9d874e8191992236852b2f}





### `public int64_t votepay_factor` {#structeosiosystem_1_1eosio__global__state4_1a8546257ff3690e0fcb4f73a9c88762b3}





### `public inline  eosio_global_state4()` {#structeosiosystem_1_1eosio__global__state4_1ad1567456a109d93a0308bb1780207abf}






# struct `key_weight` {#structeosiosystem_1_1key__weight}




Weighted key.

A weighted key is defined by a public key and an associated weight.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::public_key key` | 
`public uint16_t weight` | 

## Members

### `public eosio::public_key key` {#structeosiosystem_1_1key__weight_1a31686ad5ac920e70870cf69e6a2dddc6}





### `public uint16_t weight` {#structeosiosystem_1_1key__weight_1a2e8de08d6240bb75204c4d5195489f85}






# struct `limit_auth_change` {#structeosiosystem_1_1limit__auth__change}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint8_t version` | 
`public name account` | 
`public std::vector< name > allow_perms` | 
`public std::vector< name > disallow_perms` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint8_t version` {#structeosiosystem_1_1limit__auth__change_1ac9ee9788288b853d89f9bbe7ab73ccac}





### `public name account` {#structeosiosystem_1_1limit__auth__change_1a624c1c7a43da37e6697c618801ac1a4c}





### `public std::vector< name > allow_perms` {#structeosiosystem_1_1limit__auth__change_1a9535c07d3294a3a65a69adb9ff8f6301}





### `public std::vector< name > disallow_perms` {#structeosiosystem_1_1limit__auth__change_1a49b0036332a792a390387813cfe20afd}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1limit__auth__change_1afca9e603259b25aca9807294c17d3a5a}






# struct `name_bid` {#structeosiosystem_1_1name__bid}




The `eosio.system` smart contract is provided by `block.one` as a sample system contract, and it defines the structures and actions needed for blockchain's core functionality.

Just like in the `eosio.bios` sample contract implementation, there are a few actions which are not implemented at the contract level (`newaccount`, `updateauth`, `deleteauth`, `linkauth`, `unlinkauth`, `canceldelay`, `onerror`, `setabi`, `setcode`), they are just declared in the contract so they will show in the contract's ABI and users will be able to push those actions to the chain via the account holding the `eosio.system` contract, but the implementation is at the EOSIO core level. They are referred to as EOSIO native actions.



* Users can stake tokens for CPU and Network bandwidth, and then vote for producers or delegate their vote to a proxy.


* Producers register in order to be voted for, and can claim per-block and per-vote rewards.


* Users can buy and sell RAM at a market-determined price.


* Users can bid on premium names.


* A resource exchange system (REX) allows token holders to lend their tokens, and users to rent CPU and Network resources in return for a market-determined fee.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public name newname` | 
`public name high_bidder` | 
`public int64_t high_bid` | negative high_bid == closed auction waiting to be claimed
`public time_point last_bid_time` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t by_high_bid() const` | 

## Members

### `public name newname` {#structeosiosystem_1_1name__bid_1a3463fcad67e50421775df9e215fcc6f5}





### `public name high_bidder` {#structeosiosystem_1_1name__bid_1a96ebb10d61560583560670e686216783}





### `public int64_t high_bid` {#structeosiosystem_1_1name__bid_1ac9ef960920cc7cf790dbe29b7378a1e6}

negative high_bid == closed auction waiting to be claimed



### `public time_point last_bid_time` {#structeosiosystem_1_1name__bid_1aaf689933142cde0cc33c8a945d5e1dde}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1name__bid_1aee075c874d04b3b4fa03a6b7ba0e1be5}





### `public inline uint64_t by_high_bid() const` {#structeosiosystem_1_1name__bid_1a5b559444f1ddf4eee7a32c53a3ef9485}






# struct `pair_time_point_sec_int64` {#structeosiosystem_1_1pair__time__point__sec__int64}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public time_point_sec first` | 
`public int64_t second` | 
`public  EOSLIB_SERIALIZE(`[`pair_time_point_sec_int64`](#structeosiosystem_1_1pair__time__point__sec__int64)`,(`[`first](#structeosiosystem_1_1pair__time__point__sec__int64_1acd031753aeb4381736b222cb5af110f1))([second`](#structeosiosystem_1_1pair__time__point__sec__int64_1ac501dfa169ad5f4a3d72554082f1ea26))`)` | 

## Members

### `public time_point_sec first` {#structeosiosystem_1_1pair__time__point__sec__int64_1acd031753aeb4381736b222cb5af110f1}





### `public int64_t second` {#structeosiosystem_1_1pair__time__point__sec__int64_1ac501dfa169ad5f4a3d72554082f1ea26}





### `public  EOSLIB_SERIALIZE(`[`pair_time_point_sec_int64`](#structeosiosystem_1_1pair__time__point__sec__int64)`,(`[`first](#structeosiosystem_1_1pair__time__point__sec__int64_1acd031753aeb4381736b222cb5af110f1))([second`](#structeosiosystem_1_1pair__time__point__sec__int64_1ac501dfa169ad5f4a3d72554082f1ea26))`)` {#structeosiosystem_1_1pair__time__point__sec__int64_1a61aef904c9c7214339d59263e49148b1}






# struct `permission_level_weight` {#structeosiosystem_1_1permission__level__weight}




A weighted permission.

Defines a weighted permission, that is a permission which has a weight associated. A permission is defined by an account name plus a permission name.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public permission_level permission` | 
`public uint16_t weight` | 

## Members

### `public permission_level permission` {#structeosiosystem_1_1permission__level__weight_1ac98d5c5795870835a76e2c1d73bda998}





### `public uint16_t weight` {#structeosiosystem_1_1permission__level__weight_1a2df97f79b29e376a927df14444071684}






# struct `powerup_config` {#structeosiosystem_1_1powerup__config}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public std::optional< uint32_t > powerup_days` | 
`public std::optional< asset > min_powerup_fee` | 

## Members

### `public std::optional< uint32_t > powerup_days` {#structeosiosystem_1_1powerup__config_1a0bf5894e66ce146b1d46dab27ed79052}





### `public std::optional< asset > min_powerup_fee` {#structeosiosystem_1_1powerup__config_1a2d7ede4759845fcf1dd25498dfb48f10}






# struct `powerup_order` {#structeosiosystem_1_1powerup__order}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint8_t version` | 
`public uint64_t id` | 
`public name owner` | 
`public int64_t net_weight` | 
`public int64_t cpu_weight` | 
`public int64_t ram_bytes` | 
`public time_point_sec expires` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t by_owner() const` | 
`public inline uint64_t by_expires() const` | 

## Members

### `public uint8_t version` {#structeosiosystem_1_1powerup__order_1a5add3b2a59588f2a986ab8533b6ef07e}





### `public uint64_t id` {#structeosiosystem_1_1powerup__order_1a0c852ce9dc447d27818c6f76c9966f5b}





### `public name owner` {#structeosiosystem_1_1powerup__order_1a7367e434fe5c04cc3eea0a4e79d925f4}





### `public int64_t net_weight` {#structeosiosystem_1_1powerup__order_1a4ef585bec4e45a5643280a2be23c8abe}





### `public int64_t cpu_weight` {#structeosiosystem_1_1powerup__order_1afedd41b2e48a95941cf15b1a44dc401c}





### `public int64_t ram_bytes` {#structeosiosystem_1_1powerup__order_1ab74ec855ca847ed930eb17b1c800eadb}





### `public time_point_sec expires` {#structeosiosystem_1_1powerup__order_1a2d6b97d6744b9d46cb2fa6f64ecbcc9e}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1powerup__order_1a84d6154b02216683f5bac277a9c332ab}





### `public inline uint64_t by_owner() const` {#structeosiosystem_1_1powerup__order_1ad6f472ce4c90343ff6c3a2a312ac64ab}





### `public inline uint64_t by_expires() const` {#structeosiosystem_1_1powerup__order_1a9a404ef12847703055ba5da5b47af05a}






# struct `powerup_state` {#structeosiosystem_1_1powerup__state}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public `[`powerup_state_resource`](#structeosiosystem_1_1powerup__state__resource)` net` | 
`public `[`powerup_state_resource`](#structeosiosystem_1_1powerup__state__resource)` cpu` | 
`public `[`powerup_state_resource`](#structeosiosystem_1_1powerup__state__resource)` ram` | 
`public uint32_t powerup_days` | 
`public asset min_powerup_fee` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public `[`powerup_state_resource`](#structeosiosystem_1_1powerup__state__resource)` net` {#structeosiosystem_1_1powerup__state_1a50378b359db22153b5767eebf3d5e91e}





### `public `[`powerup_state_resource`](#structeosiosystem_1_1powerup__state__resource)` cpu` {#structeosiosystem_1_1powerup__state_1ac00370168898d00ee8c2793728e45636}





### `public `[`powerup_state_resource`](#structeosiosystem_1_1powerup__state__resource)` ram` {#structeosiosystem_1_1powerup__state_1a66206e7637b2d175a702678ce0f858c3}





### `public uint32_t powerup_days` {#structeosiosystem_1_1powerup__state_1a41192b702d0a81d46a3f19e87149c740}





### `public asset min_powerup_fee` {#structeosiosystem_1_1powerup__state_1a1e3527154e64dac849d3a04037ff131a}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1powerup__state_1a2ee6f547d067dd757cc345f51b0b385a}






# struct `powerup_state_resource` {#structeosiosystem_1_1powerup__state__resource}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public int64_t weight` | 
`public int64_t utilization` | 

## Members

### `public int64_t weight` {#structeosiosystem_1_1powerup__state__resource_1ae11adb8af7452cc4b56b3c4efd27c4b5}





### `public int64_t utilization` {#structeosiosystem_1_1powerup__state__resource_1a57a7d6c1f329f96a1964815e67bf5948}






# struct `producer_info` {#structeosiosystem_1_1producer__info}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public name owner` | 
`public double total_votes` | 
`public eosio::public_key producer_key` | 
`public bool is_active` | a packed public key object
`public std::string url` | 
`public uint32_t unpaid_blocks` | 
`public time_point last_claim_time` | 
`public uint16_t location` | 
`public eosio::binary_extension< eosio::block_signing_authority > producer_authority` | 
`public inline uint64_t primary_key() const` | 
`public inline double by_votes() const` | 
`public inline bool active() const` | 
`public inline void deactivate()` | 
`public inline eosio::block_signing_authority get_producer_authority() const` | 

## Members

### `public name owner` {#structeosiosystem_1_1producer__info_1a8e7b745ce26e291c9bb6c01197b39481}





### `public double total_votes` {#structeosiosystem_1_1producer__info_1abc5e3b6f64341d7a4c47dcf7f7303466}





### `public eosio::public_key producer_key` {#structeosiosystem_1_1producer__info_1ae39a2bd7332a10fe183c86e8810ffc42}





### `public bool is_active` {#structeosiosystem_1_1producer__info_1a1e99c8edf840eef65937a55e81a96b50}

a packed public key object



### `public std::string url` {#structeosiosystem_1_1producer__info_1ae5fa32f4888e2db7faddb5e5b2b4da58}





### `public uint32_t unpaid_blocks` {#structeosiosystem_1_1producer__info_1a24e7945da7f329a88dd915d1582a877c}





### `public time_point last_claim_time` {#structeosiosystem_1_1producer__info_1a2c0ee0458d7b6b5629bab41d2f94a156}





### `public uint16_t location` {#structeosiosystem_1_1producer__info_1a5e543ec2f645048f754beca0fafc9daa}





### `public eosio::binary_extension< eosio::block_signing_authority > producer_authority` {#structeosiosystem_1_1producer__info_1acca552d34b42bf725872f6dd4883e8aa}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1producer__info_1a9a9423735c7e495e3e244a92719c5606}





### `public inline double by_votes() const` {#structeosiosystem_1_1producer__info_1a2923be76a2e0b28b39b772fc8f5f30aa}





### `public inline bool active() const` {#structeosiosystem_1_1producer__info_1ac747e9353d178231d7d71131b5c4e94e}





### `public inline void deactivate()` {#structeosiosystem_1_1producer__info_1ab83626158bab8b0d943315ace5255ada}





### `public inline eosio::block_signing_authority get_producer_authority() const` {#structeosiosystem_1_1producer__info_1a2134cf5d2aa6af5b55fa9a577654e4aa}






# struct `producer_info2` {#structeosiosystem_1_1producer__info2}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public name owner` | 
`public double votepay_share` | 
`public time_point last_votepay_share_update` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public name owner` {#structeosiosystem_1_1producer__info2_1aa80744eebc2853521dd294e15124b2b3}





### `public double votepay_share` {#structeosiosystem_1_1producer__info2_1a62de9e8545175dc4aa45e5d9494c272b}





### `public time_point last_votepay_share_update` {#structeosiosystem_1_1producer__info2_1ae6771ab3b619d7cc3a354baf05336e96}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1producer__info2_1ae112142fed736880a03bd0ee305b34ef}






# struct `ram_debt_record` {#structeosiosystem_1_1ram__debt__record}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public name account` | 
`public int64_t ram_debt` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public name account` {#structeosiosystem_1_1ram__debt__record_1a02a0d0b6c6afc5f2c46cb1fd9fa23816}





### `public int64_t ram_debt` {#structeosiosystem_1_1ram__debt__record_1ab1cdbf3bc0d4ac2b809e4a1b8d959719}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1ram__debt__record_1a0d652dd6aba9f3bcf5af7a732110e4e1}






# struct `refund_request` {#structeosiosystem_1_1refund__request}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public name owner` | 
`public time_point_sec request_time` | 
`public eosio::asset net_amount` | 
`public eosio::asset cpu_amount` | 
`public inline bool is_empty() const` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public name owner` {#structeosiosystem_1_1refund__request_1a11e5c7ef5cc00904a0d00bf0af8900ef}





### `public time_point_sec request_time` {#structeosiosystem_1_1refund__request_1ac5513485aad2e2e0de4717ca1b06530e}





### `public eosio::asset net_amount` {#structeosiosystem_1_1refund__request_1a5f314641a1ad02216a89275dd38c8b8b}





### `public eosio::asset cpu_amount` {#structeosiosystem_1_1refund__request_1a8d8e9e6b4bcdbdc1b30b045feb58c6fa}





### `public inline bool is_empty() const` {#structeosiosystem_1_1refund__request_1a2061e7187b06f1adb76043b7935bd091}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1refund__request_1a527ccee1812739017b22d69139b2e40b}






# struct `rex_balance` {#structeosiosystem_1_1rex__balance}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint8_t version` | 
`public name owner` | 
`public asset vote_stake` | 
`public asset rex_balance` | 
`public int64_t matured_rex` | 
`public std::vector< `[`pair_time_point_sec_int64`](#structeosiosystem_1_1pair__time__point__sec__int64)` > rex_maturities` | 
`public inline uint64_t primary_key() const` | REX daily maturity buckets.

## Members

### `public uint8_t version` {#structeosiosystem_1_1rex__balance_1a3db8caee6ce5eb1430db7d473a072098}





### `public name owner` {#structeosiosystem_1_1rex__balance_1aa7efa762e2abf679ec0c159fb1439372}





### `public asset vote_stake` {#structeosiosystem_1_1rex__balance_1a5b062ac98d3d4ad65bdda637f1687952}





### `public asset rex_balance` {#structeosiosystem_1_1rex__balance_1a530ed574f5fa7277422866734ba59aed}





### `public int64_t matured_rex` {#structeosiosystem_1_1rex__balance_1aae06fcc4eda99c14544e1e0394335834}





### `public std::vector< `[`pair_time_point_sec_int64`](#structeosiosystem_1_1pair__time__point__sec__int64)` > rex_maturities` {#structeosiosystem_1_1rex__balance_1ab69ca2daff48bdeb9a54de9f19895d38}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1rex__balance_1a8aebb3d7caed879be417d12c0ea75ff8}

REX daily maturity buckets.




# struct `rex_fund` {#structeosiosystem_1_1rex__fund}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint8_t version` | 
`public name owner` | 
`public asset balance` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint8_t version` {#structeosiosystem_1_1rex__fund_1a0d87faa3b4241bb715ef75854f885815}





### `public name owner` {#structeosiosystem_1_1rex__fund_1a9f04d7bcf465529f0870d54aabf5ae36}





### `public asset balance` {#structeosiosystem_1_1rex__fund_1af8fa096b0e2c5d24798a659aa5ddb309}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1rex__fund_1ada0d465ad79456e859e6dce84be99be5}






# struct `rex_loan` {#structeosiosystem_1_1rex__loan}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint8_t version` | 
`public name from` | 
`public name receiver` | 
`public asset payment` | 
`public asset balance` | 
`public asset total_staked` | 
`public uint64_t loan_num` | 
`public eosio::time_point expiration` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t by_expr() const` | 
`public inline uint64_t by_owner() const` | 

## Members

### `public uint8_t version` {#structeosiosystem_1_1rex__loan_1a0b4256fabf5de5140d7454287b7ce635}





### `public name from` {#structeosiosystem_1_1rex__loan_1a59654a42de37d5f6ad11ef7b1ca8845f}





### `public name receiver` {#structeosiosystem_1_1rex__loan_1ad28a74d7cfcfcbe1c7502c49f34ab988}





### `public asset payment` {#structeosiosystem_1_1rex__loan_1aa0eb3d72cb30bc2b205cebab93a2c4f2}





### `public asset balance` {#structeosiosystem_1_1rex__loan_1aaa8b37af599e458df1cc4a4557558c34}





### `public asset total_staked` {#structeosiosystem_1_1rex__loan_1a030741dbdcc390ac07da48560f86a39b}





### `public uint64_t loan_num` {#structeosiosystem_1_1rex__loan_1a5727e851ef16e34096c5f3864a43d994}





### `public eosio::time_point expiration` {#structeosiosystem_1_1rex__loan_1ab9f18e11705c3798b616cf7caf476590}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1rex__loan_1a98d977e47a68a99e7af5283f5d7621d9}





### `public inline uint64_t by_expr() const` {#structeosiosystem_1_1rex__loan_1a7e1408b91e4d52c1381ce8f99b9eb521}





### `public inline uint64_t by_owner() const` {#structeosiosystem_1_1rex__loan_1a5dc4c8b0bf062ea5a729db6c859c033d}






# struct `rex_order` {#structeosiosystem_1_1rex__order}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint8_t version` | 
`public name owner` | 
`public asset rex_requested` | 
`public asset proceeds` | 
`public asset stake_change` | 
`public eosio::time_point order_time` | 
`public bool is_open` | 
`public inline void close()` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t by_time() const` | 

## Members

### `public uint8_t version` {#structeosiosystem_1_1rex__order_1a3aa81dd239e34b29a863c01bc6af8d75}





### `public name owner` {#structeosiosystem_1_1rex__order_1ab655d38bd47b5e4ec102503954942035}





### `public asset rex_requested` {#structeosiosystem_1_1rex__order_1ab0d23a11ffd760162e02681f64e9639e}





### `public asset proceeds` {#structeosiosystem_1_1rex__order_1a87cdfd344b993ca26a3451124f128911}





### `public asset stake_change` {#structeosiosystem_1_1rex__order_1ad648133afca5497f7e4827b65d1cc529}





### `public eosio::time_point order_time` {#structeosiosystem_1_1rex__order_1aea99fc69a44d35acd09707dac2af61ee}





### `public bool is_open` {#structeosiosystem_1_1rex__order_1a4bc7bda7076a0a6b3b410ec83d2eb365}





### `public inline void close()` {#structeosiosystem_1_1rex__order_1a91ae93fddccc4a9bcc9cf156ff602bd6}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1rex__order_1a38851f3f71d5a2a95de515b044fcf90b}





### `public inline uint64_t by_time() const` {#structeosiosystem_1_1rex__order_1a177603993abdb4e9b521d273d9cbc818}






# struct `rex_order_outcome` {#structeosiosystem_1_1rex__order__outcome}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public bool success` | 
`public asset proceeds` | 
`public asset stake_change` | 

## Members

### `public bool success` {#structeosiosystem_1_1rex__order__outcome_1aa322c7f9f86c43de4134d427543f628f}





### `public asset proceeds` {#structeosiosystem_1_1rex__order__outcome_1a268d8b4466e29f8cb3dfaf58411a8602}





### `public asset stake_change` {#structeosiosystem_1_1rex__order__outcome_1a0cede7598459d110eb4f68ea71cb1e86}






# struct `rex_pool` {#structeosiosystem_1_1rex__pool}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint8_t version` | 
`public asset total_lent` | 
`public asset total_unlent` | 
`public asset total_rent` | 
`public asset total_lendable` | 
`public asset total_rex` | 
`public asset namebid_proceeds` | 
`public uint64_t loan_num` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint8_t version` {#structeosiosystem_1_1rex__pool_1addca00704e91405d1808533eefd8fd56}





### `public asset total_lent` {#structeosiosystem_1_1rex__pool_1a1819b3c6b83b60c9c31e1440dc074d58}





### `public asset total_unlent` {#structeosiosystem_1_1rex__pool_1aed217f873a6574c1db42493c9ad385d0}





### `public asset total_rent` {#structeosiosystem_1_1rex__pool_1ae2c5584ec5fe6f17b765e04b5c5c468a}





### `public asset total_lendable` {#structeosiosystem_1_1rex__pool_1a3a8999d7f7b8029e0d718f885b683737}





### `public asset total_rex` {#structeosiosystem_1_1rex__pool_1aaf99b9aead4d756af061f16febe73006}





### `public asset namebid_proceeds` {#structeosiosystem_1_1rex__pool_1a617f0698ad272dc19fae83cd576c1dc5}





### `public uint64_t loan_num` {#structeosiosystem_1_1rex__pool_1a95d45a7a85681d1930a62443d0fec47a}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1rex__pool_1a138432772e5150f86b56b22e5011048b}






# struct `rex_return_buckets` {#structeosiosystem_1_1rex__return__buckets}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint8_t version` | 
`public std::vector< `[`pair_time_point_sec_int64`](#structeosiosystem_1_1pair__time__point__sec__int64)` > return_buckets` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint8_t version` {#structeosiosystem_1_1rex__return__buckets_1a24996f1358821a23925f8fb6fedfbcac}





### `public std::vector< `[`pair_time_point_sec_int64`](#structeosiosystem_1_1pair__time__point__sec__int64)` > return_buckets` {#structeosiosystem_1_1rex__return__buckets_1a40398601193e8e6f2e343b417c5195c6}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1rex__return__buckets_1a91e47ae14cf12803658fc81858ba7d76}






# struct `rex_return_pool` {#structeosiosystem_1_1rex__return__pool}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint8_t version` | 
`public time_point_sec last_dist_time` | 
`public time_point_sec pending_bucket_time` | 
`public time_point_sec oldest_bucket_time` | 
`public int64_t pending_bucket_proceeds` | 
`public int64_t current_rate_of_increase` | 
`public int64_t proceeds` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint8_t version` {#structeosiosystem_1_1rex__return__pool_1a9b16ec989d93521cc26f51b97793c662}





### `public time_point_sec last_dist_time` {#structeosiosystem_1_1rex__return__pool_1a6bcacb0d59cb8951b83f089e2dee3bed}





### `public time_point_sec pending_bucket_time` {#structeosiosystem_1_1rex__return__pool_1a54bd0982d04bfea79eea5343cb2dce33}





### `public time_point_sec oldest_bucket_time` {#structeosiosystem_1_1rex__return__pool_1a50d9c4534af93ce26fffc8ae21c83981}





### `public int64_t pending_bucket_proceeds` {#structeosiosystem_1_1rex__return__pool_1a6586a70341c24f27a2b0ef2234fc5005}





### `public int64_t current_rate_of_increase` {#structeosiosystem_1_1rex__return__pool_1a9c40149b316eefb370e1733584ff1d6f}





### `public int64_t proceeds` {#structeosiosystem_1_1rex__return__pool_1a8c9014435e9325e35ee05115ac9e0ca3}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1rex__return__pool_1a1e8d9b3fd69ac163dcf6d77c3aedb80a}






# struct `user_resources` {#structeosiosystem_1_1user__resources}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public name owner` | 
`public asset net_weight` | 
`public asset cpu_weight` | 
`public int64_t ram_bytes` | 
`public inline bool is_empty() const` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public name owner` {#structeosiosystem_1_1user__resources_1a8a373411ce7eb180649f9a7c938fb7dd}





### `public asset net_weight` {#structeosiosystem_1_1user__resources_1ad6532b1796871feb9d26c94e69266e7a}





### `public asset cpu_weight` {#structeosiosystem_1_1user__resources_1ace76500804d6a6bb4c0a2f0cc07f08b2}





### `public int64_t ram_bytes` {#structeosiosystem_1_1user__resources_1a89f602f8770024c9d9fd592854ac9e75}





### `public inline bool is_empty() const` {#structeosiosystem_1_1user__resources_1a62970931ec8e856b3ce21a78f5d51306}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1user__resources_1a8c06a7e1b3e4a3cc22af7fd677b09542}






# struct `voter_info` {#structeosiosystem_1_1voter__info}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public name owner` | 
`public name proxy` | the voter
`public std::vector< name > producers` | the proxy set by the voter, if any
`public int64_t staked` | the producers approved by this voter if no proxy set
`public double last_vote_weight` | 
`public double proxied_vote_weight` | the vote weight cast the last time the vote was updated
`public bool is_proxy` | the total vote weight delegated to this voter as a proxy
`public uint32_t flags1` | whether the voter is a proxy for others
`public uint32_t reserved2` | 
`public eosio::asset reserved3` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public name owner` {#structeosiosystem_1_1voter__info_1a09e1d98da6939690519ea85baedf1c01}





### `public name proxy` {#structeosiosystem_1_1voter__info_1a482ed4e822024852ac234353ac5f08cc}

the voter



### `public std::vector< name > producers` {#structeosiosystem_1_1voter__info_1a90023077cb4f76e82c42482dc65a2783}

the proxy set by the voter, if any



### `public int64_t staked` {#structeosiosystem_1_1voter__info_1a3f1a76120b906f5f304c0c4604d4bf37}

the producers approved by this voter if no proxy set



### `public double last_vote_weight` {#structeosiosystem_1_1voter__info_1aa60bfb8489e47d8514efd230cd18288e}





### `public double proxied_vote_weight` {#structeosiosystem_1_1voter__info_1af7a7c2263d90ed79d2bf4281e57193c0}

the vote weight cast the last time the vote was updated



### `public bool is_proxy` {#structeosiosystem_1_1voter__info_1aee50442b79d5561a967486e6b7a61e65}

the total vote weight delegated to this voter as a proxy



### `public uint32_t flags1` {#structeosiosystem_1_1voter__info_1af77c6136a8d1300951ad6f39b3c28dc1}

whether the voter is a proxy for others



### `public uint32_t reserved2` {#structeosiosystem_1_1voter__info_1a5c4ecbb96e5e70acd2b9ce55c6a886d0}





### `public eosio::asset reserved3` {#structeosiosystem_1_1voter__info_1ac45fcd076d0257637f5c6b80618e4647}





### `public inline uint64_t primary_key() const` {#structeosiosystem_1_1voter__info_1a1fdb370974b4c33380e8114ff52a7d8f}






# struct `wait_weight` {#structeosiosystem_1_1wait__weight}




Wait weight.

A wait weight is defined by a number of seconds to wait for and a weight.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint32_t wait_sec` | 
`public uint16_t weight` | 

## Members

### `public uint32_t wait_sec` {#structeosiosystem_1_1wait__weight_1ad12a131189bb9c36470e221222338bb7}





### `public uint16_t weight` {#structeosiosystem_1_1wait__weight_1a9a1e16d11c7b5fd2ced0b1f15f418d8a}






# namespace `eosio`



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`class `[``token``](#classeosio_1_1token)    | 
`class `[``multisig``](#classeosio_1_1multisig)    | 
`class `[``wrap``](#classeosio_1_1wrap)    | 

# class `token` {#classeosio_1_1token}

```
class token
  : public contract
```  



The `[eosio.token](#classeosio_1_1token)` sample system contract defines the structures and actions that allow users to create, issue, and manage tokens for EOSIO based blockchains. It demonstrates one way to implement a smart contract which allows for creation and management of tokens. It is possible for one to create a similar contract which suits different needs. However, it is recommended that if one only needs a token with the below listed actions, that one uses the `[eosio.token](#classeosio_1_1token)` contract instead of developing their own.

The `[eosio.token](#classeosio_1_1token)` contract class also implements two useful public static methods: `get_supply` and `get_balance`. The first allows one to check the total supply of a specified token, created by an account and the second allows one to check the balance of a token for a specified account (the token creator account has to be specified as well).

The `[eosio.token](#classeosio_1_1token)` contract manages the set of tokens, accounts and their corresponding balances, by using two internal multi-index structures: the `accounts` and `stats`. The `accounts` multi-index table holds, for each row, instances of `account` object and the `account` object holds information about the balance of one token. The `accounts` table is scoped to an EOSIO account, and it keeps the rows indexed based on the token's symbol. This means that when one queries the `accounts` multi-index table for an account name the result is all the tokens that account holds at the moment.

Similarly, the `stats` multi-index table, holds instances of `[currency_stats](#structeosio_1_1token_1_1currency__stats)` objects for each row, which contains information about current supply, maximum supply, and the creator account for a symbol token. The `stats` table is scoped to the token symbol. Therefore, when one queries the `stats` table for a token symbol the result is one single entry/row corresponding to the queried symbol token if it was previously created, or nothing, otherwise.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`struct `[``account``](#structeosio_1_1token_1_1account)        | 
`struct `[``currency_stats``](#structeosio_1_1token_1_1currency__stats)        | 
`public void create(const name & issuer,const asset & maximum_supply)` | 
`public void issue(const name & to,const asset & quantity,const string & memo)` | 
`public void retire(const asset & quantity,const string & memo)` | 
`public void transfer(const name & from,const name & to,const asset & quantity,const string & memo)` | 
`public void open(const name & owner,const symbol & symbol,const name & ram_payer)` | 
`public void close(const name & owner,const symbol & symbol)` | 

## Members

### `struct `[``account``](#structeosio_1_1token_1_1account) {#structeosio_1_1token_1_1account}




### `struct `[``currency_stats``](#structeosio_1_1token_1_1currency__stats) {#structeosio_1_1token_1_1currency__stats}




### `public void create(const name & issuer,const asset & maximum_supply)` {#classeosio_1_1token_1a5277670549e2458dd4a4fab4c56a9599}



Allows `issuer` account to create a token in supply of `maximum_supply`. If validation is successful a new entry in statstable for token symbol scope gets created.


#### Parameters
* `issuer` - the account that creates the token, 


* `maximum_supply` - the maximum supply set for the token created.




Token symbol has to be valid, 

Token symbol must not be already created, 

maximum_supply has to be smaller than the maximum supply allowed by the system: 1^62 - 1. 

Maximum supply must be positive;

### `public void issue(const name & to,const asset & quantity,const string & memo)` {#classeosio_1_1token_1a3a62d2a7493710b3e34fbdfd4fe81d59}



This action issues to `to` account a `quantity` of tokens.


#### Parameters
* `to` - the account to issue tokens to, it must be the same as the issuer, 


* `quantity` - the amount of tokens to be issued, @memo - the memo string that accompanies the token issue transaction.

### `public void retire(const asset & quantity,const string & memo)` {#classeosio_1_1token_1a4bf7770086ab81931c6c5a22265bae4f}



The opposite for create action, if all validations succeed, it debits the statstable.supply amount.


#### Parameters
* `quantity` - the quantity of tokens to retire, 


* `memo` - the memo string to accompany the transaction.

### `public void transfer(const name & from,const name & to,const asset & quantity,const string & memo)` {#classeosio_1_1token_1a8a9d08a9791d818797dc5f70ed8f826f}



Allows `from` account to transfer to `to` account the `quantity` tokens. One account is debited and the other is credited with quantity tokens.


#### Parameters
* `from` - the account to transfer from, 


* `to` - the account to be transferred to, 


* `quantity` - the quantity of tokens to be transferred, 


* `memo` - the memo string to accompany the transaction.

### `public void open(const name & owner,const symbol & symbol,const name & ram_payer)` {#classeosio_1_1token_1a4da303699332aacdc69f3ff582477141}



Allows `ram_payer` to create an account `owner` with zero balance for token `symbol` at the expense of `ram_payer`.


#### Parameters
* `owner` - the account to be created, 


* `symbol` - the token to be payed with by `ram_payer`, 


* `ram_payer` - the account that supports the cost of this action.





More information can be read [here](https://github.com/EOSIO/eosio.contracts/issues/62) and [here](https://github.com/EOSIO/eosio.contracts/issues/61).

### `public void close(const name & owner,const symbol & symbol)` {#classeosio_1_1token_1a88402f75888c4cfe6a3ef0391bd7ecbd}



This action is the opposite for open, it closes the account `owner` for token `symbol`.


#### Parameters
* `owner` - the owner account to execute the close action for, 


* `symbol` - the symbol of the token to execute the close action for.




The pair of owner plus symbol has to exist otherwise no action is executed, 

If the pair of owner plus symbol exists, the balance has to be zero.


# struct `account` {#structeosio_1_1token_1_1account}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public asset balance` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public asset balance` {#structeosio_1_1token_1_1account_1a6d6aaf37aa4e794b64a1b04990339043}





### `public inline uint64_t primary_key() const` {#structeosio_1_1token_1_1account_1a0881195a3cc88813bfc082903e2e627a}






# struct `currency_stats` {#structeosio_1_1token_1_1currency__stats}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public asset supply` | 
`public asset max_supply` | 
`public name issuer` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public asset supply` {#structeosio_1_1token_1_1currency__stats_1ab24e3519d9ea9063b80d74a64ec1fd73}





### `public asset max_supply` {#structeosio_1_1token_1_1currency__stats_1ab83559b6e1cdd215cf7b60153009a966}





### `public name issuer` {#structeosio_1_1token_1_1currency__stats_1a98c87418ed2e156ca2bd94a8e51a0b9f}





### `public inline uint64_t primary_key() const` {#structeosio_1_1token_1_1currency__stats_1ac651a49171c9e1b62d07e529657895ec}






# class `multisig` {#classeosio_1_1multisig}

```
class multisig
  : public contract
```  



The `eosio.msig` system contract allows for creation of proposed transactions which require authorization from a list of accounts, approval of the proposed transactions by those accounts required to approve it, and finally, it also allows the execution of the approved transactions on the blockchain.

In short, the workflow to propose, review, approve and then executed a transaction it can be described by the following:

* first you create a transaction json file,


* then you submit this proposal to the `eosio.msig` contract, and you also insert the account permissions required to approve this proposal into the command that submits the proposal to the blockchain,


* the proposal then gets stored on the blockchain by the `eosio.msig` contract, and is accessible for review and approval to those accounts required to approve it,


* after each of the appointed accounts required to approve the proposed transactions reviews and approves it, you can execute the proposed transaction. The `eosio.msig` contract will execute it automatically, but not before validating that the transaction has not expired, it is not cancelled, and it has been signed by all the permissions in the initial proposal's required permission list.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`struct `[``approval``](#structeosio_1_1multisig_1_1approval)        | 
`struct `[``approvals_info``](#structeosio_1_1multisig_1_1approvals__info)        | 
`struct `[``invalidation``](#structeosio_1_1multisig_1_1invalidation)        | 
`struct `[``old_approvals_info``](#structeosio_1_1multisig_1_1old__approvals__info)        | 
`struct `[``proposal``](#structeosio_1_1multisig_1_1proposal)        | 
`public void propose(name proposer,name proposal_name,std::vector< permission_level > requested,ignore< transaction > trx)` | 
`public void approve(name proposer,name proposal_name,permission_level level,const eosio::binary_extension< eosio::checksum256 > & proposal_hash)` | 
`public void unapprove(name proposer,name proposal_name,permission_level level)` | 
`public void cancel(name proposer,name proposal_name,name canceler)` | 
`public void exec(name proposer,name proposal_name,name executer)` | 
`public void invalidate(name account)` | 

## Members

### `struct `[``approval``](#structeosio_1_1multisig_1_1approval) {#structeosio_1_1multisig_1_1approval}




### `struct `[``approvals_info``](#structeosio_1_1multisig_1_1approvals__info) {#structeosio_1_1multisig_1_1approvals__info}




### `struct `[``invalidation``](#structeosio_1_1multisig_1_1invalidation) {#structeosio_1_1multisig_1_1invalidation}




### `struct `[``old_approvals_info``](#structeosio_1_1multisig_1_1old__approvals__info) {#structeosio_1_1multisig_1_1old__approvals__info}




### `struct `[``proposal``](#structeosio_1_1multisig_1_1proposal) {#structeosio_1_1multisig_1_1proposal}




### `public void propose(name proposer,name proposal_name,std::vector< permission_level > requested,ignore< transaction > trx)` {#classeosio_1_1multisig_1acb9cee084979625820b6f45492f13be0}



Propose action, creates a proposal containing one transaction. Allows an account `proposer` to make a proposal `proposal_name` which has `requested` permission levels expected to approve the proposal, and if approved by all expected permission levels then `trx` transaction can we executed by this proposal. The `proposer` account is authorized and the `trx` transaction is verified if it was authorized by the provided keys and permissions, and if the proposal name doesn’t already exist; if all validations pass the `proposal_name` and `trx` trasanction are saved in the proposals table and the `requested` permission levels to the approvals table (for the `proposer` context). Storage changes are billed to `proposer`.


#### Parameters
* `proposer` - The account proposing a transaction 


* `proposal_name` - The name of the proposal (should be unique for proposer) 


* `requested` - Permission levels expected to approve the proposal 


* `trx` - Proposed transaction

### `public void approve(name proposer,name proposal_name,permission_level level,const eosio::binary_extension< eosio::checksum256 > & proposal_hash)` {#classeosio_1_1multisig_1a5cccb8b23b9fffa1b27d443fad9ce3e9}



Approve action approves an existing proposal. Allows an account, the owner of `level` permission, to approve a proposal `proposal_name` proposed by `proposer`. If the proposal's requested approval list contains the `level` permission then the `level` permission is moved from internal `requested_approvals` list to internal `provided_approvals` list of the proposal, thus persisting the approval for the `proposal_name` proposal. Storage changes are billed to `proposer`.


#### Parameters
* `proposer` - The account proposing a transaction 


* `proposal_name` - The name of the proposal (should be unique for proposer) 


* `level` - Permission level approving the transaction 


* `proposal_hash` - Transaction's checksum

### `public void unapprove(name proposer,name proposal_name,permission_level level)` {#classeosio_1_1multisig_1a7d6e568f96c6697728e316557a9bcf1e}



Unapprove action revokes an existing proposal. This action is the reverse of the `approve` action: if all validations pass the `level` permission is erased from internal `provided_approvals` and added to the internal `requested_approvals` list, and thus un-approve or revoke the proposal.


#### Parameters
* `proposer` - The account proposing a transaction 


* `proposal_name` - The name of the proposal (should be an existing proposal) 


* `level` - Permission level revoking approval for proposal

### `public void cancel(name proposer,name proposal_name,name canceler)` {#classeosio_1_1multisig_1a2dc849ab961ea31c0e93ebace0f0dd00}



Cancel action cancels an existing proposal.


#### Parameters
* `proposer` - The account proposing a transaction 


* `proposal_name` - The name of the proposal (should be an existing proposal) 


* `canceler` - The account cancelling the proposal (only the proposer can cancel an unexpired transaction, and the canceler has to be different than the proposer)





Allows the `canceler` account to cancel the `proposal_name` proposal, created by a `proposer`, only after time has expired on the proposed transaction. It removes corresponding entries from internal proptable and from approval (or old approvals) tables as well.

### `public void exec(name proposer,name proposal_name,name executer)` {#classeosio_1_1multisig_1ac31b3d91892bf8fcdad8d6f123c278ff}



Exec action allows an `executer` account to execute a proposal.

Preconditions:

* `executer` has authorization,


* `proposal_name` is found in the proposals table,


* all requested approvals are received,


* proposed transaction is not expired,


* and approval accounts are not found in invalidations table.





If all preconditions are met the transaction is executed as a deferred transaction, and the proposal is erased from the proposals table.


#### Parameters
* `proposer` - The account proposing a transaction 


* `proposal_name` - The name of the proposal (should be an existing proposal) 


* `executer` - The account executing the transaction

### `public void invalidate(name account)` {#classeosio_1_1multisig_1a59e26671d5a02dd2f809b08f56b4d3b8}



Invalidate action allows an `account` to invalidate itself, that is, its name is added to the invalidations table and this table will be cross referenced when exec is performed.


#### Parameters
* `account` - The account invalidating the transaction


# struct `approval` {#structeosio_1_1multisig_1_1approval}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public permission_level level` | 
`public time_point time` | 

## Members

### `public permission_level level` {#structeosio_1_1multisig_1_1approval_1a7de164e0abd4d7ecec9da2f67fe57ece}





### `public time_point time` {#structeosio_1_1multisig_1_1approval_1a07e19daf20f6ca5a96251a0c590cbe31}






# struct `approvals_info` {#structeosio_1_1multisig_1_1approvals__info}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint8_t version` | 
`public name proposal_name` | 
`public std::vector< `[`approval`](#structeosio_1_1multisig_1_1approval)` > requested_approvals` | 
`public std::vector< `[`approval`](#structeosio_1_1multisig_1_1approval)` > provided_approvals` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint8_t version` {#structeosio_1_1multisig_1_1approvals__info_1a7067d083de3933cab8fa1b961e1d810d}





### `public name proposal_name` {#structeosio_1_1multisig_1_1approvals__info_1a76bf340c4c8c42d573664bd0fab91675}





### `public std::vector< `[`approval`](#structeosio_1_1multisig_1_1approval)` > requested_approvals` {#structeosio_1_1multisig_1_1approvals__info_1ad173253fe8f9e34c17117db4baeb41ae}





### `public std::vector< `[`approval`](#structeosio_1_1multisig_1_1approval)` > provided_approvals` {#structeosio_1_1multisig_1_1approvals__info_1a6d57f9792fe5bc8b2cacbf071e1059f8}





### `public inline uint64_t primary_key() const` {#structeosio_1_1multisig_1_1approvals__info_1ac7ec9b713d3a499006ea9ba9030e10b4}






# struct `invalidation` {#structeosio_1_1multisig_1_1invalidation}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public name account` | 
`public time_point last_invalidation_time` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public name account` {#structeosio_1_1multisig_1_1invalidation_1af732aa933c30257e90b28db376af4461}





### `public time_point last_invalidation_time` {#structeosio_1_1multisig_1_1invalidation_1a767f677a40d58d61189e2bfd2bbdff1f}





### `public inline uint64_t primary_key() const` {#structeosio_1_1multisig_1_1invalidation_1a8222535d8511a2cd9667a7c816a169b8}






# struct `old_approvals_info` {#structeosio_1_1multisig_1_1old__approvals__info}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public name proposal_name` | 
`public std::vector< permission_level > requested_approvals` | 
`public std::vector< permission_level > provided_approvals` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public name proposal_name` {#structeosio_1_1multisig_1_1old__approvals__info_1ababd8996161ac79d6889b81a2f1755af}





### `public std::vector< permission_level > requested_approvals` {#structeosio_1_1multisig_1_1old__approvals__info_1ae499fbd3582721ecd3caa4d0ff71737f}





### `public std::vector< permission_level > provided_approvals` {#structeosio_1_1multisig_1_1old__approvals__info_1a1f9c8bcc263e6a6d324c81e1cbbd49f1}





### `public inline uint64_t primary_key() const` {#structeosio_1_1multisig_1_1old__approvals__info_1a3a8558018b8d987b48ae8d0d0935611e}






# struct `proposal` {#structeosio_1_1multisig_1_1proposal}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public name proposal_name` | 
`public std::vector< char > packed_transaction` | 
`public eosio::binary_extension< std::optional< time_point > > earliest_exec_time` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public name proposal_name` {#structeosio_1_1multisig_1_1proposal_1a231b1043a7721d508c07ca6c6b20385b}





### `public std::vector< char > packed_transaction` {#structeosio_1_1multisig_1_1proposal_1af9a7406b371dbf10a78ce56f9f921f84}





### `public eosio::binary_extension< std::optional< time_point > > earliest_exec_time` {#structeosio_1_1multisig_1_1proposal_1ac8b4f63052c9089874b1a0be48bcef07}





### `public inline uint64_t primary_key() const` {#structeosio_1_1multisig_1_1proposal_1ae459219bcd83cd451771b633277519db}






# class `wrap` {#classeosio_1_1wrap}

```
class wrap
  : public contract
```  



The `[eosio.wrap](#classeosio_1_1wrap)` system contract allows block producers to bypass authorization checks or run privileged actions with 15/21 producer approval and thus simplifies block producers superuser actions. It also makes these actions easier to audit.

It does not give block producers any additional powers or privileges that do not already exist within the EOSIO based blockchains. As it is implemented, in an EOSIO based blockchain, 15/21 block producers can change an account's permissions or modify an account's contract code if they decided it is beneficial for the blockchain and community. However, the current method is opaque and leaves undesirable side effects on specific system accounts, and thus the `[eosio.wrap](#classeosio_1_1wrap)`contract solves this matter by providing an easier method of executing important governance actions.

The only action implemented by the `[eosio.wrap](#classeosio_1_1wrap)` system contract is the `exec` action. This action allows for execution of a transaction, which is passed to the `exec` method in the form of a packed transaction in json format via the 'trx' parameter and the `executer` account that executes the transaction. The same `executer` account will also be used to pay the RAM and CPU fees needed to execute the transaction.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public void exec(ignore< name > executer,ignore< transaction > trx)` | 

## Members

### `public void exec(ignore< name > executer,ignore< transaction > trx)` {#classeosio_1_1wrap_1a62a9871067de866d265fd69a5a4eac60}



Execute action.

Execute a transaction while bypassing regular authorization checks.

Preconditions:

* Requires authorization of [eosio.wrap](#classeosio_1_1wrap) which needs to be a privileged account.





Postconditions:

* Deferred transaction RAM usage is billed to 'executer' *






#### Parameters
* `executer` - account executing the transaction, 


* `trx` - the transaction to be executed.


# namespace `eosioboot`



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`class `[``boot``](#classeosioboot_1_1boot)    | 
`struct `[``authority``](#structeosioboot_1_1authority)    | 
`struct `[``key_weight``](#structeosioboot_1_1key__weight)    | 
`struct `[``permission_level_weight``](#structeosioboot_1_1permission__level__weight)    | 
`struct `[``wait_weight``](#structeosioboot_1_1wait__weight)    | 

# class `boot` {#classeosioboot_1_1boot}

```
class boot
  : public eosio::contract
```  



eosio.boot is a extremely minimalistic system contract that only supports the native actions and an activate action that allows activating desired protocol features prior to deploying a system contract with more features such as eosio.bios or eosio.system.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public void onerror(ignore< uint128_t > sender_id,ignore< std::vector< char > > sent_trx)` | 
`public void activate(const eosio::checksum256 & feature_digest)` | 
`public void reqactivated(const eosio::checksum256 & feature_digest)` | 

## Members

### `public void onerror(ignore< uint128_t > sender_id,ignore< std::vector< char > > sent_trx)` {#classeosioboot_1_1boot_1a91777be02b457df0533db35776387b13}



On error action.

Notification of this action is delivered to the sender of a deferred transaction when an objective error occurs while executing the deferred transaction. This action is not meant to be called directly.


#### Parameters
* `sender_id` - the id for the deferred transaction chosen by the sender, 


* `sent_trx` - the deferred transaction that failed.

### `public void activate(const eosio::checksum256 & feature_digest)` {#classeosioboot_1_1boot_1a8159d04a7a5ca851a1c654594bafba5e}



Activates a protocol feature.

Activates a protocol feature


#### Parameters
* `feature_digest` - hash of the protocol feature to activate.

### `public void reqactivated(const eosio::checksum256 & feature_digest)` {#classeosioboot_1_1boot_1a470f541c80fa910ded5dd8c6ab2f2a18}



Asserts that a protocol feature has been activated.

Asserts that a protocol feature has been activated


#### Parameters
* `feature_digest` - hash of the protocol feature to check for activation.


# struct `authority` {#structeosioboot_1_1authority}




Blockchain authority.

An authority is defined by:

* a vector of key_weights (a [key_weight](#structeosioboot_1_1key__weight) is a public key plus a weight),


* a vector of permission_level_weights, (a permission_level is an account name plus a permission name)


* a vector of wait_weights (a [wait_weight](#structeosioboot_1_1wait__weight) is defined by a number of seconds to wait and a weight)


* a threshold value

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint32_t threshold` | 
`public std::vector< `[`key_weight`](#structeosioboot_1_1key__weight)` > keys` | 
`public std::vector< `[`permission_level_weight`](#structeosioboot_1_1permission__level__weight)` > accounts` | 
`public std::vector< `[`wait_weight`](#structeosioboot_1_1wait__weight)` > waits` | 

## Members

### `public uint32_t threshold` {#structeosioboot_1_1authority_1a5b48219c77dcce29612ebbe13e938a0b}





### `public std::vector< `[`key_weight`](#structeosioboot_1_1key__weight)` > keys` {#structeosioboot_1_1authority_1a8a29267ddb0d0f2ce3c26ce76f0ad027}





### `public std::vector< `[`permission_level_weight`](#structeosioboot_1_1permission__level__weight)` > accounts` {#structeosioboot_1_1authority_1a95c344e31f5bfb700d54a989e79d9bdd}





### `public std::vector< `[`wait_weight`](#structeosioboot_1_1wait__weight)` > waits` {#structeosioboot_1_1authority_1a46eb48c5adc47301ee18186e19dc1795}






# struct `key_weight` {#structeosioboot_1_1key__weight}




Weighted key.

A weighted key is defined by a public key and an associated weight.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::public_key key` | 
`public uint16_t weight` | 

## Members

### `public eosio::public_key key` {#structeosioboot_1_1key__weight_1a6249f3c7222da01c56a3fb7d477e0356}





### `public uint16_t weight` {#structeosioboot_1_1key__weight_1a0744bfe820d29f51d77f2d89156f1969}






# struct `permission_level_weight` {#structeosioboot_1_1permission__level__weight}




A weighted permission.

Defines a weighted permission, that is a permission which has a weight associated. A permission is defined by an account name plus a permission name. The weight is going to be used against a threshold, if the weight is equal or greater than the threshold set then authorization will pass.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public permission_level permission` | 
`public uint16_t weight` | 

## Members

### `public permission_level permission` {#structeosioboot_1_1permission__level__weight_1a5354d5a9ed36751c733ca4788058bf34}





### `public uint16_t weight` {#structeosioboot_1_1permission__level__weight_1abe39f569e8b39abfa6a13d786f300369}






# struct `wait_weight` {#structeosioboot_1_1wait__weight}




Wait weight.

A wait weight is defined by a number of seconds to wait for and a weight.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint32_t wait_sec` | 
`public uint16_t weight` | 

## Members

### `public uint32_t wait_sec` {#structeosioboot_1_1wait__weight_1a32fde6e420badc853fafc1981de02c98}





### `public uint16_t weight` {#structeosioboot_1_1wait__weight_1a0940f3b02940bf41ec6b6ce5980ea93a}






# namespace `blockinfo_tester`



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`struct `[``get_latest_block_batch_info``](#structsystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1_1get__latest__block__batch__info)    | Input data structure for `[get_latest_block_batch_info](#structsystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1_1get__latest__block__batch__info)` RPC.
`struct `[``latest_block_batch_info_result``](#structsystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1_1latest__block__batch__info__result)    | Output data structure for `[get_latest_block_batch_info](#structsystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1_1get__latest__block__batch__info)` RPC.

# struct `get_latest_block_batch_info` {#structsystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1_1get__latest__block__batch__info}


Input data structure for `[get_latest_block_batch_info](#structsystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1_1get__latest__block__batch__info)` RPC.

Use this struct as the input for a call to the `[get_latest_block_batch_info](#structsystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1_1get__latest__block__batch__info)` RPC. That call will return the result as the `[latest_block_batch_info_result](#structsystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1_1latest__block__batch__info__result)` struct.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint32_t batch_start_height_offset` | 
`public uint32_t batch_size` | 

## Members

### `public uint32_t batch_start_height_offset` {#structsystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1_1get__latest__block__batch__info_1ac601553260c24e5fe6ed17828e4a41ef}





### `public uint32_t batch_size` {#structsystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1_1get__latest__block__batch__info_1a82ebd86affa47e2f8024b9083374ed8a}






# struct `latest_block_batch_info_result` {#structsystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1_1latest__block__batch__info__result}


Output data structure for `[get_latest_block_batch_info](#structsystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1_1get__latest__block__batch__info)` RPC.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public std::optional< `[`block_batch_info`](#structeosiosystem_1_1block__info_1_1block__batch__info)` > result` | 
`public `[`varint`](#namespacesystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1a3b3bb00116a9d1c649c8cd6b9d8eb43b)` error_code` | 
`public inline bool has_error() const` | 
`public inline `[`error_code_enum`](#structsystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1_1latest__block__batch__info__result_1aeb9f839d5c51488952f8f1381f89a64a)` get_error() const` | 

## Members

### `public std::optional< `[`block_batch_info`](#structeosiosystem_1_1block__info_1_1block__batch__info)` > result` {#structsystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1_1latest__block__batch__info__result_1a6bef31839eb6c99ff3bb5f69bbb8b7a5}





### `public `[`varint`](#namespacesystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1a3b3bb00116a9d1c649c8cd6b9d8eb43b)` error_code` {#structsystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1_1latest__block__batch__info__result_1aa3bf20610c6a68ed0a5e3261cbdb5808}





### `public inline bool has_error() const` {#structsystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1_1latest__block__batch__info__result_1a8c36a77acb6436c335c075939e5ffc0d}





### `public inline `[`error_code_enum`](#structsystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1_1latest__block__batch__info__result_1aeb9f839d5c51488952f8f1381f89a64a)` get_error() const` {#structsystem__contracts_1_1testing_1_1test__contracts_1_1blockinfo__tester_1_1latest__block__batch__info__result_1a98a9efbd6bbda91e666bd57e16095adb}






# namespace `anonymous_namespace{block_info.cpp}`



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------

# namespace `anonymous_namespace{blockinfo_tester.cpp}`



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------

# class `powup_results` {#classpowup__results}

```
class powup_results
  : private eosio::contract
```  



The action `powupresult` is a no-op. It is added as an inline convenience action to `powerup` reservation. This inline convenience action does not have any effect, however, its data includes the result of the parent action and appears in its trace.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public void powupresult(const asset & fee,const int64_t powup_net,const int64_t powup_cpu)` | 

## Members

### `public void powupresult(const asset & fee,const int64_t powup_net,const int64_t powup_cpu)` {#classpowup__results_1a92bf8a4505d35524f2708a7984d5df59}



powupresult action.


#### Parameters
* `fee` - powerup fee amount 


* `powup_net` - amount of powup NET tokens 


* `powup_cpu` - amount of powup CPU tokens


# class `rex_results` {#classrex__results}

```
class rex_results
  : private eosio::contract
```  



The actions `buyresult`, `sellresult`, `rentresult`, and `orderresult` of `rex.results` are all no-ops. They are added as inline convenience actions to `rentnet`, `rentcpu`, `buyrex`, `unstaketorex`, and `sellrex`. An inline convenience action does not have any effect, however, its data includes the result of the parent action and appears in its trace.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public void buyresult(const asset & rex_received)` | 
`public void sellresult(const asset & proceeds)` | 
`public void orderresult(const name & owner,const asset & proceeds)` | 
`public void rentresult(const asset & rented_tokens)` | 

## Members

### `public void buyresult(const asset & rex_received)` {#classrex__results_1a955ccff2c80578de034ee8c16bb29b47}



Buyresult action.


#### Parameters
* `rex_received` - amount of tokens used in buy order

### `public void sellresult(const asset & proceeds)` {#classrex__results_1a2fdb9897a541ee103c618e8b311afab9}



Sellresult action.


#### Parameters
* `proceeds` - amount of tokens used in sell order

### `public void orderresult(const name & owner,const asset & proceeds)` {#classrex__results_1aadee1538b7d17cbbe6efd8438fa46ae1}



Orderresult action.


#### Parameters
* `owner` - the owner of the order 


* `proceeds` - amount of tokens used in order

### `public void rentresult(const asset & rented_tokens)` {#classrex__results_1ab9cb69866c17d4344e0d50ea810ecef4}



Rentresult action.


#### Parameters
* `rented_tokens` - amount of rented tokens


# class `sendinline` {#classsendinline}

```
class sendinline
  : public eosio::contract
```  



`eosio.code` is a virtual permission (there is no private or public key associated with it). Therefore, this test tests how `eosio.msig` contract reacts to a smart contract submitting a proposal and approving/unnapproving itself.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public inline void send(eosio::name contract,eosio::name action_name,std::vector< eosio::permission_level > auths,std::vector< char > payload)` | 

## Members

### `public inline void send(eosio::name contract,eosio::name action_name,std::vector< eosio::permission_level > auths,std::vector< char > payload)` {#classsendinline_1a9302e043cf61d0320016d60788389ac8}






# struct `accfunds` {#structaccfunds}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | нулевой идентификатор изменять нельзя вообще
`public eosio::name coopname` | идентификатор кооператива
`public eosio::name contract` | внешний контракт, которому передано управление фондом
`public std::string name` | название фонда
`public std::string description` | описание фонда
`public uint64_t percent` | 1000000 == 100%,
`public eosio::asset available` | количество средств, накопленных в фонде
`public eosio::asset withdrawed` | количество средств, изъятых из фонда накопления
`public inline uint64_t primary_key() const` | Первичный ключ для индексации по идентификатору фонда

## Members

### `public uint64_t id` {#structaccfunds_1adcca9a94251183d055c7ab6a8d9d6f0c}

нулевой идентификатор изменять нельзя вообще



### `public eosio::name coopname` {#structaccfunds_1a589dfd35cf6f0d85513f31be12770b1b}

идентификатор кооператива



### `public eosio::name contract` {#structaccfunds_1aab16a8f0030602551948b7f9f5c47ba3}

внешний контракт, которому передано управление фондом



### `public std::string name` {#structaccfunds_1afb03c0dac27d7eb7d8464128662a33df}

название фонда



### `public std::string description` {#structaccfunds_1a607854b22be889d87c61271a53b62dad}

описание фонда



### `public uint64_t percent` {#structaccfunds_1afb524b5b54ad69d9f093bff456a6ce2f}

1000000 == 100%,



### `public eosio::asset available` {#structaccfunds_1a8fa753c862acaa0dc47a7987dd40febc}

количество средств, накопленных в фонде



### `public eosio::asset withdrawed` {#structaccfunds_1a7e3aff526dae580046c35c1619c64fad}

количество средств, изъятых из фонда накопления



### `public inline uint64_t primary_key() const` {#structaccfunds_1ae19e8876823cb6f1e5543314f7e8153f}

Первичный ключ для индексации по идентификатору фонда




# struct `accounts` {#structaccounts}


Структура, представляющая учетные записи аккаунтов.

Эта структура хранит информацию о пользователях аккаунта и их статусе, репутации и других параметрах.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name username` | Имя аккаунта гостя. Имя пользователя в системе.
`public eosio::name referer` | Имя аккаунта, который был реферером при регистрации.
`public eosio::name registrator` | Имя аккаунта регистратора, который создал этот аккаунт.
`public eosio::name type` | Тип аккаунта: user (пользователь) \| org (организация).
`public eosio::name status` | Статус аккаунта:
`public std::string meta` | Дополнительная мета-информация о аккаунте.
`public uint64_t reputation` | Репутация аккаунта
`public eosio::asset registration_amount` | Количество токенов, которое требуется для регистрации.
`public eosio::time_point_sec registered_at` | Время регистрации аккаунта.
`public eosio::time_point_sec signature_expires_at` | Время истечения срока действия подписи аккаунта.
`public inline uint64_t primary_key() const` | Возвращает первичный ключ учетной записи аккаунта.
`public inline uint64_t by_referer() const` | Возвращает ключ по рефереру.
`public inline uint64_t by_type() const` | Возвращает ключ по типу аккаунта.
`public inline uint64_t by_status() const` | Возвращает ключ по статусу аккаунта.
`public inline uint64_t by_registr() const` | Возвращает ключ по регистратору.
`public inline uint64_t by_registered_at() const` | 

## Members

### `public eosio::name username` {#structaccounts_1a05876c5415522c3ff8a3d8e8a783d6a7}

Имя аккаунта гостя. Имя пользователя в системе.



### `public eosio::name referer` {#structaccounts_1a42c6052506d3b2d89bbbb95913591809}

Имя аккаунта, который был реферером при регистрации.



### `public eosio::name registrator` {#structaccounts_1a432b7b14e9da714d2de1546dbbd74a7e}

Имя аккаунта регистратора, который создал этот аккаунт.



### `public eosio::name type` {#structaccounts_1af1bfc2d1e94ebb71271c0ce836558160}

Тип аккаунта: user (пользователь) | org (организация).



### `public eosio::name status` {#structaccounts_1a0498f8a07ad571d52548a076370d9253}

Статус аккаунта:



### `public std::string meta` {#structaccounts_1aeecfd8c0939c8e7de20db6126b965bd9}

Дополнительная мета-информация о аккаунте.



### `public uint64_t reputation` {#structaccounts_1aaa9631adef3e542db5ddc1e2388dfd61}

Репутация аккаунта



### `public eosio::asset registration_amount` {#structaccounts_1a9aa73b3535b162d4dbcdb5c581146ce2}

Количество токенов, которое требуется для регистрации.



### `public eosio::time_point_sec registered_at` {#structaccounts_1a5ab584c673fa71b1aebd8eb3ec4e69e5}

Время регистрации аккаунта.



### `public eosio::time_point_sec signature_expires_at` {#structaccounts_1a38b262d4ca61e4cecb3f1eab1a5d3887}

Время истечения срока действия подписи аккаунта.



### `public inline uint64_t primary_key() const` {#structaccounts_1ac5356720544cfbfc9b82d829f4fa4f37}

Возвращает первичный ключ учетной записи аккаунта.

#### Returns
uint64_t - первичный ключ, равный значению имени аккаунта. return username - primary_key

### `public inline uint64_t by_referer() const` {#structaccounts_1a779b22d35ce6564c3b2e34e03b77aa29}

Возвращает ключ по рефереру.

#### Returns
uint64_t - ключ, равный значению имени реферера.

### `public inline uint64_t by_type() const` {#structaccounts_1a336640e99b5ae6106b3715cae302ff46}

Возвращает ключ по типу аккаунта.

#### Returns
uint64_t - ключ, равный значению типа аккаунта.

### `public inline uint64_t by_status() const` {#structaccounts_1ae34d3eff438789f8e09b1fef2eed2af2}

Возвращает ключ по статусу аккаунта.

#### Returns
uint64_t - ключ, равный значению статуса аккаунта.

### `public inline uint64_t by_registr() const` {#structaccounts_1a88ed8c7045efdf796cfadb54e91ef284}

Возвращает ключ по регистратору.

#### Returns
uint64_t - ключ, равный значению имени регистратора.

### `public inline uint64_t by_registered_at() const` {#structaccounts_1a7cedd43173876881e9122f35509c1e24}






# struct `address` {#structaddress}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public std::string latitude` | 
`public std::string longitude` | 
`public std::string country` | 
`public std::string state` | 
`public std::string city` | 
`public std::string district` | 
`public std::string street` | 
`public std::string house_number` | 
`public std::string building_section` | 
`public std::string unit_number` | 
`public std::string directions` | 
`public std::string phone_number` | 
`public std::string business_hours` | 

## Members

### `public std::string latitude` {#structaddress_1accd30a3285ef1acb4f46c3d27e6e00d5}





### `public std::string longitude` {#structaddress_1aeb0eaac4f41de68f2e2b2c08c7d4b111}





### `public std::string country` {#structaddress_1a6d3c5423727d5ca2340631a7ddde1022}





### `public std::string state` {#structaddress_1a706a208b78c00371a9922fa00efdd4e1}





### `public std::string city` {#structaddress_1ac6aa41c02d77a9ed850d1b54cc10f379}





### `public std::string district` {#structaddress_1a28d18e08d44ef8531ef15f6ad3f44f45}





### `public std::string street` {#structaddress_1a732f9ae03c0b36327b1d60ffebc81415}





### `public std::string house_number` {#structaddress_1ab2decd423a20551450767c384ea25f9c}





### `public std::string building_section` {#structaddress_1aa714c4baddf02fffea8834660e39118a}





### `public std::string unit_number` {#structaddress_1acc87a21438b16fe0b9234fb81c5cdd65}





### `public std::string directions` {#structaddress_1aebec737c9fa2d67ff39b8a0abe0d1a78}





### `public std::string phone_number` {#structaddress_1ad5de7798476a236539e0bd28b73f9b88}





### `public std::string business_hours` {#structaddress_1adf8eb6a178e4938ce78a51a5acba0183}






# struct `addresses` {#structaddresses}


Структура, представляющая адреса кооператива.

Эта структура содержит информацию о адресах кооператива, которые используются как точки приёма-выдачи товаров на кооплейсе.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name coopname` | 
`public eosio::name cooplate` | 
`public `[`address`](#structaddress)` data` | 
`public std::string meta` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t bycooplate() const` | 

## Members

### `public uint64_t id` {#structaddresses_1a1861499bb0e9be9241b4e08f40b544b8}





### `public eosio::name coopname` {#structaddresses_1a88027a9691f6ce27cc71ed720455413d}





### `public eosio::name cooplate` {#structaddresses_1a7faceb467e12773662fba5f289221b21}





### `public `[`address`](#structaddress)` data` {#structaddresses_1a198b932b99b9881a2bb8d5aae2b82f74}





### `public std::string meta` {#structaddresses_1af318fa67c0c2a366ed24434f5fb27706}





### `public inline uint64_t primary_key() const` {#structaddresses_1a505c9476012d6d24ae6379ebccd29c4d}





### `public inline uint64_t bycooplate() const` {#structaddresses_1a82b8881b433938145f8d9af152cc19c3}






# struct `authority` {#structauthority}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint32_t threshold` | 
`public std::vector< `[`key_weight`](#structkey__weight)` > keys` | 
`public std::vector< `[`permission_level_weight`](#structpermission__level__weight)` > accounts` | 
`public std::vector< `[`wait_weight`](#structwait__weight)` > waits` | 

## Members

### `public uint32_t threshold` {#structauthority_1ab9fbc4a419012844e80ffebb57d76971}





### `public std::vector< `[`key_weight`](#structkey__weight)` > keys` {#structauthority_1a7538d1edffef8dd8ebe41758fbc4fdd8}





### `public std::vector< `[`permission_level_weight`](#structpermission__level__weight)` > accounts` {#structauthority_1a0929cd72740ad0953b59fdc1df188b60}





### `public std::vector< `[`wait_weight`](#structwait__weight)` > waits` {#structauthority_1a809a2560b802fdd64dab31f7193a045b}






# struct `automator` {#structautomator}


Таблица автоматизированных действий

Таблица содержит набор действий, которые член совета автоматизировал.

Таблица хранится в области памяти с именем аккаунта: `_soviet` и скоупом: `coopname`

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | Уникальный идентификатор автоматизированного действия
`public eosio::name coopname` | Имя кооператива, к которому относится данное автоматизированное действие
`public uint64_t board_id` | Идентификатор совета, который автоматизировал данное действие
`public eosio::name member` | Член совета, который автоматизировал данное действие
`public eosio::name action_type` | Тип автоматизированного действия
`public eosio::name permission_name` | Имя разрешения для авторизации действия
`public std::string encrypted_private_key` | Зашифрованный приватный ключ для авторизации действия
`public inline uint64_t primary_key() const` | 
`public inline uint128_t by_member_and_action_type() const` | Индекс по члену совета и типу действия
`public inline uint64_t by_action() const` | Индекс по типу действия

## Members

### `public uint64_t id` {#structautomator_1a9389163367c7579bd9725a83bb27cef6}

Уникальный идентификатор автоматизированного действия



### `public eosio::name coopname` {#structautomator_1a020dc5169affb64d4df454bd10d8176f}

Имя кооператива, к которому относится данное автоматизированное действие



### `public uint64_t board_id` {#structautomator_1af9991df3151a7d8205c4205caf993a92}

Идентификатор совета, который автоматизировал данное действие



### `public eosio::name member` {#structautomator_1a7bb8cdadaa140b733497e80fdecf49ab}

Член совета, который автоматизировал данное действие



### `public eosio::name action_type` {#structautomator_1afcb1a4bcd330d3e3cb50fe5f977cc069}

Тип автоматизированного действия



### `public eosio::name permission_name` {#structautomator_1a4d145638d25608ddf9c5d737f9fe6da2}

Имя разрешения для авторизации действия



### `public std::string encrypted_private_key` {#structautomator_1a94be657d9e8d176482eee2a344bb6665}

Зашифрованный приватный ключ для авторизации действия



### `public inline uint64_t primary_key() const` {#structautomator_1af52fe34bcc89bb7d8c7c1805a2afd76a}





### `public inline uint128_t by_member_and_action_type() const` {#structautomator_1a7f20068251bdaec35114f83497ca3411}

Индекс по члену совета и типу действия



### `public inline uint64_t by_action() const` {#structautomator_1a7690ecf690fe20aaed35f277330e2e5f}

Индекс по типу действия




# struct `autosigner` {#structautosigner}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t decision_id` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint64_t decision_id` {#structautosigner_1a2d1155e3a9d63342575c616944f30a4c}





### `public inline uint64_t primary_key() const` {#structautosigner_1a2487244a79f8fdf7fa6e67d8821c960f}






# struct `balances_base` {#structbalances__base}


Структура таблицы баланса.

Структура, представляющая записи баланса в контрактах. Используется при отправке токенов в контракты _marketplace или _registrator.

Таблица хранится в области памяти пользователя (_registrator | _marketplace, username)

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name contract` | 
`public eosio::asset quantity` | 
`public inline uint64_t primary_key() const` | 
`public inline uint128_t byconsym() const` | 

## Members

### `public uint64_t id` {#structbalances__base_1a2069a54454c59cae6ac2a977002bc8ce}



идентификатор баланса

### `public eosio::name contract` {#structbalances__base_1acebf235da280f8d16533b6913912da41}



имя контракта токена

### `public eosio::asset quantity` {#structbalances__base_1a1c3e3dfd8436a6e847dfbe620ba26c23}



количество токенов на балансе

### `public inline uint64_t primary_key() const` {#structbalances__base_1a242d9cbeea53e487eff7eb9349327931}



return id - primary_key

### `public inline uint128_t byconsym() const` {#structbalances__base_1af43d841c9f521f6fbbed7c943d4fd366}



возвращает уникальный индекс, сформированный из значения contract и символа токена


# struct `bank` {#structbank}


Структура, представляющая информацию о банке.

Эта структура содержит информацию о номере расчётного счёта банка, времени создания, времени последнего обновления и статусе активности.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public std::string account` | Номер расчётного счёта банка.
`public eosio::time_point_sec created_at` | Время создания записи.
`public eosio::time_point_sec last_update` | Время последнего обновления записи.
`public bool is_active` | Флаг, указывающий, активен ли банк.

## Members

### `public std::string account` {#structbank_1ac222eff37f2528232734f3c6b4425b67}

Номер расчётного счёта банка.



### `public eosio::time_point_sec created_at` {#structbank_1a20b362142d5c764f45432e0efd1f36e0}

Время создания записи.



### `public eosio::time_point_sec last_update` {#structbank_1a9d3a91fdf7922097c93148ffdf4c815d}

Время последнего обновления записи.



### `public bool is_active` {#structbank_1a5364d9937ea115f7b721270e942505b0}

Флаг, указывающий, активен ли банк.




# struct `board_member` {#structboard__member}


Структура, представляющая членов доски.

Эта структура содержит информацию о членах доски, включая их уникальные имена, статус участия в голосовании, название должности и позицию в доске.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name username` | Уникальное имя члена доски.
`public bool is_voting` | Флаг, указывающий, имеет ли член доски право голоса.
`public std::string position_title` | Название должности члена доски.
`public eosio::name position` | Позиция члена в доске:

## Members

### `public eosio::name username` {#structboard__member_1adf5286ff237471ceabc65d4d091972d0}

Уникальное имя члена доски.



### `public bool is_voting` {#structboard__member_1a20b8902343fc67a6f8a0665a4e40b330}

Флаг, указывающий, имеет ли член доски право голоса.



### `public std::string position_title` {#structboard__member_1a2961218c9a14c7125a53ade0c15e5598}

Название должности члена доски.



### `public eosio::name position` {#structboard__member_1a10fea881563dbbbbee4ad11c3756e7ec}

Позиция члена в доске:




# struct `boards` {#structboards}


Структура советов кооперативов

Эта структура содержит информацию о досках кооператива, их типе, названии, описании и членах.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | Уникальный идентификатор доски.
`public eosio::name type` | Тип доски:
`public std::string name` | Название доски.
`public std::string description` | Описание доски.
`public std::vector< `[`board_member`](#structboard__member)` > members` | Список членов доски.
`public eosio::time_point_sec created_at` | Время создания доски.
`public eosio::time_point_sec last_update` | Время последнего обновления информации о доске.
`public inline uint64_t primary_key() const` | Возвращает первичный ключ доски.
`public inline uint64_t by_type() const` | Возвращает ключ для индексации по типу доски.
`public inline bool is_valid_member(eosio::name member) const` | 
`public inline bool is_voting_member(eosio::name member) const` | 
`public inline bool is_valid_chairman(eosio::name chairman) const` | 
`public inline eosio::name get_chairman() const` | 
`public inline bool is_valid_secretary(eosio::name secretary) const` | 
`public inline bool has_voting_right(eosio::name member) const` | 
`public inline uint64_t get_members_count() const` | 

## Members

### `public uint64_t id` {#structboards_1a4902e87f13247db8f837629544f12a6a}

Уникальный идентификатор доски.



### `public eosio::name type` {#structboards_1a277bb2ea397a8b6076584f07f2c4432f}

Тип доски:



### `public std::string name` {#structboards_1ad9a12ccbc9bdbabfe1dbd5f7cffdb06f}

Название доски.



### `public std::string description` {#structboards_1ae8f0825aa67789f638169abf513ce674}

Описание доски.



### `public std::vector< `[`board_member`](#structboard__member)` > members` {#structboards_1a784be337829b64e7caaa6c6657f5c262}

Список членов доски.



### `public eosio::time_point_sec created_at` {#structboards_1add47e6975c343415f278a0414c714066}

Время создания доски.



### `public eosio::time_point_sec last_update` {#structboards_1acaad31053c6dc00a9b99faaece7213bc}

Время последнего обновления информации о доске.



### `public inline uint64_t primary_key() const` {#structboards_1a8a887a056d41000b8183975cc5fe0c91}

Возвращает первичный ключ доски.

#### Returns
uint64_t - первичный ключ, равный уникальному идентификатору доски.

### `public inline uint64_t by_type() const` {#structboards_1a62601ce5131f1d2be8f17c3df5866f38}

Возвращает ключ для индексации по типу доски.

#### Returns
uint64_t - ключ, равный значению типа доски.

### `public inline bool is_valid_member(eosio::name member) const` {#structboards_1a3108c80a73de20b8ad7a776e108abe26}





### `public inline bool is_voting_member(eosio::name member) const` {#structboards_1acfe79079ce96ef59fea0f86c95506cb8}





### `public inline bool is_valid_chairman(eosio::name chairman) const` {#structboards_1ad1e613d161e0dfb3bee0b043b71c13db}





### `public inline eosio::name get_chairman() const` {#structboards_1ad04db86aca0d3390cbf18c1696818f16}





### `public inline bool is_valid_secretary(eosio::name secretary) const` {#structboards_1a19e6b2ff62948b4922ea2ea945b5083a}





### `public inline bool has_voting_right(eosio::name member) const` {#structboards_1a151ec00b76ae9f7c7fdc1e4dee7f0412}





### `public inline uint64_t get_members_count() const` {#structboards_1a3eb726cb1d52d85288567c3e5848828e}






# struct `changes` {#structchanges}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public uint64_t exchange_id` | 
`public uint64_t contribution_product_decision_id` | 
`public uint64_t return_product_decision_id` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t byexchange() const` | 

## Members

### `public uint64_t id` {#structchanges_1a0c37e4322ad12ebbb02e2f3d68a0aadd}





### `public uint64_t exchange_id` {#structchanges_1a3c3ab08bf6376ebb8b88d5aa808d0bd5}





### `public uint64_t contribution_product_decision_id` {#structchanges_1a354d3fdf5288abccfe91a9197d34744c}





### `public uint64_t return_product_decision_id` {#structchanges_1a1f53ddc615f0b3ec60d9e332d94286fe}





### `public inline uint64_t primary_key() const` {#structchanges_1a133052a9a076552c4760f1976bf4d1ee}





### `public inline uint64_t byexchange() const` {#structchanges_1ae81e67349741bfa93d1367311b3b99c4}






# struct `counts` {#structcounts}

```
struct counts
  : public counts_base
```  





## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------

## Members


# struct `counts_base` {#structcounts__base}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name key` | 
`public eosio::name secondary_key` | 
`public uint64_t value` | 
`public inline uint64_t primary_key() const` | 
`public inline uint128_t keyskey() const` | 
`public inline uint128_t keyvalue() const` | 

## Members

### `public eosio::name key` {#structcounts__base_1a190877bf5f40a7a357007574c5ecc1d7}





### `public eosio::name secondary_key` {#structcounts__base_1a54989b6bb7004b9dce497350707fce21}





### `public uint64_t value` {#structcounts__base_1a7e8ded653625d24a13ae3b651c42dd5a}





### `public inline uint64_t primary_key() const` {#structcounts__base_1a776ade72cbcce230ac5dd0c5d45b3578}



return id - primary_key

### `public inline uint128_t keyskey() const` {#structcounts__base_1acabcbd93c1b35dca98c77646bbfda21d}



(contract, blocked_now.symbol) - комбинированный secondary_key для получения курса по имени выходного контракта и символу

### `public inline uint128_t keyvalue() const` {#structcounts__base_1a79af91982641a07988d9cf8f01dc1fd1}



(contract, blocked_now.symbol) - комбинированный secondary_key для получения курса по имени выходного контракта и символу


# struct `decisions` {#structdecisions}


Структура, представляющая решения кооператива.

Эта структура содержит информацию о решениях, включая уникальный идентификатор, имя кооператива, тип решения, идентификатор карточки, списки голосов "за" и "против", а также различные флаги состояния решения.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | Уникальный идентификатор решения.
`public eosio::name coopname` | Имя кооператива, связанного с решением.
`public eosio::name username` | Имя пользователя, связанного с решением.
`public eosio::name type` | Тип решения: // joincoop \| change \| ...
`public uint64_t batch_id` | Идентификатор карточки, связанной с типом решения.
`public std::vector< eosio::name > votes_for` | Список имен, голосовавших "за" решение.
`public std::vector< eosio::name > votes_against` | Список имен, голосовавших "против" решения.
`public bool validated` | Сигнальный флаг, указывающий, что администратор подтверждает валидность решения.
`public bool approved` | Сигнальный флаг, указывающий, что решение советом принято.
`public bool authorized` | Флаг, указывающий, что получена авторизация председателя после голосования и валидации до исполнения.
`public `[`document`](#structdocument)` authorization` | Документ подписанного решения председателем
`public eosio::time_point_sec created_at` | Время создания карточки решения.
`public inline uint64_t primary_key() const` | Возвращает первичный ключ решения.
`public inline uint64_t by_secondary() const` | Возвращает ключ для индексации по идентификатору карточки.
`public inline uint64_t bytype() const` | Возвращает ключ для индексации по типу решения.
`public inline uint64_t byapproved() const` | Возвращает ключ для индексации по статусу "принято".
`public inline uint64_t byvalidated() const` | Возвращает ключ для индексации по статусу "подтверждено".
`public inline uint64_t byauthorized() const` | Возвращает ключ для индексации по статусу "авторизовано".
`public inline void check_for_any_vote_exist(eosio::name member) const` | 
`public inline std::pair< uint64_t, uint64_t > get_votes_count() const` | 

## Members

### `public uint64_t id` {#structdecisions_1aa61841427ff58ac67bf67d16c00c0250}

Уникальный идентификатор решения.



### `public eosio::name coopname` {#structdecisions_1a7a7e7d1969b2cb6de6b85f88e9056f64}

Имя кооператива, связанного с решением.



### `public eosio::name username` {#structdecisions_1ad74a3f563f26f85d9333dbb2cc040843}

Имя пользователя, связанного с решением.



### `public eosio::name type` {#structdecisions_1a7a8e39491d6be766cd3c4fa1fadafbc0}

Тип решения: // joincoop | change | ...



### `public uint64_t batch_id` {#structdecisions_1a4a5595962b6b67447b283aed64d24b87}

Идентификатор карточки, связанной с типом решения.



### `public std::vector< eosio::name > votes_for` {#structdecisions_1a8b734c20fd41967766efcd38cf7c0580}

Список имен, голосовавших "за" решение.



### `public std::vector< eosio::name > votes_against` {#structdecisions_1a944df451a915494bc936415e24d39abe}

Список имен, голосовавших "против" решения.



### `public bool validated` {#structdecisions_1adface34e69e2468b2986e6d395794a1a}

Сигнальный флаг, указывающий, что администратор подтверждает валидность решения.



### `public bool approved` {#structdecisions_1a302e21ff37fe798e2b908a4fdd1bb7bb}

Сигнальный флаг, указывающий, что решение советом принято.



### `public bool authorized` {#structdecisions_1a980288ea11bd9b954357ddf4a094b4f3}

Флаг, указывающий, что получена авторизация председателя после голосования и валидации до исполнения.



### `public `[`document`](#structdocument)` authorization` {#structdecisions_1a2016e7425a2396ac98ca0dfee2603f08}

Документ подписанного решения председателем



### `public eosio::time_point_sec created_at` {#structdecisions_1ae90c18d643b92b8afc17394ab666aa5e}

Время создания карточки решения.



### `public inline uint64_t primary_key() const` {#structdecisions_1a7393d2d0ce88224d46edaa7973fb41dd}

Возвращает первичный ключ решения.

#### Returns
uint64_t - первичный ключ, равный уникальному идентификатору решения.

### `public inline uint64_t by_secondary() const` {#structdecisions_1a9ec7ed6dab56a52187a4dcbd430b1d48}

Возвращает ключ для индексации по идентификатору карточки.

#### Returns
uint64_t - ключ, равный идентификатору карточки.

### `public inline uint64_t bytype() const` {#structdecisions_1a87937d6b4c5c378d3e3ac77141b11394}

Возвращает ключ для индексации по типу решения.

#### Returns
uint64_t - ключ, равный значению типа решения.

### `public inline uint64_t byapproved() const` {#structdecisions_1a6cadd228516907f8eb55f915eb03d4b8}

Возвращает ключ для индексации по статусу "принято".

#### Returns
uint64_t - ключ, равный статусу "принято" (1, если решение принято, иначе 0).

### `public inline uint64_t byvalidated() const` {#structdecisions_1a9661d6706d87e72e74ad3590620e5009}

Возвращает ключ для индексации по статусу "подтверждено".

#### Returns
uint64_t - ключ, равный статусу "подтверждено" (1, если решение подтверждено, иначе 0).

### `public inline uint64_t byauthorized() const` {#structdecisions_1ab166604702a060e0a03e885efa8c14b9}

Возвращает ключ для индексации по статусу "авторизовано".

#### Returns
uint64_t - ключ, равный статусу "авторизовано" (1, если решение авторизовано, иначе 0).

### `public inline void check_for_any_vote_exist(eosio::name member) const` {#structdecisions_1abe41b47952071a0589bf968b5f9cc6f6}





### `public inline std::pair< uint64_t, uint64_t > get_votes_count() const` {#structdecisions_1a23e686cd085c2be8cf671d55ffc8a20c}






# struct `deposits` {#structdeposits}


Таблица `deposits` отслеживает депозиты в контракте GATEWAY.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name username` | 
`public eosio::name coopname` | 
`public eosio::name type` | 
`public eosio::name token_contract` | 
`public eosio::asset quantity` | 
`public eosio::name status` | 
`public std::string link` | 
`public std::string memo` | 
`public eosio::time_point_sec expired_at` | Время истечения срока давности
`public inline uint64_t primary_key() const` | 
`public inline uint64_t by_username() const` | 
`public inline uint64_t by_status() const` | 
`public inline uint64_t by_expired() const` | 

## Members

### `public uint64_t id` {#structdeposits_1a5c45aae9c52f01084bc8aaa17845946f}



Уникальный идентификатор записи депозита

### `public eosio::name username` {#structdeposits_1a900766dd7cb6ae871e5a02dc23aa298f}



Имя аккаунта пользователя, совершившего депозит

### `public eosio::name coopname` {#structdeposits_1a8f1ce91fc16f7ead9fe1381b820236ab}



Имя аккаунта кооператива, в контексте которого совершается депозит

### `public eosio::name type` {#structdeposits_1af7050767f2cc4c041b06717b77457c29}



Тип взноса (deposit | initial)

### `public eosio::name token_contract` {#structdeposits_1a7bb75856564899a414be208947bffc7d}



Имя аккаунта контракта токена для депозита

### `public eosio::asset quantity` {#structdeposits_1ae892cb9379aeb7afd09539fac520eae9}



Количество средств во внутренней валюте

### `public eosio::name status` {#structdeposits_1af09ca4dfb7c381c557cc852ae2e4e119}



Статус депозита

### `public std::string link` {#structdeposits_1a96fcef18e7a0632efe0f15ae432cdacf}



Ссылка на дополнительную информацию или внешние данные

### `public std::string memo` {#structdeposits_1aeb880b7a35cbb9aee64e87c432d190fc}



Примечание к депозиту

### `public eosio::time_point_sec expired_at` {#structdeposits_1a57fc3f06ba682c23325da1c2f0b0f6a2}

Время истечения срока давности



### `public inline uint64_t primary_key() const` {#structdeposits_1a0989b88356d889dfdeb17eb10ccd3471}



Возвращает id как первичный ключ

### `public inline uint64_t by_username() const` {#structdeposits_1a3c25feb661e1accede7aa739f26052f1}



Индекс по имени пользователя

### `public inline uint64_t by_status() const` {#structdeposits_1a39a21fbb51377a5500950eb5401bd03d}



Индекс по статусу депозита

### `public inline uint64_t by_expired() const` {#structdeposits_1a4bdefd36c0695c406576e41187f347a9}



Индекс по статусу истечения


# struct `document` {#structdocument}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public std::string hash` | 
`public std::string pkey` | 
`public std::string sign` | 
`public std::string meta` | 

## Members

### `public std::string hash` {#structdocument_1a974c7e2fc7d1331789ef2fadffe810d5}





### `public std::string pkey` {#structdocument_1a5ac94be6f1dab06a1de785c6cb7be25b}





### `public std::string sign` {#structdocument_1ad4244e8a2d23974528c6dc33bfb08794}





### `public std::string meta` {#structdocument_1ad6e940cca29c2ca96fb1f740d3f2dc90}






# struct `drafts` {#structdrafts}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public uint64_t registry_id` | 
`public eosio::name creator` | 
`public std::vector< eosio::name > actions` | 
`public uint64_t version` | 
`public uint64_t default_translation_id` | 
`public std::string title` | 
`public std::string description` | 
`public std::string context` | 
`public std::string model` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t by_registry_id() const` | 

## Members

### `public uint64_t id` {#structdrafts_1ae4f6a2e4ba90a7043096bd4e794d6bf3}





### `public uint64_t registry_id` {#structdrafts_1a3c19fc9662a19c50bfeff598f8a1449c}





### `public eosio::name creator` {#structdrafts_1a8df4fd1bcc1fee81f5c4740aa32cdbdf}





### `public std::vector< eosio::name > actions` {#structdrafts_1a25684af468d00487f79b9cfd27893706}





### `public uint64_t version` {#structdrafts_1a50a0407cbd62f29e3e7d5aac8777166f}





### `public uint64_t default_translation_id` {#structdrafts_1ab4646bfd78c923b34a1be693aff7610a}





### `public std::string title` {#structdrafts_1aafbad8f41421d0738aae5e7c45898c09}





### `public std::string description` {#structdrafts_1a2272b3e60f6641b0d1da65099f765c25}





### `public std::string context` {#structdrafts_1aa55f776ebd674d1c19cb20e4b8643aef}





### `public std::string model` {#structdrafts_1aa9558aa8a79638c438c87738271c3a54}





### `public inline uint64_t primary_key() const` {#structdrafts_1a9fd3017e1f4512f4d43dcc432b761d70}





### `public inline uint64_t by_registry_id() const` {#structdrafts_1aa80fbfe183450b1a52993a71d7e7ce92}






# struct `exchange` {#structexchange}


Таблица обменов для контракта "marketplace"

Эта таблица используется для хранения информации об обменных заявках в системе.

#### Parameters
* `id` Идентификатор обмена, используемый как первичный ключ 


* `parent_id` Идентификатор родительской заявки (если есть) 


* `type` Тип обмена (покупка, продажа и т.д.) 


* `status` Статус обмена (например, "опубликовано", "на модерации" и т.д.) 


* `username` Имя пользователя, создавшего заявку 


* `token_contract` Имя контракта токена 


* `unit_cost` Цена за единицу товара в заявке 


* `remain_units` Оставшееся количество товара 


* `blocked_units` Заблокированное количество товара 


* `delivered_units` Количество доставленного товара 


* `data` Дополнительные данные, связанные с заявкой 


* `meta` Метаданные заявки





Дополнительные индексы по status, type и parent_id позволяют искать заявки по этим полям.

Пример использования: 
```cpp
[exchange_index](#changes_8hpp_1ab95e0e1aa8d62946eff9deb3e8905f81) [exchange](#structexchange)(_me, [coopname](#structexchange_1a2105898359603634c6158ed2b59fefcc).value);
auto exchange_order = [exchange](#structexchange).find(id);
```

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public uint64_t parent_id` | 
`public uint64_t program_id` | 
`public eosio::name coopname` | 
`public eosio::name type` | 
`public eosio::name status` | 
`public eosio::name username` | 
`public eosio::name parent_username` | 
`public eosio::name token_contract` | 
`public eosio::asset unit_cost` | 
`public eosio::asset supplier_amount` | 
`public eosio::asset total_cost` | 
`public eosio::asset membership_fee` | 
`public uint64_t remain_units` | 
`public uint64_t blocked_units` | 
`public uint64_t delivered_units` | 
`public std::string data` | 
`public std::string meta` | 
`public eosio::name money_contributor` | 
`public eosio::name product_contributor` | 
`public `[`document`](#structdocument)` contribute_product_statement` | 
`public `[`document`](#structdocument)` return_product_statement` | 
`public uint64_t contribution_product_decision_id` | 
`public `[`document`](#structdocument)` contribution_product_authorization` | 
`public uint64_t return_product_decision_id` | 
`public `[`document`](#structdocument)` return_product_authorization` | 
`public `[`document`](#structdocument)` product_contribution_act_validation` | 
`public `[`document`](#structdocument)` product_contribution_act` | 
`public `[`document`](#structdocument)` product_recieve_act` | 
`public `[`document`](#structdocument)` product_recieve_act_validation` | 
`public uint64_t product_lifecycle_secs` | 
`public uint64_t cancellation_fee` | 
`public eosio::asset cancellation_fee_amount` | 
`public eosio::time_point_sec created_at` | 
`public eosio::time_point_sec accepted_at` | 
`public eosio::time_point_sec supplied_at` | 
`public eosio::time_point_sec delivered_at` | 
`public eosio::time_point_sec recieved_at` | 
`public eosio::time_point_sec completed_at` | 
`public eosio::time_point_sec declined_at` | 
`public eosio::time_point_sec disputed_at` | 
`public eosio::time_point_sec canceled_at` | 
`public eosio::time_point_sec warranty_delay_until` | 
`public eosio::time_point_sec deadline_for_receipt` | 
`public bool is_warranty_return` | 
`public uint64_t warranty_return_id` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t by_coop() const` | 
`public inline uint64_t by_status() const` | 
`public inline uint64_t by_program() const` | 
`public inline uint64_t by_type() const` | 
`public inline uint64_t by_parent() const` | 
`public inline uint64_t by_username() const` | 
`public inline uint64_t by_parent_username() const` | 
`public inline uint64_t by_created() const` | 
`public inline uint64_t by_completed() const` | 
`public inline uint64_t by_declined() const` | 
`public inline uint64_t by_canceled() const` | 
`public inline uint64_t by_warranty_id() const` | 

## Members

### `public uint64_t id` {#structexchange_1a8e84e7623af395f8d56d3689b997bc92}



идентификатор обмена

### `public uint64_t parent_id` {#structexchange_1a6ddc83225c4f9279f07e103d353b93e5}



идентификатор родительской заявки

### `public uint64_t program_id` {#structexchange_1adf0b7cf71405ab423568baf3a73545ff}



идентификатор программы

### `public eosio::name coopname` {#structexchange_1a2105898359603634c6158ed2b59fefcc}



имя аккаунта кооператива

### `public eosio::name type` {#structexchange_1acaa2e38908cc4759522a519ab4e5b020}



тип обмена

### `public eosio::name status` {#structexchange_1aa0662d5405ca80e5ff7ecde0f44f3245}



статус обмена

### `public eosio::name username` {#structexchange_1aeeddb9da07fc4c3abb417e4c5ce4f5d9}



имя аккаунта владельца заявки

### `public eosio::name parent_username` {#structexchange_1ae53e835f2474b42b519bca00d2915aa9}



имя аккаунта владельца объявления

### `public eosio::name token_contract` {#structexchange_1a852265f6a9a1bbe902d8b986d21083d9}



имя контракта токена

### `public eosio::asset unit_cost` {#structexchange_1a8455db0663bd0705b8416f57009ddb3c}



себестоимость единицы товара от поставщика

### `public eosio::asset supplier_amount` {#structexchange_1a7bda50d474b10ea252847cc2ba1cb12d}



сумма взноса поставщика

### `public eosio::asset total_cost` {#structexchange_1ae6fbc4b0f22917acd97b76b82eae9748}



сумма взноса заказчика

### `public eosio::asset membership_fee` {#structexchange_1ae84c4883716b5015d00d9ace75a7db1f}



членский взнос заказчика

### `public uint64_t remain_units` {#structexchange_1a340d949978cb99c510e10ba231c106e4}



оставшееся количество товара

### `public uint64_t blocked_units` {#structexchange_1a6fd079725783b578fd72e9dba31eb6e7}



заблокированное количество товара

### `public uint64_t delivered_units` {#structexchange_1a2a4c94f947fa284349132738412d59c4}



количество доставленного товара

### `public std::string data` {#structexchange_1add5daca8e65c7bdfc41163844bb1d07a}



дополнительные данные

### `public std::string meta` {#structexchange_1a5febab5dc90693a173066e1c190bbb80}



метаданные заявки

### `public eosio::name money_contributor` {#structexchange_1aefdecec89f6aa3b0172ba5a456450df9}





### `public eosio::name product_contributor` {#structexchange_1a754709fe54317173f6afc6ced693b704}





### `public `[`document`](#structdocument)` contribute_product_statement` {#structexchange_1a4e9c5e7ef7c1840fbae6954d4fe36168}





### `public `[`document`](#structdocument)` return_product_statement` {#structexchange_1a77a88c8b783e3eb34621c75a546da8be}





### `public uint64_t contribution_product_decision_id` {#structexchange_1aa7a994f37d20b0396127ba8fbdb13adb}





### `public `[`document`](#structdocument)` contribution_product_authorization` {#structexchange_1aabee255389ad9344cf54fa5b76890ef0}





### `public uint64_t return_product_decision_id` {#structexchange_1a53315bc58ebb64aa2660b7f98fdcf78f}





### `public `[`document`](#structdocument)` return_product_authorization` {#structexchange_1aee7af8606018fe8a4d24c8f8dabbb9f3}





### `public `[`document`](#structdocument)` product_contribution_act_validation` {#structexchange_1a6e30576f0345d5759d9c757f48c3cc3f}





### `public `[`document`](#structdocument)` product_contribution_act` {#structexchange_1aead650b619ee084555123d81e8789442}





### `public `[`document`](#structdocument)` product_recieve_act` {#structexchange_1a5aeccdeda0b8cfce8650b84885ccd243}





### `public `[`document`](#structdocument)` product_recieve_act_validation` {#structexchange_1a19fa40b7182a2170c5d1c355b348b65d}





### `public uint64_t product_lifecycle_secs` {#structexchange_1a7dd7733ed92097ffdeddcb8c5dd5a1b2}





### `public uint64_t cancellation_fee` {#structexchange_1a0eec3e49e6bd6b444bca071670b2eee8}





### `public eosio::asset cancellation_fee_amount` {#structexchange_1ac6209a52b651ece91891ee5fd2ee87c7}





### `public eosio::time_point_sec created_at` {#structexchange_1a707b849f1e28677607cc2ea58edc9ec8}





### `public eosio::time_point_sec accepted_at` {#structexchange_1aaf1fa63f449d21ea305b3e4e9d6fa8e2}





### `public eosio::time_point_sec supplied_at` {#structexchange_1a27d43111bf8d38e6095b6e715d5ce488}





### `public eosio::time_point_sec delivered_at` {#structexchange_1a3d52ae7a985e62ba32f41dc86061f694}





### `public eosio::time_point_sec recieved_at` {#structexchange_1ae57bee7712847c46c6bc0e00ef3adbca}





### `public eosio::time_point_sec completed_at` {#structexchange_1a108031ef9cbb58e4cd8c1548e475dd1d}





### `public eosio::time_point_sec declined_at` {#structexchange_1adb2f0c1569be46684e34adb0c1023dfa}





### `public eosio::time_point_sec disputed_at` {#structexchange_1a7f62b77bfd9bb80d753d9539e000a6ef}





### `public eosio::time_point_sec canceled_at` {#structexchange_1a3a85dd352d0b622f12af68956abe6b69}





### `public eosio::time_point_sec warranty_delay_until` {#structexchange_1ab9d50b69c2d30ad10b1f435ef8fd9903}





### `public eosio::time_point_sec deadline_for_receipt` {#structexchange_1ad1092fa2bb4bdfec8ca5996a1715ee74}





### `public bool is_warranty_return` {#structexchange_1a2acd659baddb8ac458105f5c74ce4bf4}





### `public uint64_t warranty_return_id` {#structexchange_1a47be77608e8a70caa188f14d9b6e8674}





### `public inline uint64_t primary_key() const` {#structexchange_1a3cd67e59b84642a2b8b834ec897967fa}



return id - primary_key

### `public inline uint64_t by_coop() const` {#structexchange_1aad91922434c8226846d393a6960a343b}



кооператив

### `public inline uint64_t by_status() const` {#structexchange_1af6641a816f36ce6af1c7c3a9e7ee78c2}



индекс по статусу

### `public inline uint64_t by_program() const` {#structexchange_1abb80c26178c6f659eb885289b6b5bb10}



индекс по программе

### `public inline uint64_t by_type() const` {#structexchange_1ab6ecb17793b182ce81385431fc597573}



индекс по типу

### `public inline uint64_t by_parent() const` {#structexchange_1addeb8bcc639717b6e95f9e792fd2aa0e}



индекс по родительскому ID

### `public inline uint64_t by_username() const` {#structexchange_1a0e57a0df23d6a5b470b6e88bf7fef359}



индекс по имени аккаунта

### `public inline uint64_t by_parent_username() const` {#structexchange_1ab08842a6bec345f366bbf465101bea2d}



индекс по имени аккаунта владельца объявления

### `public inline uint64_t by_created() const` {#structexchange_1a58c05688070a01939ad9ed1761805c2d}





### `public inline uint64_t by_completed() const` {#structexchange_1ab7bcbfa8b06b2110e885cd0721daac65}





### `public inline uint64_t by_declined() const` {#structexchange_1a7dad9c03b323d132af2116f7ca6c4c97}





### `public inline uint64_t by_canceled() const` {#structexchange_1a887de17304d064e2e1258807cb9d36c4}





### `public inline uint64_t by_warranty_id() const` {#structexchange_1a5d39784997a70edaafe0ac35e90e4f16}






# struct `exchange_params` {#structexchange__params}


Параметры заявки на обмен.

Эта структура предоставляет набор данных, которые требуются для создания или обновления заявки на обмен в контракте "marketplace".

Следующие параметры определяют заявку:

* username: Имя пользователя, инициирующего или обновляющего заявку.


* parent_id: Идентификатор родительской заявки (если есть).


* coopname: Имя кооператива.


* pieces: Количество частей (штук) товара или услуги для обмена.


* unit_cost: Цена за единицу (штуку) товара или услуги, выраженная в определенном формате asset.


* data: Содержит дополнительные данные, специфичные для заявки (например, условия обмена).


* meta: Метаданные, предоставляющие дополнительную информацию о заявке (например, описание товара или условия обмена).

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name username` | 
`public uint64_t parent_id` | 
`public uint64_t program_id` | 
`public eosio::name coopname` | 
`public uint64_t pieces` | 
`public eosio::asset unit_cost` | 
`public uint64_t product_lifecycle_secs` | 
`public `[`document`](#structdocument)` document` | 
`public std::string data` | 
`public std::string meta` | 

## Members

### `public eosio::name username` {#structexchange__params_1aa1fec016e858c8612fdb5655a7933fa4}



Имя пользователя, инициирующего или обновляющего заявку

### `public uint64_t parent_id` {#structexchange__params_1af3e8005efedfb42d53c5d141f9d35885}



Идентификатор родительской заявки

### `public uint64_t program_id` {#structexchange__params_1a13f740b5a33cf405f33aa7695c372efa}



Идентификатор программы

### `public eosio::name coopname` {#structexchange__params_1a8db71147b1b1a925d77a234a0e37fdc5}



Имя кооператива

### `public uint64_t pieces` {#structexchange__params_1a4984c8df3dba7213b1cd36e9c3886f8b}



Количество частей (штук) товара или услуги

### `public eosio::asset unit_cost` {#structexchange__params_1a50658ca46b0f9202d850ac5ad70fa1ec}



Цена за единицу (штуку) товара или услуги

### `public uint64_t product_lifecycle_secs` {#structexchange__params_1a77168c26ed335290f8dd81c10c93bf6f}



Время жизни продукта, заявляемое поставщиком

### `public `[`document`](#structdocument)` document` {#structexchange__params_1acd228f83a4f786bc1f98a51d0c9c49fb}



Сопутствующий подписанный документ на взнос или возврат взноса

### `public std::string data` {#structexchange__params_1ae14b0160b59af35e8e4786811398b314}



Дополнительные данные, специфичные для заявки

### `public std::string meta` {#structexchange__params_1aa769a0dcb11b34cad0d0cef89d2e2952}



Метаданные о заявке


# struct `expfunds` {#structexpfunds}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | идентификатор
`public eosio::name coopname` | идентификатор кооператива
`public eosio::name contract` | внешний контракт, которому передано управление фондом
`public std::string name` | название фонда
`public std::string description` | описание фонда
`public eosio::asset expended` | количество средств, списанных по фонду
`public inline uint64_t primary_key() const` | Первичный ключ для индексации по идентификатору фонда

## Members

### `public uint64_t id` {#structexpfunds_1aa6287ec5a42e47adab5a0a8a53785d8e}

идентификатор



### `public eosio::name coopname` {#structexpfunds_1ae7a605d6dc753f5d8c0341cd7d6a1d28}

идентификатор кооператива



### `public eosio::name contract` {#structexpfunds_1a4e0b2458abd7c9f57c1366c166bd3954}

внешний контракт, которому передано управление фондом



### `public std::string name` {#structexpfunds_1a245dc3f2b349a59561af7998d10d04cd}

название фонда



### `public std::string description` {#structexpfunds_1a781bc2179065a8410a066e72ec368dc5}

описание фонда



### `public eosio::asset expended` {#structexpfunds_1a088bb14e8732b2d547d2c568cb327d81}

количество средств, списанных по фонду



### `public inline uint64_t primary_key() const` {#structexpfunds_1a0e8b5ccb59641007b85bbcf7ef1d834d}

Первичный ключ для индексации по идентификатору фонда




# struct `fundwallet` {#structfundwallet}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | только нулевой идентификатор
`public eosio::name coopname` | идентификатор кооператива
`public eosio::asset circulating` | оборотные средства (паевый фонд)
`public eosio::asset membership` | все членские взносы
`public eosio::asset accumulated` | собрано фонды накопления
`public eosio::asset withdrawed` | выведено из фондов накопления
`public eosio::asset available` | доступно для списания в фондах
`public eosio::asset expended` | всего списано через фонды
`public inline uint64_t primary_key() const` | первичный ключ

## Members

### `public uint64_t id` {#structfundwallet_1a6e7afadc2f5184566173602aa2cdd5ef}

только нулевой идентификатор



### `public eosio::name coopname` {#structfundwallet_1a84bcb63f815cee187d2d83224d80fc7d}

идентификатор кооператива



### `public eosio::asset circulating` {#structfundwallet_1aa737876c0e5ec9449b1bafe8d4560f91}

оборотные средства (паевый фонд)



### `public eosio::asset membership` {#structfundwallet_1a3188bcd02db0c86fdabc3e1fb38d7fe1}

все членские взносы



### `public eosio::asset accumulated` {#structfundwallet_1a4bd269caa75108d5e1ca17219629741f}

собрано фонды накопления



### `public eosio::asset withdrawed` {#structfundwallet_1a99ca9b77cca8ac9dad189a9a046f9d78}

выведено из фондов накопления



### `public eosio::asset available` {#structfundwallet_1aa706c9090b7457a98ad757e007b47418}

доступно для списания в фондах



### `public eosio::asset expended` {#structfundwallet_1a912eec4e769a02fef0bf0baac729593c}

всего списано через фонды



### `public inline uint64_t primary_key() const` {#structfundwallet_1a4c444c553117baed8e54df059c9bcd23}

первичный ключ




# struct `fwithdraws` {#structfwithdraws}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name coopname` | 
`public eosio::name username` | 
`public eosio::name status` | 
`public eosio::name type` | 
`public uint64_t fund_id` | 
`public eosio::asset quantity` | 
`public `[`document`](#structdocument)` document` | 
`public std::string bank_data_id` | 
`public eosio::time_point_sec expired_at` | 
`public inline uint64_t primary_key() const` | Первичный ключ для индексации по идентификатору фонда
`public inline uint64_t by_username() const` | 
`public inline uint64_t by_status() const` | 
`public inline uint64_t by_expired() const` | 

## Members

### `public uint64_t id` {#structfwithdraws_1a7b59d31cd8750255c069d15dec51b5f4}





### `public eosio::name coopname` {#structfwithdraws_1a7cc27c0c01c6d7fa4484aa2d57bc46ee}





### `public eosio::name username` {#structfwithdraws_1aebff33b80e315c5a2e01cd00895df005}





### `public eosio::name status` {#structfwithdraws_1a8bdc79b90cdf30a96797fb5f87f193af}





### `public eosio::name type` {#structfwithdraws_1a41375de2393d4775fa06a10565699732}





### `public uint64_t fund_id` {#structfwithdraws_1a1bc432bcd93c911ca0c21a8619db9ea0}





### `public eosio::asset quantity` {#structfwithdraws_1a855dd04f3082e7ccde322d8cbe73b64a}





### `public `[`document`](#structdocument)` document` {#structfwithdraws_1adb442e17444ad0ed958b490adf7db24a}





### `public std::string bank_data_id` {#structfwithdraws_1a450ab8e004e6fca3d6a8dad17629b701}





### `public eosio::time_point_sec expired_at` {#structfwithdraws_1a77fc081541e3b88e4c34b5e9a309d25f}





### `public inline uint64_t primary_key() const` {#structfwithdraws_1ad2e9ed511ba3d596ef34e7431758f1ab}

Первичный ключ для индексации по идентификатору фонда



### `public inline uint64_t by_username() const` {#structfwithdraws_1abd60956d0d3e9ed918869a51aea1607a}





### `public inline uint64_t by_status() const` {#structfwithdraws_1a60fa7497fb02d85a90f473b5bca4ea02}





### `public inline uint64_t by_expired() const` {#structfwithdraws_1a02eb4b02215e0e13184515ea062df127}



Индекс по статусу истечения


# struct `joincoops` {#structjoincoops}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name username` | 
`public bool is_paid` | 
`public `[`document`](#structdocument)` statement` | 
`public std::string notice` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t byusername() const` | 

## Members

### `public uint64_t id` {#structjoincoops_1ac4a3e9cdd6ef21101afeae01c2ed2be1}





### `public eosio::name username` {#structjoincoops_1ad104e0cf15859a44c59c4a4ddb78f7ad}





### `public bool is_paid` {#structjoincoops_1ad10d6d51a81f467d3827bded4b0cebcd}





### `public `[`document`](#structdocument)` statement` {#structjoincoops_1ac9fb7c4a331725c153affefeb8955416}





### `public std::string notice` {#structjoincoops_1a2aa5cf144690ce5d537aceb9e68e15cf}





### `public inline uint64_t primary_key() const` {#structjoincoops_1a6de49204f0f1ca75f68f2d1bf473ddc9}





### `public inline uint64_t byusername() const` {#structjoincoops_1ab1dcb94186977ce2bd13899656f41a7f}






# struct `key_weight` {#structkey__weight}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::public_key key` | 
`public uint16_t weight` | 

## Members

### `public eosio::public_key key` {#structkey__weight_1a1c32ddbfc4c0d307c1f25503dfccd796}





### `public uint16_t weight` {#structkey__weight_1ac78b3460df5fe852385da3629d2a64e6}






# struct `org_data` {#structorg__data}


Структура данных нового юридического лица

Данная структура содержит всю необходимую информацию для регистрации нового юридического лица в блокчейне.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public `[`storage`](#structstorage)` storage` | Хранилища персональных данных и идентификаторы данных в них.
`public bool is_cooperative` | Является ли кооперативом
`public eosio::name coop_type` | Тип кооператива (union, conscoop, prodcoop, agricoop, builderscoop, nonprofitorg)
`public eosio::name token_contract` | Контракт токена
`public std::string announce` | Анонс
`public std::string description` | Описание
`public eosio::asset initial` | Вступительный взнос
`public eosio::asset minimum` | Минимальный взнос

## Members

### `public `[`storage`](#structstorage)` storage` {#structorg__data_1ae5aa80cb42de74403396c50b33bd20be}

Хранилища персональных данных и идентификаторы данных в них.



### `public bool is_cooperative` {#structorg__data_1a73a9e2d8573545f8852ff0dda1c860e5}

Является ли кооперативом



### `public eosio::name coop_type` {#structorg__data_1a879607268d761ab2fb337ac067234678}

Тип кооператива (union, conscoop, prodcoop, agricoop, builderscoop, nonprofitorg)



### `public eosio::name token_contract` {#structorg__data_1ad8aea1135b4ca307880d7a4477e841f4}

Контракт токена



### `public std::string announce` {#structorg__data_1af31eb176481e60108b902def456ff225}

Анонс



### `public std::string description` {#structorg__data_1a53f757c92b8548c72cb7edc7bb97663a}

Описание



### `public eosio::asset initial` {#structorg__data_1a46f196bb722c9e9218cb972193a676ff}

Вступительный взнос



### `public eosio::asset minimum` {#structorg__data_1a0e181c3124d29937b16b0bef4b2bdafa}

Минимальный взнос




# struct `orgs` {#structorgs}


Структура, представляющая организации.

Эта структура содержит информацию о юридических лицах (организациях), их верификации и других параметрах.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name username` | Имя аккаунта организации.
`public eosio::name parent_username` | Имя родительской организации, если есть.
`public std::vector< `[`verification`](#structverification)` > verifications` | Информация о верификации организации.
`public std::vector< `[`storage`](#structstorage)` > storages` | Хранилища персональных данных и идентификаторы данных в них.
`public bool is_cooperative` | Флаг, указывающий, является ли организация кооперативом.
`public bool is_active` | Флаг, указывающий, что организация активна.
`public eosio::name coop_type` | Тип некоммерческой организации (если это кооператив).
`public eosio::name token_contract` | Контракт токена, связанного с организацией.
`public std::string announce` | Анонс организации.
`public std::string description` | Описание организации.
`public eosio::asset registration` | Регистрационный взнос
`public eosio::asset initial` | Вступительный членский взнос
`public eosio::asset minimum` | Минимальный паевый взнос
`public inline uint64_t primary_key() const` | Возвращает первичный ключ учетной записи организации.
`public inline void check_contract_or_fail(eosio::name contract)` | Сравнивает контракт токена кооператива и представленный
`public inline void check_symbol_or_fail(eosio::asset contribution)` | Сравнивает символ токена кооператива и представленный
`public inline uint64_t by_parent() const` | Возвращает ключ по родительской организации.
`public inline uint128_t by_coop_childs() const` | Возвращает ключ для индекса кооперативных подразделений организации.
`public inline uint64_t is_coop_index() const` | Возвращает индекс для определения, является ли организация кооперативом.
`public inline uint64_t bycooptype() const` | Возвращает ключ для индекса по типу некоммерческой организации (если это кооператив).
`public inline uint64_t is_verified_index() const` | Возвращает индекс для определения, является ли организация верифицированной.
`public inline bool is_coop() const` | Проверяет, является ли организация кооперативом.
`public inline bool is_verified() const` | Проверяет, верифицирована ли организация.

## Members

### `public eosio::name username` {#structorgs_1a30bd763b06c88e2db7fe63b7829448c8}

Имя аккаунта организации.



### `public eosio::name parent_username` {#structorgs_1ae6431f4f421b347e828673c17c11016a}

Имя родительской организации, если есть.



### `public std::vector< `[`verification`](#structverification)` > verifications` {#structorgs_1a2f2e75cd711056a893bb6f84283cf069}

Информация о верификации организации.



### `public std::vector< `[`storage`](#structstorage)` > storages` {#structorgs_1a018bd81cc0de74c7c0edc6cb34296b29}

Хранилища персональных данных и идентификаторы данных в них.



### `public bool is_cooperative` {#structorgs_1a7e106134eb289da6034239f59f8c8b00}

Флаг, указывающий, является ли организация кооперативом.



### `public bool is_active` {#structorgs_1ab7a88197a80884f578161b340a42d445}

Флаг, указывающий, что организация активна.



### `public eosio::name coop_type` {#structorgs_1ac0f67a51c09138a902b4626694496810}

Тип некоммерческой организации (если это кооператив).



### `public eosio::name token_contract` {#structorgs_1a0acaa1da073fa33862536db6e1335bbd}

Контракт токена, связанного с организацией.



### `public std::string announce` {#structorgs_1aaa319494d52bab5194e2ccd093d8dff5}

Анонс организации.



### `public std::string description` {#structorgs_1a7e5900ad7ad2c99fe7a13bd70a154cca}

Описание организации.



### `public eosio::asset registration` {#structorgs_1af6d7d8a744c2ae53a5cec35369bf77ea}

Регистрационный взнос



### `public eosio::asset initial` {#structorgs_1ac195fc7b023e954eafd20e854570a302}

Вступительный членский взнос



### `public eosio::asset minimum` {#structorgs_1a1dbf728ec4dff7ca8f172bee4da901dd}

Минимальный паевый взнос



### `public inline uint64_t primary_key() const` {#structorgs_1ad63f7f770daeaf11e9960f0bb3413257}

Возвращает первичный ключ учетной записи организации.

#### Returns
uint64_t - первичный ключ, равный значению имени аккаунта организации.

### `public inline void check_contract_or_fail(eosio::name contract)` {#structorgs_1a0f61fb5a23a20f8418b6a840027ee59b}

Сравнивает контракт токена кооператива и представленный



### `public inline void check_symbol_or_fail(eosio::asset contribution)` {#structorgs_1ac30f7ac01496aacd2515dc474123c6f2}

Сравнивает символ токена кооператива и представленный



### `public inline uint64_t by_parent() const` {#structorgs_1ab88eb6ded8ef4609e0d0a013d4fcd19f}

Возвращает ключ по родительской организации.

#### Returns
uint64_t - ключ, равный значению имени родительской организации.

### `public inline uint128_t by_coop_childs() const` {#structorgs_1aaf7a07b7ede42be8755e8dab89b68273}

Возвращает ключ для индекса кооперативных подразделений организации.

#### Returns
uint128_t - составной ключ, включающий значения имени организации и родительской организации.

### `public inline uint64_t is_coop_index() const` {#structorgs_1a62e31444747797512f25c736d36e8ad3}

Возвращает индекс для определения, является ли организация кооперативом.

#### Returns
uint64_t - ключ, равный 1, если организация является кооперативом, иначе 0.

### `public inline uint64_t bycooptype() const` {#structorgs_1a9c08ac2829263b63bcf3cb773855691a}

Возвращает ключ для индекса по типу некоммерческой организации (если это кооператив).

#### Returns
uint64_t - ключ, равный значению типа некоммерческой организации.

### `public inline uint64_t is_verified_index() const` {#structorgs_1ab93e3b210063838c61fdc01849e93124}

Возвращает индекс для определения, является ли организация верифицированной.

#### Returns
uint64_t - ключ, равный 1, если организация верифицирована, иначе 0.

### `public inline bool is_coop() const` {#structorgs_1a56f3d02c014cc9448b0dc2e771c99f10}

Проверяет, является ли организация кооперативом.

#### Returns
bool - true, если организация является кооперативом, иначе false.

### `public inline bool is_verified() const` {#structorgs_1a153188e5dd5b21d491fd733feb6e8ff5}

Проверяет, верифицирована ли организация.

#### Returns
bool - true, если организация верифицирована, иначе false.


# struct `participants` {#structparticipants}


Структура, представляющая членов кооператива.

Эта структура содержит информацию о членах кооператива, включая их уникальные имена, дату создания, дату последнего обновления, дату последнего минимального платежа, должность, позицию, флаги их статуса и участия.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name username` | Уникальное имя члена кооператива.
`public eosio::time_point_sec created_at` | Время создания записи о члене.
`public eosio::time_point_sec last_update` | Время последнего обновления информации о члене.
`public eosio::time_point_sec last_min_pay` | Время последнего минимального платежа.
`public eosio::name status` | 
`public bool is_initial` | Флаг, указывающий, внесен ли регистрационный взнос.
`public bool is_minimum` | Флаг, указывающий, внесен ли минимальный паевый взнос.
`public bool has_vote` | Флаг, указывающий, имеет ли член право голоса.
`public inline uint64_t primary_key() const` | Возвращает первичный ключ учетной записи члена кооператива.
`public inline uint64_t bylastpay() const` | Возвращает ключ для индексации по времени последнего минимального платежа.
`public inline uint64_t by_created_at() const` | Возвращает ключ для индексации по времени создания
`public inline bool is_active() const` | 

## Members

### `public eosio::name username` {#structparticipants_1ae5560052371e0cc4d94f156f8d732fe1}

Уникальное имя члена кооператива.



### `public eosio::time_point_sec created_at` {#structparticipants_1a23d435b17537bb122b11ede9692b605a}

Время создания записи о члене.



### `public eosio::time_point_sec last_update` {#structparticipants_1a4f914f47ab6ea9ff77135d13f71f361d}

Время последнего обновления информации о члене.



### `public eosio::time_point_sec last_min_pay` {#structparticipants_1acb3931363302e336cfb0c62e3a2d9abc}

Время последнего минимального платежа.



### `public eosio::name status` {#structparticipants_1a3c39e76b933e59ec19db7205337b5a9a}





### `public bool is_initial` {#structparticipants_1a9172b4ea5fa9b858be1a2ca24d467c31}

Флаг, указывающий, внесен ли регистрационный взнос.



### `public bool is_minimum` {#structparticipants_1adb615a78b2bf2c4ea428289a6af5f64c}

Флаг, указывающий, внесен ли минимальный паевый взнос.



### `public bool has_vote` {#structparticipants_1a3b77b4b3083b75e0adf822241ae5fbfe}

Флаг, указывающий, имеет ли член право голоса.



### `public inline uint64_t primary_key() const` {#structparticipants_1a8623d16a26dea29aaf14d87f10ecf3c9}

Возвращает первичный ключ учетной записи члена кооператива.

#### Returns
uint64_t - первичный ключ, равный значению имени члена кооператива. return username - primary_key

### `public inline uint64_t bylastpay() const` {#structparticipants_1aa40e98abaf7212e03dd80e1f993d6c17}

Возвращает ключ для индексации по времени последнего минимального платежа.

#### Returns
uint64_t - ключ, равный количеству секунд с начала эпохи Unix.

### `public inline uint64_t by_created_at() const` {#structparticipants_1ab63c149354ae6df6a4a5de3948fdaf00}

Возвращает ключ для индексации по времени создания

#### Returns
uint64_t - ключ, равный количеству секунд с начала эпохи Unix.

### `public inline bool is_active() const` {#structparticipants_1a755cb10cc54ed15fcf4849ace4c679ad}






# struct `permission_level_weight` {#structpermission__level__weight}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::permission_level permission` | 
`public uint16_t weight` | 

## Members

### `public eosio::permission_level permission` {#structpermission__level__weight_1a8e66350534de3fc26309013f5ade74b8}





### `public uint16_t weight` {#structpermission__level__weight_1a8a90481461f59afdd92875d0caa6959f}






# struct `plot_data` {#structplot__data}


Структура данных нового юридического лица

Данная структура содержит всю необходимую информацию для регистрации нового юридического лица в блокчейне.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public `[`storage`](#structstorage)` storage` | Хранилища персональных данных и идентификаторы данных в них.
`public std::string announce` | Анонс
`public std::string description` | Описание

## Members

### `public `[`storage`](#structstorage)` storage` {#structplot__data_1ac51b73b9ad19fb9a869e952c8ebf9828}

Хранилища персональных данных и идентификаторы данных в них.



### `public std::string announce` {#structplot__data_1a5fb18fab386662439ed945d5a2ef73bb}

Анонс



### `public std::string description` {#structplot__data_1abd7a5526c0912ce5e7be26ec188a2b0d}

Описание




# struct `progcomarket` {#structprogcomarket}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name coopname` | 
`public bool is_active` | 
`public std::string title` | 
`public std::string announce` | 
`public std::string description` | 
`public std::string preview` | 
`public std::string images` | 
`public std::string meta` | 
`public eosio::name calculation_type` | 
`public uint64_t membership_percent_fee` | 
`public eosio::asset fixed_membership_contribution` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public uint64_t id` {#structprogcomarket_1a70a4d09612198f1e70dc392965a7adf8}



идентификатор обмена

### `public eosio::name coopname` {#structprogcomarket_1af4d7b00b42ee363950cb8ba56290306b}



имя аккаунта кооператива

### `public bool is_active` {#structprogcomarket_1ac6b384389548125020788aa7dfc80c76}





### `public std::string title` {#structprogcomarket_1a496676734d8019a739f71170d4ea11d1}





### `public std::string announce` {#structprogcomarket_1ae62aa2f18cc0ce05663bdc7d40bbead5}





### `public std::string description` {#structprogcomarket_1a9b8ce6d41fb8c42fca2904e19c9f8f23}





### `public std::string preview` {#structprogcomarket_1a74aac7b95f2f1ab8a4d2b525d2c0b787}





### `public std::string images` {#structprogcomarket_1ac6006d3e095260e4762a0d1ac98cd2c8}





### `public std::string meta` {#structprogcomarket_1aa8f5800006f9d88312ee65d6b3c1073b}





### `public eosio::name calculation_type` {#structprogcomarket_1a00dded91ff525dab50fdb41167ed26af}



тип настройки платежей по программе ( absolute | relative )

### `public uint64_t membership_percent_fee` {#structprogcomarket_1a4a48607ebb37e1b46f617ffe204364e5}



процент комиссии со взноса

### `public eosio::asset fixed_membership_contribution` {#structprogcomarket_1a542812cefc717f045d930d19f85b13cc}



Членский взнос

### `public inline uint64_t primary_key() const` {#structprogcomarket_1aef6ff4eb497b4cefc3e543eac79ad9ca}



return id - primary_key


# struct `progwallets` {#structprogwallets}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name coopname` | 
`public uint64_t program_id` | 
`public eosio::name username` | 
`public eosio::asset available` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t by_username() const` | 
`public inline uint64_t by_program() const` | 
`public inline uint128_t by_username_and_program() const` | 

## Members

### `public uint64_t id` {#structprogwallets_1a7d1baaf6016763a42b0ffee6e00ea690}





### `public eosio::name coopname` {#structprogwallets_1a670074d9a5dacbb811ecd82e1049f41a}





### `public uint64_t program_id` {#structprogwallets_1a0b8f5a1eae7b19cc2a35441ff1f39277}





### `public eosio::name username` {#structprogwallets_1a8c3361a53f0b8d8124538293ee4d428a}





### `public eosio::asset available` {#structprogwallets_1afdd0676819e6c70ec2a9a9fa82b3510a}





### `public inline uint64_t primary_key() const` {#structprogwallets_1a89de991f214be871ee4baa55bc941756}



return id - primary_key

### `public inline uint64_t by_username() const` {#structprogwallets_1a4cceac2dd54374e766cb43110f8c8924}



username - secondary_key

### `public inline uint64_t by_program() const` {#structprogwallets_1a5b332d51633dbda7bb8f17e04e49e11b}



username - secondary_key

### `public inline uint128_t by_username_and_program() const` {#structprogwallets_1ad86cd689f05547c9dea842ea605542da}



возвращает уникальный индекс, сформированный из значения username и program_id


# struct `right` {#structright}


Структура, представляющая права доступа.

Эта структура содержит информацию о правах доступа, связанных с конкретным контрактом и действием.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name contract` | Имя контракта, к которому применяется право доступа.
`public eosio::name action_name` | Имя действия, к которому применяется право доступа.

## Members

### `public eosio::name contract` {#structright_1a72bb88ed7a935b2652edd7c01ed9cbfb}

Имя контракта, к которому применяется право доступа.



### `public eosio::name action_name` {#structright_1a7b8447f641a5a31a5c77b23ab3d97dd3}

Имя действия, к которому применяется право доступа.




# struct `staff` {#structstaff}


Структура, представляющая администраторов кооператива.

Эта структура содержит информацию об администраторах, их уникальных именах, должностях и правах.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name username` | Уникальное имя администратора.
`public std::string position_title` | Название должности администратора.
`public std::vector< eosio::name > roles` | Список ролей.
`public std::vector< `[`right`](#structright)` > rights` | Список прав администратора.
`public eosio::time_point_sec created_at` | Время создания записи об администраторе.
`public eosio::time_point_sec updated_at` | Время последнего обновления информации об администраторе.
`public inline uint64_t primary_key() const` | Первичный ключ для индексации по имени администратора
`public inline bool has_right(eosio::name contract,eosio::name action_name) const` | 

## Members

### `public eosio::name username` {#structstaff_1aa67b206d474a4f87453bca87fda2f01b}

Уникальное имя администратора.



### `public std::string position_title` {#structstaff_1a8fa0fdaf003c7588f9f1185a087785ba}

Название должности администратора.



### `public std::vector< eosio::name > roles` {#structstaff_1adc9ddede2cb6c7035ee3425ecdee5df4}

Список ролей.



### `public std::vector< `[`right`](#structright)` > rights` {#structstaff_1a935eee764193e5796da146fedfc465e5}

Список прав администратора.



### `public eosio::time_point_sec created_at` {#structstaff_1ab46bbfc312f86ec081fead61e3d01a10}

Время создания записи об администраторе.



### `public eosio::time_point_sec updated_at` {#structstaff_1a2079dc310b3faf4112d9046e54a72ab7}

Время последнего обновления информации об администраторе.



### `public inline uint64_t primary_key() const` {#structstaff_1a907194daa8db9aee9629350891757d22}

Первичный ключ для индексации по имени администратора



### `public inline bool has_right(eosio::name contract,eosio::name action_name) const` {#structstaff_1a72d9d9dac115d8379e3afa76ff712c98}






# struct `storage` {#structstorage}


Структура, представляющая хранилища данных, в которых хранятся персональные данные и их идентификаторы.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name storage_username` | Имя аккаунта хранилища персональных данных

## Members

### `public eosio::name storage_username` {#structstorage_1aac03d6540bfa034d307d826b47e4cae1}

Имя аккаунта хранилища персональных данных




# struct `translations` {#structtranslations}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name creator` | 
`public uint64_t draft_id` | 
`public eosio::name lang` | 
`public std::string data` | 
`public bool is_published` | 
`public bool is_approved` | 
`public inline uint64_t primary_key() const` | 
`public inline uint64_t by_draft() const` | 
`public inline uint128_t by_draft_lang() const` | 

## Members

### `public uint64_t id` {#structtranslations_1a01cec3def75b81168dc6b45e9c4dbff1}





### `public eosio::name creator` {#structtranslations_1a9e827247b492346414cc70ce06a5e6c1}





### `public uint64_t draft_id` {#structtranslations_1a99f9ba150840e017e9c8735f60641051}





### `public eosio::name lang` {#structtranslations_1abdd9ea697a2e94844c11ec157c51c39b}





### `public std::string data` {#structtranslations_1a2ea009ac87abff8fe9859b57d6285faa}





### `public bool is_published` {#structtranslations_1a926ff0d82adf496d8bdd9eb980bb3ecd}





### `public bool is_approved` {#structtranslations_1a9cf608b0fca1dcca549a4a67ec1f1edb}





### `public inline uint64_t primary_key() const` {#structtranslations_1ae38e282198e859143fd310d83fcebbea}





### `public inline uint64_t by_draft() const` {#structtranslations_1a27121dfdce788ffd9d590ed583dc7899}





### `public inline uint128_t by_draft_lang() const` {#structtranslations_1a2bb68da3a95931454c65132d34acbbb6}






# struct `users` {#structusers}


Структура, представляющая учетные записи пользователей.

Эта структура хранит информацию о пользователях, их профилях и верификации.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name username` | Имя аккаунта пользователя.
`public bool is_active` | Флаг активности.
`public std::vector< `[`storage`](#structstorage)` > storages` | Хранилища персональных данных и идентификаторы данных в них.
`public std::vector< `[`verification`](#structverification)` > verifications` | Информация о верификации пользователя.
`public inline uint64_t primary_key() const` | Возвращает первичный ключ учетной записи пользователя.

## Members

### `public eosio::name username` {#structusers_1a737eae3ed6e76f98919b26b20288164b}

Имя аккаунта пользователя.



### `public bool is_active` {#structusers_1abe29added38df2a3d85bd2f8a6f1cc91}

Флаг активности.



### `public std::vector< `[`storage`](#structstorage)` > storages` {#structusers_1a2752786aa29eb539056f93523777e347}

Хранилища персональных данных и идентификаторы данных в них.



### `public std::vector< `[`verification`](#structverification)` > verifications` {#structusers_1a9b0680107bdfb2818db4b299d6b60de0}

Информация о верификации пользователя.



### `public inline uint64_t primary_key() const` {#structusers_1a6894c358dbcce05a63de742f2b5fbd8e}

Возвращает первичный ключ учетной записи пользователя.

#### Returns
uint64_t - первичный ключ, равный значению имени аккаунта пользователя. return username - primary_key


# struct `verification` {#structverification}


Структура, представляющая верификацию.

Тип процедуры верификации (procedure) может принимать следующие значения:

* 0: Online


* 1: Video Call


* 2: Qualified Signature


* 3: Gosuslugi


* 4: С помощью мобильного приложения (App)


* 5: Некоторые платежные системы проводят верификацию в Связном (Agent Store)


* 6: Верификация в банке (Bank)


* 7: Верификация в кооперативе (In Person)

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name verificator` | Организация, которая произвела верификацию
`public bool is_verified` | Флаг, указывающий, прошла ли верификация
`public eosio::name procedure` | Тип процедуры верификации
`public eosio::time_point_sec created_at` | Время создания записи
`public eosio::time_point_sec last_update` | Время последнего обновления записи
`public std::string notice` | Дополнительная информация

## Members

### `public eosio::name verificator` {#structverification_1a7fb3c0fd447a28ad0919859e3ccb5aa7}

Организация, которая произвела верификацию



### `public bool is_verified` {#structverification_1aeb6702bd4b7903963252735d6f760c50}

Флаг, указывающий, прошла ли верификация



### `public eosio::name procedure` {#structverification_1a0445ec6869e5259c336dd7010a357a6e}

Тип процедуры верификации



### `public eosio::time_point_sec created_at` {#structverification_1aa7d19ca745ac2729d841ff65548622e6}

Время создания записи



### `public eosio::time_point_sec last_update` {#structverification_1a674206f7442d3ba3319ce4b22d542b04}

Время последнего обновления записи



### `public std::string notice` {#structverification_1a231be135ab1c6c218ca0ed86383d043c}

Дополнительная информация




# struct `wait_weight` {#structwait__weight}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint32_t wait_sec` | 
`public uint16_t weight` | 

## Members

### `public uint32_t wait_sec` {#structwait__weight_1a3286af83356d29e62a35ccb650d92c98}





### `public uint16_t weight` {#structwait__weight_1a41ebcf1f6076155456429094e7db0d2f}






# struct `wallets` {#structwallets}






## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public eosio::name username` | 
`public eosio::name coopname` | 
`public eosio::asset available` | 
`public eosio::asset blocked` | 
`public eosio::asset minimum` | 
`public inline uint64_t primary_key() const` | 

## Members

### `public eosio::name username` {#structwallets_1a5bf63dc37dc288590a7ff1b003ce5bdd}





### `public eosio::name coopname` {#structwallets_1aca067f3f06b79fdf17bc20f2f7b5b5ca}





### `public eosio::asset available` {#structwallets_1a957cd6ea3e8b379c05b9ecae8aecf2ec}





### `public eosio::asset blocked` {#structwallets_1a46f39ef4eb3a087abb588d747f4c3cf7}





### `public eosio::asset minimum` {#structwallets_1a8b429e66e7455d556b00ab2f140ec103}





### `public inline uint64_t primary_key() const` {#structwallets_1ab97728a6804a1c5d59b7043796a06c20}



return username - primary_key


# struct `withdraws` {#structwithdraws}


Таблица `withdraws` отслеживает операции вывода средств в контракте GATEWAY.



## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public uint64_t id` | 
`public eosio::name username` | 
`public eosio::name coopname` | 
`public std::string bank_data_id` | 
`public eosio::name token_contract` | 
`public eosio::asset quantity` | 
`public eosio::name status` | 
`public `[`document`](#structdocument)` document` | 
`public std::string memo` | 
`public eosio::time_point_sec created_at` | Время истечения срока давности
`public inline uint64_t primary_key() const` | 
`public inline uint64_t by_coopname() const` | 
`public inline uint64_t by_username() const` | 
`public inline uint64_t by_status() const` | 
`public inline uint64_t by_created() const` | 

## Members

### `public uint64_t id` {#structwithdraws_1a0bd51f247965dbfb329fec3d40ea4aff}



Уникальный идентификатор записи вывода

### `public eosio::name username` {#structwithdraws_1a90f3595d227a8fa839c987ff704a6fb1}



Имя пользователя, осуществляющего вывод средств

### `public eosio::name coopname` {#structwithdraws_1a8a335a5d2d6d9df49548f2742babfa56}



Имя аккаунта кооператива, в рамках которого осуществляется вывод

### `public std::string bank_data_id` {#structwithdraws_1a8ffde0a7ea12626e021d4556726d2cf1}



Строковый идентификатор банковских реквизитов в приватной базе данных кооператива

### `public eosio::name token_contract` {#structwithdraws_1a0c214e95d7d9b8d7b9689f7ddd0e62f1}



Имя контракта контракта токена для вывода

### `public eosio::asset quantity` {#structwithdraws_1ad636cffd24311103bc97378f444d7ffd}



Количество средств для вывода во внутренней валюте

### `public eosio::name status` {#structwithdraws_1a66705c798a455391622a98254fa57d94}



Статус операции вывода

### `public `[`document`](#structdocument)` document` {#structwithdraws_1a1d5c5381ecaf78a41e8786f7aaad12ba}



Заявление на возврат

### `public std::string memo` {#structwithdraws_1a37174203ef7bac581734dc9b623a8d07}



Примечание к операции вывода

### `public eosio::time_point_sec created_at` {#structwithdraws_1ae7aab0d7401094cf68dac64ca3b2baf1}

Время истечения срока давности



### `public inline uint64_t primary_key() const` {#structwithdraws_1a03f0fac1b1b5de27fb09730a35eb2ac4}



Возвращает id как первичный ключ

### `public inline uint64_t by_coopname() const` {#structwithdraws_1a2cdd545bc8a4b1986d670e17abb9e4f3}



Индекс по имени кооператива

### `public inline uint64_t by_username() const` {#structwithdraws_1af28428694d19b3e435befc37311f874b}



Индекс по имени пользователя

### `public inline uint64_t by_status() const` {#structwithdraws_1a279e91da89642cc4bee51458069ee21d}



Индекс по статусу операции вывода

### `public inline uint64_t by_created() const` {#structwithdraws_1a9b1951a93b9c89267a38e17882f74023}



Индекс по статусу истечения


