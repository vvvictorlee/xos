// 
#pragma once


#include "./finalizablecrowdsale.hpp"
#include "../eoxpayment/refundescrow.hpp"
#include "./refundablecrowdsale.hpp"
using namespace eosio;
/**eosiocpp  -g refundablecrowdsale.abi refundablecrowdsale.cpp
 * @title RefundableCrowdsale
 * @dev Extension of Crowdsale class that adds a funding goal, and
 * the possibility of users getting a refund if goal is not met.
 */
// class RefundableCrowdsale : public FinalizableCrowdsale {
//   // using SafeMath for _uint64_t;
// public:
//   // minimum amount of funds to be raised in weis
//   uint64_t  goal;

//   // refund escrow used to hold funds while crowdsale is running
//   RefundEscrow  escrow;

  /**
   * @dev Constructor, creates RefundEscrow.
   * @param _goal Funding goal
   */
  RefundableCrowdsale::RefundableCrowdsale(uint64_t _goal):contract(_goal)  {
    eosio_assert(_goal > 0,"");
    escrow = new RefundEscrow(wallet);
    goal = _goal;
  }

  /**
   * @dev Investors can claim refunds here if crowdsale is unsuccessful
   */
  void RefundableCrowdsale::claimRefund()  {
    eosio_assert(isFinalized,"");
    eosio_assert(!goalReached(),"");

    escrow->withdraw(_self);
  }

  /**
   * @dev Checks whether funding goal was reached.
   * @return Whether funding goal was reached
   */
  bool RefundableCrowdsale::goalReached() 
  // public view returns (bool) 
  {
    return weiRaised >= goal;
  }

  /**
   * @dev escrow finalization task, called when owner calls finalize()
   */
  void RefundableCrowdsale::finalization()  {
    if (goalReached()) {
      escrow->close();
      escrow->bwithdraw();
    } else {
      escrow->enrefund();
    }

    FinalizableCrowdsale::finalization();
  }

  /**
   * @dev Overrides Crowdsale fund forwarding, sending funds to escrow.
   */
void RefundableCrowdsale::_forwardFunds(uint64_t amount)  {
    escrow->deposit(_self,amount);
  }

// };
