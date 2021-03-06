/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include <eosio/testing/tester.hpp>
#include <eosio/chain/global_property_object.hpp>
#include <fc/crypto/digest.hpp>
#include <boost/container/flat_set.hpp>
#include <eosio/chain/types.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#ifdef NON_VALIDATING_TEST
#define TESTER tester
#else
#define TESTER validating_tester
#endif


using namespace eosio::chain;
using namespace eosio::testing;
namespace bfs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(database_tests)

flat_set<account_name> parse_list_string(string items)
{
  flat_set<account_name> item_list;
vector<string> itemlist;
      boost::split(itemlist, items, boost::is_any_of(","));
      for (string item: itemlist) {
      
        //  try {
        //     node = boost::lexical_cast<uint16_t,string>(node_number);
        //  }
        //  catch(boost::bad_lexical_cast &) {
        //     // This exception will be handled below
        //  }
        //  if (node < 0 || node > 99) {
        //     cerr << "Bad node number found in node number list: " << node_number << endl;
        //     exit(-1);
        //  }
         item_list.insert(string_to_name(item.c_str()));
      }

      return item_list;
}


    // Simple tests of undo infrastructure
   BOOST_AUTO_TEST_CASE(list_config_parse_test) {
      try {
         TESTER test;
      //  // Bypass read-only restriction on state DB access for this unit test which really needs to mutate the DB to properly conduct its test.
      //    eosio::chain::database& db = const_cast<eosio::chain::database&>( test.control->db() );

      //    auto ses = db.start_undo_session(true);

      //    // Create an account
      //    db.create<global_property_list_object>([](global_property_list_object &a) {
      //            chain_list_config config;
      //            config.resource_whitelist={1};
      //      a.set_list(config);
      //    });

      //    // Make sure we can retrieve that account by name
      //    const global_property_list_object& ptr = db.get<global_property_list_object>();
      //   //   BOOST_TEST(a.list_configuration.size > 0);
      //    chain_list_config a  = ptr.get_list();
       
      //    uint64_t v = 0;
      //    if(a.resource_whitelist.size() > 0)
      //    {
      //          v =  *(a.resource_whitelist.begin());
      //    }
      string str= "alice,bob,tom";
      flat_set<account_name> list = parse_list_string(str);
         BOOST_TEST(list.size() > 0);
         account_name n =N(a);
         if(list.size()>0)
         {
            n = *(list.begin());
         }

         BOOST_TEST(n !=N(a));
         BOOST_TEST(n ==N(a));
          BOOST_TEST(n ==N(alice));
        //  BOOST_TEST(v == 1);
         // Undo creation of the account
        //  ses.undo();

        //  // Make sure we can no longer find the account
        //  ptr = db.find<global_property_list_object, by_name, std::string>("billy");
        //  BOOST_TEST(ptr == nullptr);
      } FC_LOG_AND_RETHROW()
   }

 // Simple tests of undo infrastructure
   BOOST_AUTO_TEST_CASE(list_config_test) {
      try {
         TESTER test;
       // Bypass read-only restriction on state DB access for this unit test which really needs to mutate the DB to properly conduct its test.
         eosio::chain::database& db = const_cast<eosio::chain::database&>( test.control->db() );

         auto ses = db.start_undo_session(true);

         // Create an account
         db.create<global_property_list_object>([](global_property_list_object &a) {
                 chain_list_config config;
                 config.resource_whitelist={1};
           a.set_list(config);
         });

         // Make sure we can retrieve that account by name
         const global_property_list_object& ptr = db.get<global_property_list_object>();
        //   BOOST_TEST(a.list_configuration.size > 0);
         chain_list_config a  = ptr.get_list();
       
         uint64_t v = 0;
         if(a.resource_whitelist.size() > 0)
         {
               v =  *(a.resource_whitelist.begin());
         }
         BOOST_TEST(v > 0);
         BOOST_TEST(v == 1);
         // Undo creation of the account
         ses.undo();

        //  // Make sure we can no longer find the account
        //  ptr = db.find<global_property_list_object, by_name, std::string>("billy");
        //  BOOST_TEST(ptr == nullptr);
      } FC_LOG_AND_RETHROW()
   }
   
   // Simple tests of undo infrastructure
   BOOST_AUTO_TEST_CASE(undo_test) {
      try {
         TESTER test;

         // Bypass read-only restriction on state DB access for this unit test which really needs to mutate the DB to properly conduct its test.
         eosio::chain::database& db = const_cast<eosio::chain::database&>( test.control->db() );

         auto ses = db.start_undo_session(true);

         // Create an account
         db.create<account_object>([](account_object &a) {
            a.name = "billy";
         });

         // Make sure we can retrieve that account by name
         auto ptr = db.find<account_object, by_name, std::string>("billy");
         BOOST_TEST(ptr != nullptr);

         // Undo creation of the account
         ses.undo();

         // Make sure we can no longer find the account
         ptr = db.find<account_object, by_name, std::string>("billy");
         BOOST_TEST(ptr == nullptr);
      } FC_LOG_AND_RETHROW()
   }


   // Test the block fetching methods on database, fetch_bock_by_id, and fetch_block_by_number
   BOOST_AUTO_TEST_CASE(get_blocks) {
      try {
         TESTER test;
         vector<block_id_type> block_ids;

         const uint32_t num_of_blocks_to_prod = 200;
         // Produce 200 blocks and check their IDs should match the above
         test.produce_blocks(num_of_blocks_to_prod);
         for (uint32_t i = 0; i < num_of_blocks_to_prod; ++i) {
            block_ids.emplace_back(test.control->fetch_block_by_number(i + 1)->id());
            BOOST_TEST(block_header::num_from_id(block_ids.back()) == i + 1);
            BOOST_TEST(test.control->fetch_block_by_number(i + 1)->id() == block_ids.back());
         }

         // Utility function to check expected irreversible block
         auto calc_exp_last_irr_block_num = [&](uint32_t head_block_num) -> uint32_t {
            const auto producers_size = test.control->head_block_state()->active_schedule.producers.size();
            const auto max_reversible_rounds = EOS_PERCENT(producers_size, config::percent_100 - config::irreversible_threshold_percent);
            if( max_reversible_rounds == 0) {
               return head_block_num;
            } else {
               const auto current_round = head_block_num / config::producer_repetitions;
               const auto irreversible_round = current_round - max_reversible_rounds;
               return (irreversible_round + 1) * config::producer_repetitions - 1;
            }
         };

         // Check the last irreversible block number is set correctly
         const auto expected_last_irreversible_block_number = calc_exp_last_irr_block_num(num_of_blocks_to_prod);
         BOOST_TEST(test.control->head_block_state()->dpos_irreversible_blocknum == expected_last_irreversible_block_number);
         // Check that block 201 cannot be found (only 20 blocks exist)
         BOOST_TEST(test.control->fetch_block_by_number(num_of_blocks_to_prod + 1 + 1) == nullptr);

         const uint32_t next_num_of_blocks_to_prod = 100;
         // Produce 100 blocks and check their IDs should match the above
         test.produce_blocks(next_num_of_blocks_to_prod);

         const auto next_expected_last_irreversible_block_number = calc_exp_last_irr_block_num(
                 num_of_blocks_to_prod + next_num_of_blocks_to_prod);
         // Check the last irreversible block number is updated correctly
         BOOST_TEST(test.control->head_block_state()->dpos_irreversible_blocknum == next_expected_last_irreversible_block_number);
         // Check that block 201 can now be found
         BOOST_CHECK_NO_THROW(test.control->fetch_block_by_number(num_of_blocks_to_prod + 1));
         // Check the latest head block match
         BOOST_TEST(test.control->fetch_block_by_number(num_of_blocks_to_prod + next_num_of_blocks_to_prod + 1)->id() ==
                    test.control->head_block_id());
      } FC_LOG_AND_RETHROW()
   }


BOOST_AUTO_TEST_SUITE_END()
