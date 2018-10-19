#pragma once
#include <eosiolib/eosio.hpp>
#include "./conditionalescrow.hpp"
// #include "../ownership/Ownable.hpp"


/**
 * @title RefundEscrow
 * @dev Escrow that holds funds for a beneficiary, deposited from multiple parties.
 * The class owner may close the deposit period, and allow for either withdrawal
 * by the beneficiary, or refunds to the depositors.
 */
class RefundEscrow :public ConditionalEscrow,public eosio::contract {
  public:
  enum State { Active, Refunding, Closed };

  // event Closed();
  // event RefundsEnabled();

  State  state;
  account_name  beneficiary;

  /**
   * @dev Constructor.
   * @param _beneficiary The beneficiary of the deposits.
   */
  RefundEscrow(account_name _beneficiary);

  /**
   * @dev Stores funds that may later be refunded.
   * @param _refundee The account_name funds will be sent to if a refund occurs.
   */
  void depositr(account_name _refundee,uint64_t amount);

  /**
   * @dev Allows for the beneficiary to withdraw their funds, rejecting
   * further deposits.
   */
  void close();

  /**
   * @dev Allows for refunds to take place, rejecting further deposits.
   */
  void enrefund();

  /**
   * @dev Withdraws the beneficiary's funds.
   */
  void bwithdraw();

  /**
   * @dev Returns whether refundees can withdraw their deposits (be refunded).
   */
  bool withdrawalAllowed(account_name _payee);
};
