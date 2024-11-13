#pragma once

#include <eosio/asset.hpp>
#include <eosio/binary_extension.hpp>
#include <eosio/privileged.hpp>
#include <eosio/producer_schedule.hpp>
#include <eosio/singleton.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>

#include <eosio.system/exchange_state.hpp>
#include <eosio.system/native.hpp>

#include <deque>
#include <optional>
#include <string>
#include <type_traits>
#include "../../../../../common/consts.hpp"


#define CHANNEL_RAM_AND_NAMEBID_FEES_TO_REX 0

namespace eosiosystem {

   using eosio::asset;
   using eosio::binary_extension;
   using eosio::block_timestamp;
   using eosio::check;
   using eosio::const_mem_fun;
   using eosio::datastream;
   using eosio::indexed_by;
   using eosio::name;
   using eosio::same_payer;
   using eosio::symbol;
   using eosio::symbol_code;
   using eosio::time_point;
   using eosio::time_point_sec;
   using eosio::unsigned_int;

   inline constexpr int64_t powerup_frac = 1'000'000'000'000'000ll;  // 1.0 = 10^15

   template<typename E, typename F>
   static inline auto has_field( F flags, E field )
   -> std::enable_if_t< std::is_integral_v<F> && std::is_unsigned_v<F> &&
                        std::is_enum_v<E> && std::is_same_v< F, std::underlying_type_t<E> >, bool>
   {
      return ( (flags & static_cast<F>(field)) != 0 );
   }

   template<typename E, typename F>
   static inline auto set_field( F flags, E field, bool value = true )
   -> std::enable_if_t< std::is_integral_v<F> && std::is_unsigned_v<F> &&
                        std::is_enum_v<E> && std::is_same_v< F, std::underlying_type_t<E> >, F >
   {
      if( value )
         return ( flags | static_cast<F>(field) );
      else
         return ( flags & ~static_cast<F>(field) );
   }

   static constexpr uint32_t seconds_per_year      = 52 * 7 * 24 * 3600;
   static constexpr uint32_t seconds_per_day       = 24 * 3600;
   static constexpr uint32_t seconds_per_hour      = 3600;
   static constexpr int64_t  useconds_per_year     = int64_t(seconds_per_year) * 1000'000ll;
   static constexpr int64_t  useconds_per_day      = int64_t(seconds_per_day) * 1000'000ll;
   static constexpr int64_t  useconds_per_hour     = int64_t(seconds_per_hour) * 1000'000ll;
   static constexpr uint32_t blocks_per_day        = 2 * seconds_per_day; // half seconds per day

   static constexpr int64_t  min_activated_stake   = 150'000'000'0000;
   static constexpr int64_t  ram_gift_bytes        = 0; //1400;
   static constexpr int64_t  min_pervote_daily_pay = 1'0000;
   static constexpr uint32_t refund_delay_sec      = 3 * seconds_per_day;

   
#ifdef SYSTEM_BLOCKCHAIN_PARAMETERS
   struct blockchain_parameters_v1 : eosio::blockchain_parameters
   {
      eosio::binary_extension<uint32_t> max_action_return_value_size;
      EOSLIB_SERIALIZE_DERIVED( blockchain_parameters_v1, eosio::blockchain_parameters,
                                (max_action_return_value_size) )
   };
   using blockchain_parameters_t = blockchain_parameters_v1;
#else
   using blockchain_parameters_t = eosio::blockchain_parameters;
#endif

  /**
   * The `eosio.system` smart contract is provided a sample system contract, and it defines the structures and actions needed for blockchain's core functionality.
   * 
   * Just like in the `eosio.bios` sample contract implementation, there are a few actions which are not implemented at the contract level (`newaccount`, `updateauth`, `deleteauth`, `linkauth`, `unlinkauth`, `canceldelay`, `onerror`, `setabi`, `setcode`), they are just declared in the contract so they will show in the contract's ABI and users will be able to push those actions to the chain via the account holding the `eosio.system` contract, but the implementation is at the EOSIO core level. They are referred to as EOSIO native actions.
   * 
   * - Users can stake tokens for CPU and Network bandwidth, and then vote for producers or
   *    delegate their vote to a proxy.
   * - Producers register in order to be voted for, and can claim per-block and per-vote rewards.
   * - Users can buy and sell RAM at a market-determined price.
   * - Users can bid on premium names.
   */
  
   // A name bid, which consists of:
   // - a `newname` name that the bid is for
   // - a `high_bidder` account name that is the one with the highest bid so far
   // - the `high_bid` which is amount of highest bid
   // - and `last_bid_time` which is the time of the highest bid
   struct [[eosio::table, eosio::contract("eosio.system")]] name_bid {
     name            newname;
     name            high_bidder;
     int64_t         high_bid = 0; ///< negative high_bid == closed auction waiting to be claimed
     time_point      last_bid_time;

     uint64_t primary_key()const { return newname.value;                    }
     uint64_t by_high_bid()const { return static_cast<uint64_t>(-high_bid); }
   };

   // A bid refund, which is defined by:
   // - the `bidder` account name owning the refund
   // - the `amount` to be refunded
   struct [[eosio::table, eosio::contract("eosio.system")]] bid_refund {
      name         bidder;
      asset        amount;

      uint64_t primary_key()const { return bidder.value; }
   };
   typedef eosio::multi_index< "namebids"_n, name_bid,
                               indexed_by<"highbid"_n, const_mem_fun<name_bid, uint64_t, &name_bid::by_high_bid>  >
                             > name_bid_table;

   typedef eosio::multi_index< "bidrefunds"_n, bid_refund > bid_refund_table;

   // Defines new global state parameters.
   struct [[eosio::table("global"), eosio::contract("eosio.system")]] eosio_global_state : eosio::blockchain_parameters {
      uint64_t free_ram()const { return max_ram_size - total_ram_bytes_reserved; }

      uint64_t             max_ram_size = 8ll*1024 * 1024 * 1024;
      uint64_t             total_ram_bytes_reserved = 0;
      int64_t              total_ram_stake = 0;

      block_timestamp      last_producer_schedule_update;
      time_point           last_pervote_bucket_fill;
      int64_t              pervote_bucket = 0;
      int64_t              perblock_bucket = 0;
      uint32_t             total_unpaid_blocks = 0; /// all blocks which have been produced but not paid
      int64_t              total_activated_stake = 0;
      time_point           thresh_activated_stake_time;
      uint16_t             last_producer_schedule_size = 0;
      double               total_producer_vote_weight = 0; /// the sum of all producer votes
      uint16_t             new_ram_per_block = 0;
      block_timestamp      last_ram_increase;
      uint8_t              revision = 0; ///< used to track version updates in the future.

      block_timestamp      last_name_close;

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE_DERIVED( eosio_global_state, eosio::blockchain_parameters,
                                (max_ram_size)(total_ram_bytes_reserved)(total_ram_stake)
                                (last_producer_schedule_update)(last_pervote_bucket_fill)
                                (pervote_bucket)(perblock_bucket)(total_unpaid_blocks)(total_activated_stake)(thresh_activated_stake_time)
                                (last_producer_schedule_size)(total_producer_vote_weight)
                                (new_ram_per_block)(last_ram_increase)(revision)
                                (last_name_close) )
   };


   inline eosio::block_signing_authority convert_to_block_signing_authority( const eosio::public_key& producer_key ) {
      return eosio::block_signing_authority_v0{ .threshold = 1, .keys = {{producer_key, 1}} };
   }

   // Defines `producer_info` structure to be stored in `producer_info` table, added after version 1.0
   struct [[eosio::table, eosio::contract("eosio.system")]] producer_info {
      name                                                     owner;
      double                                                   total_votes = 0;
      eosio::public_key                                        producer_key; /// a packed public key object
      bool                                                     is_active = true;
      std::string                                              url;
      uint32_t                                                 unpaid_blocks = 0;
      time_point                                               last_claim_time;
      uint16_t                                                 location = 0;
      eosio::binary_extension<eosio::block_signing_authority>  producer_authority; // added in version 1.9.0

      uint64_t primary_key()const { return owner.value;                             }
      double   by_votes()const    { return is_active ? -total_votes : total_votes;  }
      bool     active()const      { return is_active;                               }
      void     deactivate()       { producer_key = public_key(); producer_authority.reset(); is_active = false; }

      eosio::block_signing_authority get_producer_authority()const {
         if( producer_authority.has_value() ) {
            bool zero_threshold = std::visit( [](auto&& auth ) -> bool {
               return (auth.threshold == 0);
            }, *producer_authority );
            // zero_threshold could be true despite the validation done in regproducer2 because the v1.9.0 eosio.system
            // contract has a bug which may have modified the producer table such that the producer_authority field
            // contains a default constructed eosio::block_signing_authority (which has a 0 threshold and so is invalid).
            if( !zero_threshold ) return *producer_authority;
         }
         return convert_to_block_signing_authority( producer_key );
      }

      // The unregprod and claimrewards actions modify unrelated fields of the producers table and under the default
      // serialization behavior they would increase the size of the serialized table if the producer_authority field
      // was not already present. This is acceptable (though not necessarily desired) because those two actions require
      // the authority of the producer who pays for the table rows.
      // However, the rmvproducer action and the onblock transaction would also modify the producer table in a similar
      // way and increasing its serialized size is not acceptable in that context.
      // So, a custom serialization is defined to handle the binary_extension producer_authority
      // field in the desired way. (Note: v1.9.0 did not have this custom serialization behavior.)

      template<typename DataStream>
      friend DataStream& operator << ( DataStream& ds, const producer_info& t ) {
         ds << t.owner
            << t.total_votes
            << t.producer_key
            << t.is_active
            << t.url
            << t.unpaid_blocks
            << t.last_claim_time
            << t.location;

         if( !t.producer_authority.has_value() ) return ds;

         return ds << t.producer_authority;
      }

      template<typename DataStream>
      friend DataStream& operator >> ( DataStream& ds, producer_info& t ) {
         return ds >> t.owner
                   >> t.total_votes
                   >> t.producer_key
                   >> t.is_active
                   >> t.url
                   >> t.unpaid_blocks
                   >> t.last_claim_time
                   >> t.location
                   >> t.producer_authority;
      }
   };

   // Voter info. Voter info stores information about the voter:
   // - `owner` the voter
   // - `proxy` the proxy set by the voter, if any
   // - `producers` the producers approved by this voter if no proxy set
   // - `staked` the amount staked
   struct [[eosio::table, eosio::contract("eosio.system")]] voter_info {
      name                owner;     /// the voter
      name                proxy;     /// the proxy set by the voter, if any
      std::vector<name>   producers; /// the producers approved by this voter if no proxy set
      int64_t             staked = 0;

      //  Every time a vote is cast we must first "undo" the last vote weight, before casting the
      //  new vote weight.  Vote weight is calculated as:
      //  stated.amount * 2 ^ ( weeks_since_launch/weeks_per_year)
      double              last_vote_weight = 0; /// the vote weight cast the last time the vote was updated

      // Total vote weight delegated to this voter.
      double              proxied_vote_weight= 0; /// the total vote weight delegated to this voter as a proxy
      bool                is_proxy = 0; /// whether the voter is a proxy for others


      uint32_t            flags1 = 0;
      uint32_t            reserved2 = 0;
      eosio::asset        reserved3;

      uint64_t primary_key()const { return owner.value; }

      enum class flags1_fields : uint32_t {
         ram_managed = 1,
         net_managed = 2,
         cpu_managed = 4
      };

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( voter_info, (owner)(proxy)(producers)(staked)(last_vote_weight)(proxied_vote_weight)(is_proxy)(flags1)(reserved2)(reserved3) )
   };


   typedef eosio::multi_index< "voters"_n, voter_info >  voters_table;

   typedef eosio::multi_index< "producers"_n, producer_info,
                               indexed_by<"prototalvote"_n, const_mem_fun<producer_info, double, &producer_info::by_votes>  >
                             > producers_table;

   typedef eosio::singleton< "global"_n, eosio_global_state >   global_state_singleton;

   struct [[eosio::table, eosio::contract("eosio.system")]] user_resources {
      name          owner;
      asset         net_weight;
      asset         cpu_weight;
      int64_t       ram_bytes = 0;

      bool is_empty()const { return net_weight.amount == 0 && cpu_weight.amount == 0 && ram_bytes == 0; }
      uint64_t primary_key()const { return owner.value; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( user_resources, (owner)(net_weight)(cpu_weight)(ram_bytes) )
   };


   // Every user 'from' has a scope/table that uses every recipient 'to' as the primary key.
   struct [[eosio::table, eosio::contract("eosio.system")]] delegated_bandwidth {
      name          from;
      name          to;
      asset         net_weight;
      asset         cpu_weight;

      bool is_empty()const { return net_weight.amount == 0 && cpu_weight.amount == 0; }
      uint64_t  primary_key()const { return to.value; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( delegated_bandwidth, (from)(to)(net_weight)(cpu_weight) )

   };

   struct [[eosio::table, eosio::contract("eosio.system")]] refund_request {
      name            owner;
      time_point_sec  request_time;
      eosio::asset    net_amount;
      eosio::asset    cpu_amount;

      bool is_empty()const { return net_amount.amount == 0 && cpu_amount.amount == 0; }
      uint64_t  primary_key()const { return owner.value; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( refund_request, (owner)(request_time)(net_amount)(cpu_amount) )
   };

    // Структура записи о долге по RAM для аккаунта.
    struct [[eosio::table, eosio::contract("eosio.system")]] ram_debt_record {
        name account;
        int64_t ram_debt;

        uint64_t primary_key() const { return account.value; }
    };
   
   typedef eosio::multi_index<"ramdebts"_n, ram_debt_record> ram_debts_table;


   typedef eosio::multi_index< "userres"_n, user_resources >      user_resources_table;
   typedef eosio::multi_index< "delband"_n, delegated_bandwidth > del_bandwidth_table;
   typedef eosio::multi_index< "refunds"_n, refund_request >      refunds_table;
   
   struct powerup_config {
      std::optional<uint32_t> powerup_days;     // `powerup` `days` argument must match this. Do not specify to preserve the
                                                //    existing setting or use the default.
      std::optional<asset>    min_powerup_fee;  // Fees below this amount are rejected. Do not specify to preserve the
                                                //    existing setting (no default exists).

      EOSLIB_SERIALIZE( powerup_config, (powerup_days)(min_powerup_fee) )
   };

   struct powerup_state_resource {
      int64_t        weight                  = 0;                  // resource market weight. calculated; varies over time.
                                                                   //    1 represents the same amount of resources as 1
                                                                   //    satoshi of SYS staked.
      int64_t        utilization             = 0;                  // Instantaneous resource utilization. This is the current
                                                                   //    amount sold. utilization <= weight.
   };

   struct [[eosio::table("powerstate"),eosio::contract("eosio.system")]] powerup_state {
      static constexpr uint32_t default_powerup_days = 30; // 30 day resource powerup
      powerup_state_resource     net               = {};                     // NET market state
      powerup_state_resource     cpu               = {};                     // CPU market state
      powerup_state_resource     ram               = {};                     // RAM market state
      
      uint32_t                   powerup_days      = default_powerup_days;   // `powerup` `days` argument must match this.
      asset                      min_powerup_fee   = {};                     // fees below this amount are rejected

      uint64_t primary_key()const { return 0; }
   };

   typedef eosio::singleton<"powerstate"_n, powerup_state> powerup_state_singleton;



  struct [[eosio::table("emission"),eosio::contract("eosio.system")]] emission_state {
      uint64_t                   tact_number           = 1;                       // Номер такта
      uint64_t                   tact_duration     = 86400;                   // Время продолжительности такта
      double                     emission_factor  = double(0.618);           // Фактор эмиссии
      asset                      current_supply;                             // Объем токенов в системе
      eosio::time_point_sec      tact_open_at;                               // Дата открытия такта
      eosio::time_point_sec      tact_close_at;                              // Дата закрытия такта
      asset                      tact_fees;                                  // Накопленные комиссии такта
      asset                      back_from_producers;                        // Вернулось в фонд от делегатских комиссий
      asset                      tact_emission;                              // Накопленная эмиссия такта
      asset                      emission_start;                    // Подвижная граница начала эмиссии в такте
      uint64_t primary_key()const { return 0; }
   };

   typedef eosio::singleton<"emission"_n, emission_state> emission_state_singleton;




   struct [[eosio::table("powup.order"),eosio::contract("eosio.system")]] powerup_order {
      uint8_t              version = 0;
      uint64_t             id;
      name                 owner;
      int64_t              net_weight;
      int64_t              cpu_weight;
      int64_t              ram_bytes;
      time_point_sec       expires;

      uint64_t primary_key()const { return id; }
      uint64_t by_owner()const    { return owner.value; }
      uint64_t by_expires()const  { return expires.utc_seconds; }
   };

   typedef eosio::multi_index< "powup.order"_n, powerup_order,
                               indexed_by<"byowner"_n, const_mem_fun<powerup_order, uint64_t, &powerup_order::by_owner>>,
                               indexed_by<"byexpires"_n, const_mem_fun<powerup_order, uint64_t, &powerup_order::by_expires>>
                               > powerup_order_table;

   /**
    * The `eosio.system` smart contract is provided by `block.one` as a sample system contract, and it defines the structures and actions needed for blockchain's core functionality.
    *
    * Just like in the `eosio.bios` sample contract implementation, there are a few actions which are not implemented at the contract level (`newaccount`, `updateauth`, `deleteauth`, `linkauth`, `unlinkauth`, `canceldelay`, `onerror`, `setabi`, `setcode`), they are just declared in the contract so they will show in the contract's ABI and users will be able to push those actions to the chain via the account holding the `eosio.system` contract, but the implementation is at the EOSIO core level. They are referred to as EOSIO native actions.
    *
    * - Users can stake tokens for CPU and Network bandwidth, and then vote for producers or
    *    delegate their vote to a proxy.
    * - Producers register in order to be voted for, and can claim per-block and per-vote rewards.
    * - Users can buy and sell RAM at a market-determined price.
    * - Users can bid on premium names.
    */
   class [[eosio::contract("eosio.system")]] system_contract : public native {

      private:
         voters_table             _voters;
         producers_table          _producers;
         global_state_singleton   _global;
         eosio_global_state       _gstate;
         rammarket                _rammarket;
         
      public:
         static constexpr eosio::name active_permission{"active"_n};
         static constexpr eosio::name token_account{"eosio.token"_n};
         static constexpr eosio::name ram_account{"eosio.ram"_n};
         static constexpr eosio::name ramfee_account{"eosio.ramfee"_n};
         static constexpr eosio::name stake_account{"eosio.stake"_n};
         static constexpr eosio::name bpay_account{"eosio.bpay"_n};
         static constexpr eosio::name vpay_account{"eosio.vpay"_n};
         static constexpr eosio::name names_account{"eosio.names"_n};
         static constexpr eosio::name null_account{"eosio.null"_n};
         static constexpr symbol ramcore_symbol = symbol(symbol_code("RAMCORE"), 4);
         static constexpr symbol ram_symbol     = symbol(symbol_code("RAM"), 0);
         
         system_contract( name s, name code, datastream<const char*> ds );
         ~system_contract();

          // Returns the core symbol by system account name
          // @param system_account - the system account to get the core symbol for.
         static symbol get_core_symbol( name system_account = "eosio"_n ) {
            rammarket rm(system_account, system_account.value);
            const static auto sym = get_core_symbol( rm );
            return sym;
         }

         // Actions:
         /**
          * The Init action initializes the system contract for a version and a symbol.
          * Only succeeds when:
          * - version is 0 and
          * - symbol is found and
          * - system token supply is greater than 0,
          * - and system contract wasn’t already been initialized.
          *
          * @param version - the version, has to be 0,
          * @param core - the system symbol.
          */
         [[eosio::action]]
         void init( uint64_t version, const symbol& core );

        /**
         * @brief Метод миграции вызывается после деплоя для внесения моментальных изменений
         * 
         */
         [[eosio::action]]
         void migrate( );

         /**
          * @brief      Метод обновления активного ключа пользователя кооперативами.
          *
          * @param[in]  account     The account
          * @param[in]  permission  The permission
          * @param[in]  parent      The parent
          * @param[in]  auth        The auth
          */
         [[eosio::action]]
         void changekey(name  account,
                        name  permission,
                        name  parent,
                        authority auth);

         

          [[eosio::action]]
          void setcode( const name& account, uint8_t vmtype, uint8_t vmversion, const std::vector<char>& code, const binary_extension<std::string>& memo );


         /**
          * Инициализация тактовой эмиссии
          * @param tact_duration - продолжительность такта в секундах,
          * @param emission_factor - множитель эмиссии от 0 до 2.618.
          */
         [[eosio::action]]
         void initemission(eosio::asset init_supply, uint64_t tact_duration, double emission_factor);

         
         /**
          * On block action. This special action is triggered when a block is applied by the given producer
          * and cannot be generated from any other source. It is used to pay producers and calculate
          * missed blocks of other producers. Producer pay is deposited into the producer's stake
          * balance and can be withdrawn over time. Once a minute, it may update the active producer config from the
          * producer votes. The action also populates the blockinfo table.
          *
          * @param header - the block header produced.
          */
         [[eosio::action]]
         void onblock( ignore<block_header> header );

         /**
          * Set account limits action sets the resource limits of an account
          *
          * @param account - name of the account whose resource limit to be set,
          * @param ram_bytes - ram limit in absolute bytes,
          * @param net_weight - fractionally proportionate net limit of available resources based on (weight / total_weight_of_all_accounts),
          * @param cpu_weight - fractionally proportionate cpu limit of available resources based on (weight / total_weight_of_all_accounts).
          */
         [[eosio::action]]
         void setalimits( const name& account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight );

         /**
          * Set account RAM limits action, which sets the RAM limits of an account
          *
          * @param account - name of the account whose resource limit to be set,
          * @param ram_bytes - ram limit in absolute bytes.
          */
         [[eosio::action]]
         void setacctram( const name& account, const std::optional<int64_t>& ram_bytes );

         /**
          * Set account NET limits action, which sets the NET limits of an account
          *
          * @param account - name of the account whose resource limit to be set,
          * @param net_weight - fractionally proportionate net limit of available resources based on (weight / total_weight_of_all_accounts).
          */
         [[eosio::action]]
         void setacctnet( const name& account, const std::optional<int64_t>& net_weight );

         /**
          * Set account CPU limits action, which sets the CPU limits of an account
          *
          * @param account - name of the account whose resource limit to be set,
          * @param cpu_weight - fractionally proportionate cpu limit of available resources based on (weight / total_weight_of_all_accounts).
          */
         [[eosio::action]]
         void setacctcpu( const name& account, const std::optional<int64_t>& cpu_weight );


         /**
          * The activate action, activates a protocol feature
          *
          * @param feature_digest - hash of the protocol feature to activate.
          */
         [[eosio::action]]
         void activate( const eosio::checksum256& feature_digest );

         // functions defined in delegate_bandwidth.cpp

         /**
          * Delegate bandwidth and/or cpu action. Stakes SYS from the balance of `from` for the benefit of `receiver`.
          *
          * @param from - the account to delegate bandwidth from, that is, the account holding
          *    tokens to be staked,
          * @param receiver - the account to delegate bandwidth to, that is, the account to
          *    whose resources staked tokens are added
          * @param stake_net_quantity - tokens staked for NET bandwidth,
          * @param stake_cpu_quantity - tokens staked for CPU bandwidth,
          * @param transfer - if true, ownership of staked tokens is transferred to `receiver`.
          *
          * @post All producers `from` account has voted for will have their votes updated immediately.
          */
         [[eosio::action]]
         void delegatebw( const name& from, const name& receiver,
                          const asset& stake_net_quantity, const asset& stake_cpu_quantity, bool transfer );

         
         /**
          * Undelegate bandwidth action, decreases the total tokens delegated by `from` to `receiver` and/or
          * frees the memory associated with the delegation if there is nothing
          * left to delegate.
          * This will cause an immediate reduction in net/cpu bandwidth of the
          * receiver.
          * A transaction is scheduled to send the tokens back to `from` after
          * the staking period has passed. If existing transaction is scheduled, it
          * will be canceled and a new transaction issued that has the combined
          * undelegated amount.
          * The `from` account loses voting power as a result of this call and
          * all producer tallies are updated.
          *
          * @param from - the account to undelegate bandwidth from, that is,
          *    the account whose tokens will be unstaked,
          * @param receiver - the account to undelegate bandwidth to, that is,
          *    the account to whose benefit tokens have been staked,
          * @param unstake_net_quantity - tokens to be unstaked from NET bandwidth,
          * @param unstake_cpu_quantity - tokens to be unstaked from CPU bandwidth,
          *
          * @post Unstaked tokens are transferred to `from` liquid balance via a
          *    deferred transaction with a delay of 3 days.
          * @post If called during the delay period of a previous `undelegatebw`
          *    action, pending action is canceled and timer is reset.
          * @post All producers `from` account has voted for will have their votes updated immediately.
          * @post Bandwidth and storage for the deferred transaction are billed to `from`.
          */
         [[eosio::action]]
         void undelegatebw( const name& from, const name& receiver,
                            const asset& unstake_net_quantity, const asset& unstake_cpu_quantity );

         /**
          * Buy ram action, increases receiver's ram quota based upon current price and quantity of
          * tokens provided. An inline transfer from receiver to system contract of
          * tokens will be executed.
          *
          * @param payer - the ram buyer,
          * @param receiver - the ram receiver,
          * @param quant - the quantity of tokens to buy ram with.
          */
         [[eosio::action]]
         void buyram( const name& payer, const name& receiver, const asset& quant );


         [[eosio::action]]
         void sellram( const name& account, int64_t bytes );


         /**
          * Buy a specific amount of ram bytes action. Increases receiver's ram in quantity of bytes provided.
          * An inline transfer from receiver to system contract of tokens will be executed.
          *
          * @param payer - the ram buyer,
          * @param receiver - the ram receiver,
          * @param bytes - the quantity of ram to buy specified in bytes.
          */
         [[eosio::action]]
         void buyrambytes( const name& payer, const name& receiver, uint32_t bytes );

         /**
          * Refund action, this action is called after the delegation-period to claim all pending
          * unstaked tokens belonging to owner.
          *
          * @param owner - the owner of the tokens claimed.
          */
         [[eosio::action]]
         void refund( const name& owner );

         // functions defined in voting.cpp

         /**
          * Register producer action, indicates that a particular account wishes to become a producer,
          * this action will create a `producer_config` and a `producer_info` object for `producer` scope
          * in producers tables.
          *
          * @param producer - account registering to be a producer candidate,
          * @param producer_key - the public key of the block producer, this is the key used by block producer to sign blocks,
          * @param url - the url of the block producer, normally the url of the block producer presentation website,
          * @param location - is the country code as defined in the ISO 3166, https://en.wikipedia.org/wiki/List_of_ISO_3166_country_codes
          *
          * @pre Producer to register is an account
          * @pre Authority of producer to register
          */
         [[eosio::action]]
         void regproducer( const name& producer, const public_key& producer_key, const std::string& url, uint16_t location );

         /**
          * Register producer action, indicates that a particular account wishes to become a producer,
          * this action will create a `producer_config` and a `producer_info` object for `producer` scope
          * in producers tables.
          *
          * @param producer - account registering to be a producer candidate,
          * @param producer_authority - the weighted threshold multisig block signing authority of the block producer used to sign blocks,
          * @param url - the url of the block producer, normally the url of the block producer presentation website,
          * @param location - is the country code as defined in the ISO 3166, https://en.wikipedia.org/wiki/List_of_ISO_3166_country_codes
          *
          * @pre Producer to register is an account
          * @pre Authority of producer to register
          */
         [[eosio::action]]
         void regproducer2( const name& producer, const eosio::block_signing_authority& producer_authority, const std::string& url, uint16_t location );

         /**
          * Unregister producer action, deactivates the block producer with account name `producer`.
          *
          * Deactivate the block producer with account name `producer`.
          * @param producer - the block producer account to unregister.
          */
         [[eosio::action]]
         void unregprod( const name& producer );

         /**
          * Set ram action sets the ram supply.
          * @param max_ram_size - the amount of ram supply to set.
          */
         [[eosio::action]]
         void setram( uint64_t max_ram_size );

         /**
          * Set ram rate action, sets the rate of increase of RAM in bytes per block. It is capped by the uint16_t to
          * a maximum rate of 3 TB per year. If update_ram_supply hasn't been called for the most recent block,
          * then new ram will be allocated at the old rate up to the present block before switching the rate.
          *
          * @param bytes_per_block - the amount of bytes per block increase to set.
          */
         [[eosio::action]]
         void setramrate( uint16_t bytes_per_block );

         /**
          * Vote producer action, votes for a set of producers. This action updates the list of `producers` voted for,
          * for `voter` account. If voting for a `proxy`, the producer votes will not change until the
          * proxy updates their own vote. Voter can vote for a proxy __or__ a list of at most 30 producers.
          * Storage change is billed to `voter`.
          *
          * @param voter - the account to change the voted producers for,
          * @param proxy - the proxy to change the voted producers for,
          * @param producers - the list of producers to vote for, a maximum of 30 producers is allowed.
          *
          * @pre Producers must be sorted from lowest to highest and must be registered and active
          * @pre If proxy is set then no producers can be voted for
          * @pre If proxy is set then proxy account must exist and be registered as a proxy
          * @pre Every listed producer or proxy must have been previously registered
          * @pre Voter must authorize this action
          * @pre Voter must have previously staked some EOS for voting
          * @pre Voter->staked must be up to date
          *
          * @post Every producer previously voted for will have vote reduced by previous vote weight
          * @post Every producer newly voted for will have vote increased by new vote amount
          * @post Prior proxy will proxied_vote_weight decremented by previous vote weight
          * @post New proxy will proxied_vote_weight incremented by new vote weight
          */
         [[eosio::action]]
         void voteproducer( const name& voter, const name& proxy, const std::vector<name>& producers );

         /**
          * Update the vote weight for the producers or proxy `voter_name` currently votes for. 
          * 
          * @param voter_name - the account to update the votes for,
          * 
          * @post the voter.staked will be updated
          * @post previously voted for producers vote weight will be updated with new weight
          * @post previously voted for proxy vote weight will be updated with new weight
          */
         [[eosio::action]]
         void voteupdate( const name& voter_name );

         /**
          * Register proxy action, sets `proxy` account as proxy.
          * An account marked as a proxy can vote with the weight of other accounts which
          * have selected it as a proxy. Other accounts must refresh their voteproducer to
          * update the proxy's weight.
          * Storage change is billed to `proxy`.
          *
          * @param proxy - the account registering as voter proxy (or unregistering),
          * @param isproxy - if true, proxy is registered; if false, proxy is unregistered.
          *
          * @pre Proxy must have something staked (existing row in voters table)
          * @pre New state must be different than current state
          */
         [[eosio::action]]
         void regproxy( const name& proxy, bool isproxy );

         /**
          * Set the blockchain parameters. By tunning these parameters a degree of
          * customization can be achieved.
          * @param params - New blockchain parameters to set.
          */
         [[eosio::action]]
         void setparams( const blockchain_parameters_t& params );

#ifdef SYSTEM_CONFIGURABLE_WASM_LIMITS
         /**
          * Sets the WebAssembly limits.  Valid parameters are "low",
          * "default" (equivalent to low), and "high".  A value of "high"
          * allows larger contracts to be deployed.
          */
         [[eosio::action]]
         void wasmcfg( const name& settings );
#endif

         /**
          * Claim rewards action, claims block producing and vote rewards.
          * @param owner - producer account claiming per-block and per-vote rewards.
          */
         [[eosio::action]]
         void claimrewards( const name& owner );

         /**
          * Set privilege status for an account. Allows to set privilege status for an account (turn it on/off).
          * @param account - the account to set the privileged status for.
          * @param is_priv - 0 for false, > 0 for true.
          */
         [[eosio::action]]
         void setpriv( const name& account, uint8_t is_priv );

         /**
          * Remove producer action, deactivates a producer by name, if not found asserts.
          * @param producer - the producer account to deactivate.
          */
         [[eosio::action]]
         void rmvproducer( const name& producer );

         /**
          * Update revision action, updates the current revision.
          * @param revision - it has to be incremented by 1 compared with current revision.
          *
          * @pre Current revision can not be higher than 254, and has to be smaller
          * than or equal 1 (“set upper bound to greatest revision supported in the code”).
          */
         [[eosio::action]]
         void updtrevision( uint8_t revision );

         /**
          * Bid name action, allows an account `bidder` to place a bid for a name `newname`.
          * @param bidder - the account placing the bid,
          * @param newname - the name the bid is placed for,
          * @param bid - the amount of system tokens payed for the bid.
          *
          * @pre Bids can be placed only on top-level suffix,
          * @pre Non empty name,
          * @pre Names longer than 12 chars are not allowed,
          * @pre Names equal with 12 chars can be created without placing a bid,
          * @pre Bid has to be bigger than zero,
          * @pre Bid's symbol must be system token,
          * @pre Bidder account has to be different than current highest bidder,
          * @pre Bid must increase current bid by 10%,
          * @pre Auction must still be opened.
          */
         [[eosio::action]]
         void bidname( const name& bidder, const name& newname, const asset& bid );

         /**
          * Bid refund action, allows the account `bidder` to get back the amount it bid so far on a `newname` name.
          *
          * @param bidder - the account that gets refunded,
          * @param newname - the name for which the bid was placed and now it gets refunded for.
          */
         [[eosio::action]]
         void bidrefund( const name& bidder, const name& newname );

         /**
          * Configure the `power` market. The market becomes available the first time this
          * action is invoked.
          */
         [[eosio::action]]
         void cfgpowerup( powerup_config& args );

         /**
          * Process power queue and update state. Action does not execute anything related to a specific user.
          *
          * @param user - any account can execute this action
          * @param max - number of queue items to process
          */
         [[eosio::action]]
         void powerupexec( const name& user, uint16_t max );

         /**
          * Powerup NET and CPU resources by percentage
          *
          * @param payer - the resource buyer
          * @param receiver - the resource receiver
          * @param days - number of days of resource availability. Must match market configuration.
          * @param net_frac - fraction of net (100% = 10^15) managed by this market
          * @param cpu_frac - fraction of cpu (100% = 10^15) managed by this market
          * @param max_payment - the maximum amount `payer` is willing to pay. Tokens are withdrawn from
          *    `payer`'s token balance.
          */
         [[eosio::action]]
         void powerup(const name& payer, const name& receiver, uint32_t days, const asset& payment, const bool transfer = false);

         /**
          * limitauthchg opts into or out of restrictions on updateauth, deleteauth, linkauth, and unlinkauth.
          *
          * If either allow_perms or disallow_perms is non-empty, then opts into restrictions. If
          * allow_perms is non-empty, then the authorized_by argument of the restricted actions must be in
          * the vector, or the actions will abort. If disallow_perms is non-empty, then the authorized_by
          * argument of the restricted actions must not be in the vector, or the actions will abort.
          *
          * If both allow_perms and disallow_perms are empty, then opts out of the restrictions. limitauthchg
          * aborts if both allow_perms and disallow_perms are non-empty.
          *
          * @param account - account to change
          * @param allow_perms - permissions which may use the restricted actions
          * @param disallow_perms - permissions which may not use the restricted actions
          */
         [[eosio::action]]
         void limitauthchg( const name& account, const std::vector<name>& allow_perms, const std::vector<name>& disallow_perms );
        
         [[eosio::action]]
         void createaccnt(const name coopname, const name new_account_name, authority owner, authority active);

         using init_action = eosio::action_wrapper<"init"_n, &system_contract::init>;
         using setcode_action = eosio::action_wrapper<"setcode"_n, &system_contract::setcode>;
         using initemission_action = eosio::action_wrapper<"initemission"_n, &system_contract::initemission>;
         
         using setacctram_action = eosio::action_wrapper<"setacctram"_n, &system_contract::setacctram>;
         using setacctnet_action = eosio::action_wrapper<"setacctnet"_n, &system_contract::setacctnet>;
         using setacctcpu_action = eosio::action_wrapper<"setacctcpu"_n, &system_contract::setacctcpu>;
         
         using activate_action = eosio::action_wrapper<"activate"_n, &system_contract::activate>;
         using delegatebw_action = eosio::action_wrapper<"delegatebw"_n, &system_contract::delegatebw>;
         using undelegatebw_action = eosio::action_wrapper<"undelegatebw"_n, &system_contract::undelegatebw>;
         
         using buyram_action = eosio::action_wrapper<"buyram"_n, &system_contract::buyram>;
         using buyrambytes_action = eosio::action_wrapper<"buyrambytes"_n, &system_contract::buyrambytes>;
         
         using refund_action = eosio::action_wrapper<"refund"_n, &system_contract::refund>;
         
         using regproducer_action = eosio::action_wrapper<"regproducer"_n, &system_contract::regproducer>;
         using regproducer2_action = eosio::action_wrapper<"regproducer2"_n, &system_contract::regproducer2>;
         
         using unregprod_action = eosio::action_wrapper<"unregprod"_n, &system_contract::unregprod>;
         
         using setram_action = eosio::action_wrapper<"setram"_n, &system_contract::setram>;
         using setramrate_action = eosio::action_wrapper<"setramrate"_n, &system_contract::setramrate>;
         
         
         using voteproducer_action = eosio::action_wrapper<"voteproducer"_n, &system_contract::voteproducer>;
         using voteupdate_action = eosio::action_wrapper<"voteupdate"_n, &system_contract::voteupdate>;
         
         using regproxy_action = eosio::action_wrapper<"regproxy"_n, &system_contract::regproxy>;
         
         using claimrewards_action = eosio::action_wrapper<"claimrewards"_n, &system_contract::claimrewards>;
         
         using rmvproducer_action = eosio::action_wrapper<"rmvproducer"_n, &system_contract::rmvproducer>;
         
         using updtrevision_action = eosio::action_wrapper<"updtrevision"_n, &system_contract::updtrevision>;
         
         using bidname_action = eosio::action_wrapper<"bidname"_n, &system_contract::bidname>;
         using bidrefund_action = eosio::action_wrapper<"bidrefund"_n, &system_contract::bidrefund>;
         
         using setpriv_action = eosio::action_wrapper<"setpriv"_n, &system_contract::setpriv>;
         using setalimits_action = eosio::action_wrapper<"setalimits"_n, &system_contract::setalimits>;
         
         using setparams_action = eosio::action_wrapper<"setparams"_n, &system_contract::setparams>;
         
         using cfgpowerup_action = eosio::action_wrapper<"cfgpowerup"_n, &system_contract::cfgpowerup>;
         using powerupexec_action = eosio::action_wrapper<"powerupexec"_n, &system_contract::powerupexec>;
         using powerup_action = eosio::action_wrapper<"powerup"_n, &system_contract::powerup>;
         
      private:
         // Implementation details:

         static symbol get_core_symbol( const rammarket& rm ) {
            auto itr = rm.find(ramcore_symbol.raw());
            check(itr != rm.end(), "system contract must first be initialized");
            return itr->quote.balance.symbol;
         }

         //defined in eosio.system.cpp
         static eosio_global_state get_default_parameters();
         int64_t update_ram_debt_table(name payer, name account, int64_t ram_bytes);
         
         void emit(eosio::asset new_emission);     
         
         symbol core_symbol()const;
         void update_ram_supply();
         
         // defined in delegate_bandwidth.cpp
         void changebw( name from, const name& receiver,
                        const asset& stake_net_quantity, const asset& stake_cpu_quantity, bool transfer );
         void update_voting_power( const name& voter, const asset& total_update );
         
         // defined in voting.cpp
         void register_producer( const name& producer, const eosio::block_signing_authority& producer_authority, const std::string& url, uint16_t location );
         void update_elected_producers( const block_timestamp& timestamp );
         void update_votes( const name& voter, const name& proxy, const std::vector<name>& producers, bool voting );
         void propagate_weight_change( const voter_info& voter );
         
         // defined in power.cpp
         void fill_tact(eosio::name payer, eosio::asset payment);
         void adjust_resources(name payer, name account, symbol core_symbol, int64_t net_delta, int64_t cpu_delta, int64_t ram_delta, bool must_not_be_managed = false);
         void process_powerup_queue(
            time_point_sec now, symbol core_symbol, powerup_state& state,
            powerup_order_table& orders, uint32_t max_items, int64_t& net_delta_available,
            int64_t& cpu_delta_available, int64_t& ram_delta_available);
         emission_state update_tact(emission_state state);
         void change_weights(eosio::name payer, eosio::asset new_emission);
         // defined in block_info.cpp
         void add_to_blockinfo_table(const eosio::checksum256& previous_block_id, const eosio::block_timestamp timestamp) const;
         
   };

}
