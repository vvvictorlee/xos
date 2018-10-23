#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>

#include <eoxtoken/eoxtoken.wast.hpp>
#include <eoxtoken/eoxtoken.abi.hpp>

#include <Runtime/Runtime.h>

#include <fc/variant_object.hpp>

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;
using namespace std;

using mvo = fc::mutable_variant_object;

class eoxtoken_tester : public tester {
public:

   eoxtoken_tester() {
      produce_blocks( 2 );

      create_accounts( { N(alice), N(bob), N(carol), N(eoxtoken) } );
      produce_blocks( 2 );

      set_code( N(eoxtoken), eoxtoken_wast );
      set_abi( N(eoxtoken), eoxtoken_abi );

      produce_blocks();

    //   const auto& accnt = control->db().get<account_object,by_name>( N(eoxtoken) );
    //   abi_def abi;
    //   BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
    //   abi_ser.set_abi(abi);
   }

   action_result push_action( const account_name& signer, const action_name &name, const variant_object &data ) {
      string action_type_name = abi_ser.get_action_type(name);

      action act;
      act.account = N(eoxtoken);
      act.name    = name;
      act.data    = abi_ser.variant_to_binary( action_type_name, data,fc::microseconds(999'999) );

      return base_tester::push_action( std::move(act), uint64_t(signer));
   }

//    fc::variant get_stats( const string& symbolname )
//    {
//       auto symb = eosio::chain::symbol::from_string(symbolname);
//       auto symbol_code = symb.to_symbol_code().value;
//       vector<char> data = get_row_by_account( N(eoxtoken), symbol_code, N(stat), symbol_code );
//       return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "currency_stats", data );
//    }

//    fc::variant get_account( account_name acc, const string& symbolname)
//    {
//       auto symb = eosio::chain::symbol::from_string(symbolname);
//       auto symbol_code = symb.to_symbol_code().value;
//       vector<char> data = get_row_by_account( N(eoxtoken), acc, N(accounts), symbol_code );
//       return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "account", data );
//    }

//    action_result create( account_name issuer,
//                 asset        maximum_supply ) {

//       return push_action( N(eoxtoken), N(create), mvo()
//            ( "issuer", issuer)
//            ( "maximum_supply", maximum_supply)
//       );
//    }

//    action_result issue( account_name issuer, account_name to, asset quantity, string memo ) {
//       return push_action( issuer, N(issue), mvo()
//            ( "to", to)
//            ( "quantity", quantity)
//            ( "memo", memo)
//       );
//    }

   action_result transfer(  account_name to,
                  uint64_t    _value ) {
      return push_action( N(eoxtoken), N(transfer), mvo()
           ( "_to", to)
           ( "_value", _value)
           );
   }

   abi_serializer abi_ser;
};

BOOST_AUTO_TEST_SUITE(eoxtoken_tests)

// BOOST_FIXTURE_TEST_CASE( create_tests, eoxtoken_tester ) try {

//    auto token = create( N(alice), asset::from_string("1000.000 TKN"));
//    auto stats = get_stats("3,TKN");
//    REQUIRE_MATCHING_OBJECT( stats, mvo()
//       ("supply", "0.000 TKN")
//       ("max_supply", "1000.000 TKN")
//       ("issuer", "alice")
//    );
//    produce_blocks(1);

// } FC_LOG_AND_RETHROW()

// BOOST_FIXTURE_TEST_CASE( create_negative_max_supply, eoxtoken_tester ) try {

//    BOOST_REQUIRE_EQUAL( wasm_assert_msg( "max-supply must be positive" ),
//       create( N(alice), asset::from_string("-1000.000 TKN"))
//    );

// } FC_LOG_AND_RETHROW()

// BOOST_FIXTURE_TEST_CASE( symbol_already_exists, eoxtoken_tester ) try {

//    auto token = create( N(alice), asset::from_string("100 TKN"));
//    auto stats = get_stats("0,TKN");
//    REQUIRE_MATCHING_OBJECT( stats, mvo()
//       ("supply", "0 TKN")
//       ("max_supply", "100 TKN")
//       ("issuer", "alice")
//    );
//    produce_blocks(1);

//    BOOST_REQUIRE_EQUAL( wasm_assert_msg( "token with symbol already exists" ),
//                         create( N(alice), asset::from_string("100 TKN"))
//    );

// } FC_LOG_AND_RETHROW()

// BOOST_FIXTURE_TEST_CASE( create_max_supply, eoxtoken_tester ) try {

//    auto token = create( N(alice), asset::from_string("4611686018427387903 TKN"));
//    auto stats = get_stats("0,TKN");
//    REQUIRE_MATCHING_OBJECT( stats, mvo()
//       ("supply", "0 TKN")
//       ("max_supply", "4611686018427387903 TKN")
//       ("issuer", "alice")
//    );
//    produce_blocks(1);

//    asset max(10, symbol(SY(0, NKT)));
//    share_type amount = 4611686018427387904;
//    static_assert(sizeof(share_type) <= sizeof(asset), "asset changed so test is no longer valid");
//    static_assert(std::is_trivially_copyable<asset>::value, "asset is not trivially copyable");
//    memcpy(&max, &amount, sizeof(share_type)); // hack in an invalid amount

//    BOOST_CHECK_EXCEPTION( create( N(alice), max) , asset_type_exception, [](const asset_type_exception& e) {
//       return expect_assert_message(e, "magnitude of asset amount must be less than 2^62");
//    });


// } FC_LOG_AND_RETHROW()

// BOOST_FIXTURE_TEST_CASE( create_max_decimals, eoxtoken_tester ) try {

//    auto token = create( N(alice), asset::from_string("1.000000000000000000 TKN"));
//    auto stats = get_stats("18,TKN");
//    REQUIRE_MATCHING_OBJECT( stats, mvo()
//       ("supply", "0.000000000000000000 TKN")
//       ("max_supply", "1.000000000000000000 TKN")
//       ("issuer", "alice")
//    );
//    produce_blocks(1);

//    asset max(10, symbol(SY(0, NKT)));
//    //1.0000000000000000000 => 0x8ac7230489e80000L
//    share_type amount = 0x8ac7230489e80000L;
//    static_assert(sizeof(share_type) <= sizeof(asset), "asset changed so test is no longer valid");
//    static_assert(std::is_trivially_copyable<asset>::value, "asset is not trivially copyable");
//    memcpy(&max, &amount, sizeof(share_type)); // hack in an invalid amount

//    BOOST_CHECK_EXCEPTION( create( N(alice), max) , asset_type_exception, [](const asset_type_exception& e) {
//       return expect_assert_message(e, "magnitude of asset amount must be less than 2^62");
//    });

// } FC_LOG_AND_RETHROW()

// BOOST_FIXTURE_TEST_CASE( issue_tests, eoxtoken_tester ) try {

//    auto token = create( N(alice), asset::from_string("1000.000 TKN"));
//    produce_blocks(1);

//    issue( N(alice), N(alice), asset::from_string("500.000 TKN"), "hola" );

//    auto stats = get_stats("3,TKN");
//    REQUIRE_MATCHING_OBJECT( stats, mvo()
//       ("supply", "500.000 TKN")
//       ("max_supply", "1000.000 TKN")
//       ("issuer", "alice")
//    );

//    auto alice_balance = get_account(N(alice), "3,TKN");
//    REQUIRE_MATCHING_OBJECT( alice_balance, mvo()
//       ("balance", "500.000 TKN")
//    );

//    BOOST_REQUIRE_EQUAL( wasm_assert_msg( "quantity exceeds available supply" ),
//       issue( N(alice), N(alice), asset::from_string("500.001 TKN"), "hola" )
//    );

//    BOOST_REQUIRE_EQUAL( wasm_assert_msg( "must issue positive quantity" ),
//       issue( N(alice), N(alice), asset::from_string("-1.000 TKN"), "hola" )
//    );

//    BOOST_REQUIRE_EQUAL( success(),
//       issue( N(alice), N(alice), asset::from_string("1.000 TKN"), "hola" )
//    );


// } FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( transfer_tests, eoxtoken_tester ) try {

//    auto token = create( N(alice), asset::from_string("1000 CERO"));
//    produce_blocks(1);

//    issue( N(alice), N(alice), asset::from_string("1000 CERO"), "hola" );

//    auto stats = get_stats("0,CERO");
//    REQUIRE_MATCHING_OBJECT( stats, mvo()
//       ("supply", "1000 CERO")
//       ("max_supply", "1000 CERO")
//       ("issuer", "alice")
//    );

//    auto alice_balance = get_account(N(alice), "0,CERO");
//    REQUIRE_MATCHING_OBJECT( alice_balance, mvo()
//       ("balance", "1000 CERO")
//    );

//    transfer( N(alice), N(bob), asset::from_string("300 CERO"), "hola" );

//    alice_balance = get_account(N(alice), "0,CERO");
//    REQUIRE_MATCHING_OBJECT( alice_balance, mvo()
//       ("balance", "700 CERO")
//       ("frozen", 0)
//       ("whitelist", 1)
//    );

//    auto bob_balance = get_account(N(bob), "0,CERO");
//    REQUIRE_MATCHING_OBJECT( bob_balance, mvo()
//       ("balance", "300 CERO")
//       ("frozen", 0)
//       ("whitelist", 1)
//    );

//    BOOST_REQUIRE_EQUAL( wasm_assert_msg( "overdrawn balance" ),
//       transfer( N(alice), N(bob), asset::from_string("701 CERO"), "hola" )
//    );

//    BOOST_REQUIRE_EQUAL( wasm_assert_msg( "must transfer positive quantity" ),
//       transfer( N(alice), N(bob), asset::from_string("-1000 CERO"), "hola" )
//    );


} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
