// 
#pragma once

#include <eosiolib/eosio.hpp>
#include "./finalizablecrowdsale.hpp"
#include "../eoxpayment/refundescrow.hpp"


/**
 * @title RefundableCrowdsale
 * @dev Extension of Crowdsale class that adds a funding goal, and
 * the possibility of users getting a refund if goal is not met.
 */
class RefundableCrowdsale : public FinalizableCrowdsale,public eosio::contract {
  // using SafeMath for _uint64_t;
public:
  // minimum amount of funds to be raised in weis
  uint64_t  goal;

  // refund escrow used to hold funds while crowdsale is running
  RefundEscrow*  escrow;

  /**
   * @dev Constructor, creates RefundEscrow.
   * @param _goal Funding goal
   */
  RefundableCrowdsale(uint64_t _goal);

  /**
   * @dev Investors can claim refunds here if crowdsale is unsuccessful
   */
  void claimRefund();

  /**
   * @dev Checks whether funding goal was reached.
   * @return Whether funding goal was reached
   */
  bool goalReached();

  /**
   * @dev escrow finalization task, called when owner calls finalize()
   */
  void finalization();

  /**
   * @dev Overrides Crowdsale fund forwarding, sending funds to escrow.
   */
void _forwardFunds(uint64_t amount);

};
