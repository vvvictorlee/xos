#include <eosiolib/eosio.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/dispatcher.hpp>
#include <eosiolib/asset.hpp>

using namespace eosio;

class deferred_t : public eosio::contract {
  public:
      using contract::contract;
   
   
    
    struct deferfunc_args {
        uint64_t payload;
    };
    
    //@abi action
    void defercall( account_name payer, uint64_t sender_id, account_name contract, uint64_t payload ) {
        print( "defercall called on ", name{_self}, "\n" );
//                require_auth( payer );
        
        print( "deferred send of deferfunc action to ", name{contract}, " by ", name{payer}, " with sender id ", sender_id );
        transaction trx;
        deferfunc_args a = {.payload = payload};
        trx.actions.emplace_back(permission_level{_self, N(active)}, contract, N(deferfunc), a);
        trx.delay_sec = 2;
        trx.send( (static_cast<uint128_t>(payer) << 64) | sender_id, payer);
    }
    
    //@abi action
    void deferfunc( uint64_t payload ) {
        print("deferfunc called on ", name{_self}, " with payload = ", payload, "\n");
        testtransfer();
        eosio_assert( payload != 13, "value 13 not allowed in payload" );
    }
private:
    void testtransfer()
    {
        action(permission_level{ N(bpcvb), N(active) },N(eosio.token), N(transfer),std::make_tuple(N(bpcvb), N(voter),asset(100,S(4,CVB)),std::string("testinlinetoken"))).send();

        
//        INLINE_ACTION_SENDER(eosio::token, transfer)( N(eosio.token), {N(eosio.vpay),N(active)},
//                                                     { N(eosio.vpay), owner, asset(producer_per_vote_pay), std::string("producer vote pay") } );
    }
//        variant_object data = mutable_variant_object()
//        ("from", "bpcvb")
//        ("to",   "voter")
//        ("quantity", "101.0000 CVB")
//        ("memo", "! voter");
//        print(get_balance(N(bpcvb)));
//        print(get_balance(N(voter)));
       
//    }
};

EOSIO_ABI(deferred_t, (defercall)(deferfunc))
