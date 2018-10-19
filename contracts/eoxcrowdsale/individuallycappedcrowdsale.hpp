// 
#pragma once
#include <eosiolib/eosio.hpp>
#include "./crowdsale.hpp"
#include <vector>


/**
 * @title IndividuallyCappedCrowdsale
 * @dev Crowdsale with per-user caps.
 */
class IndividuallyCappedCrowdsale :public Crowdsale,public eosio::contract {
  // using SafeMath for uint64_t;
IndividuallyCappedCrowdsale( account_name self ):contract(self),Crowdsale(0,0,nullptr){}
  // mapping(account_name => uint64_t) public contributions;
  // mapping(account_name => uint64_t) public caps;
 struct contribution
  {
    account_name name;
    uint64_t balance;

    uint64_t primary_key() const { return name; }
  };

  typedef eosio::multi_index<N(contribution), contribution> contributions;

   struct cap
  {
    account_name name;
    uint64_t balance;

    uint64_t primary_key() const { return name; }
  };

  typedef eosio::multi_index<N(cap), cap> caps;
  /**
   * @dev Sets a specific user's maximum contribution.
   * @param _beneficiary Address to be capped
   * @param _cap Wei limit for individual contribution
   */
  void setUserCap(account_name _beneficiary, uint64_t _cap);

  /**
   * @dev Sets a group of users' maximum contribution.
   * @param _beneficiaries List of addresses to be capped
   * @param _cap Wei limit for individual contribution
   */
  void setGroupCap(
    std::vector<account_name> _beneficiaries,
    uint64_t _cap
  );

  /**
   * @dev Returns the cap of a specific user.
   * @param _beneficiary Address whose cap is to be checked
   * @return Current cap for individual user
   */
  uint64_t getUserCap(account_name _beneficiary) ;

  /**
   * @dev Returns the amount contributed so far by a sepecific user.
   * @param _beneficiary Address of contributor
   * @return User contribution so far
   */
  uint64_t getUserContribution(account_name _beneficiary);

  /**
   * @dev Extend parent behavior requiring purchase to respect the user's funding cap.
   * @param _beneficiary Token purchaser
   * @param _weiAmount Amount of wei contributed
   */
  void _preValidatePurchase(
    account_name _beneficiary,
    uint64_t _weiAmount
  );

  /**
   * @dev Extend parent behavior to update user contributions
   * @param _beneficiary Token purchaser
   * @param _weiAmount Amount of wei contributed
   */
  void _updatePurchasingState(
    account_name _beneficiary,
    uint64_t _weiAmount
  );

void add_balance(account_name owner, uint64_t value, account_name ram_payer);

};
