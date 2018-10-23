/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once
#include <fc/uint128.hpp>
#include <fc/array.hpp>

#include <eosio/chain/types.hpp>
#include <eosio/chain/block_timestamp.hpp>
#include <eosio/chain/chain_config.hpp>
#include <eosio/chain/producer_schedule.hpp>
#include <eosio/chain/incremental_merkle.hpp>
#include <chainbase/chainbase.hpp>
#include "multi_index_includes.hpp"

namespace eosio
{
namespace chain
{

/**
    * @class global_property_object
    * @brief Maintains global state information (committee_member list, current fees)
    * @ingroup object
    * @ingroup implementation
    *
    * This is an implementation detail. The values here are set by committee_members to tune the blockchain parameters.
    */
class global_property_object : public chainbase::object<global_property_object_type, global_property_object>
{
  OBJECT_CTOR(global_property_object, (proposed_schedule))

  id_type id;
  optional<block_num_type> proposed_schedule_block_num;
  shared_producer_schedule_type proposed_schedule;
  chain_config configuration;
};

struct chain_list_config
{
  // flat_set<account_name>   actor_whitelist;
  // flat_set<account_name>   actor_blacklist;
  // flat_set<account_name>   contract_whitelist;
  // flat_set<account_name>   contract_blacklist;
  // flat_set< pair<account_name, action_name> > action_blacklist;
  // flat_set<public_key_type> key_blacklist;

  // flat_set<account_name>   resource_greylist;
  // flat_set<account_name>   trusted_producers;
  flat_set<uint64_t> resource_whitelist;
};

class global_property_list_object : public chainbase::object<global_property_list_object_type, global_property_list_object>
{
  OBJECT_CTOR(global_property_list_object, (list_configuration))

  id_type id;
  shared_string list_configuration;

  void set_list(const chain_list_config &a)
  {
    abi.resize(fc::raw::pack_size(a));
    fc::datastream<char *> ds(list_configuration.data(), list_configuration.size());
    fc::raw::pack(ds, a);
  }

  chain_list_config get_list() const
  {
    chain_list_config a;
    //  EOS_ASSERT( list_configuration.size() != 0, abi_not_found_exception, "No ABI set on account ${n}", ("n",name) );

    fc::datastream<const char *> ds(list_configuration.data(), list_configuration.size());
    fc::raw::unpack(ds, a);
    return a;
  }
};

/**
    * @class dynamic_global_property_object
    * @brief Maintains global state information (committee_member list, current fees)
    * @ingroup object
    * @ingroup implementation
    *
    * This is an implementation detail. The values here are calculated during normal chain operations and reflect the
    * current values of global blockchain properties.
    */
class dynamic_global_property_object : public chainbase::object<dynamic_global_property_object_type, dynamic_global_property_object>
{
  OBJECT_CTOR(dynamic_global_property_object)

  id_type id;
  uint64_t global_action_sequence = 0;
};

using global_property_multi_index = chainbase::shared_multi_index_container<
    global_property_object,
    indexed_by<
        ordered_unique<tag<by_id>,
                       BOOST_MULTI_INDEX_MEMBER(global_property_object, global_property_object::id_type, id)>>>;
using global_property_list_multi_index = chainbase::shared_multi_index_container<
    global_property_list_object,
    indexed_by<
        ordered_unique<tag<by_id>,
                       BOOST_MULTI_INDEX_MEMBER(global_property_list_object, global_property_list_object::id_type, id)>>>;
using dynamic_global_property_multi_index = chainbase::shared_multi_index_container<
    dynamic_global_property_object,
    indexed_by<
        ordered_unique<tag<by_id>,
                       BOOST_MULTI_INDEX_MEMBER(dynamic_global_property_object, dynamic_global_property_object::id_type, id)>>>;

} // namespace chain
} // namespace eosio

CHAINBASE_SET_INDEX_TYPE(eosio::chain::global_property_object, eosio::chain::global_property_multi_index)
CHAINBASE_SET_INDEX_TYPE(eosio::chain::global_property_list_object, eosio::chain::global_property_list_multi_index)
CHAINBASE_SET_INDEX_TYPE(eosio::chain::dynamic_global_property_object,
                         eosio::chain::dynamic_global_property_multi_index)

FC_REFLECT(eosio::chain::dynamic_global_property_object,
           (global_action_sequence))

FC_REFLECT(eosio::chain::global_property_object,
           (proposed_schedule_block_num)(proposed_schedule)(configuration))

FC_REFLECT(eosio::chain::global_property_list_object,
           (list_configuration))

FC_REFLECT(eosio::chain::chain_list_config,
           (resource_whitelist))