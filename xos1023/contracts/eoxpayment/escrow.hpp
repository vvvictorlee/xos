// pragma solidity ^0.4.23;
#pragma once
// #include "../math/SafeMath.hpp"
// #include "../ownership/Ownable.hpp"
#include <eosiolib/eosio.hpp>

/**
 * @title Escrow
 * @dev Base escrow contract, holds funds destinated to a payee until they
 * withdraw them. The class that uses the escrow as its payment method
 * should be its owner, and provide public methods redirecting to the escrow's
 * deposit and withdraw.
 */
class Escrow /* :public Ownable */ {
  public:
  // using SafeMath for uint64_t;

  // event Deposited(account_name indexed payee, uint64_t weiAmount);
  // event Withdrawn(account_name indexed payee, uint64_t weiAmount);

  // mapping(account_name => uint64_t) private deposits;
 struct balance
  {
    account_name name;
    uint64_t balance;

    uint64_t primary_key() const { return name; }
  };

  typedef eosio::multi_index<N(balances), balance> balances;

  uint64_t depositsOf(account_name _payee) ;

  /**
  * @dev Stores the sent amount as credit to be withdrawn.
  * @param _payee The destination account_name of the funds.
  */
  void deposit(account_name _payee,uint64_t amount);
  /**
  * @dev Withdraw accumulated balance for a payee.
  * @param _payee The account_name whose funds will be withdrawn and transferred to.
  */
  void withdraw(account_name _payee) ;
void sub_balance(account_name owner, uint64_t value);

void add_balance(account_name owner, uint64_t value, account_name ram_payer);


};
