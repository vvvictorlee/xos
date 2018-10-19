// 

#pragma once

#include "./timecrowdsale.hpp"


/**
 * @title FinalizableCrowdsale
 * @dev Extension of Crowdsale where an owner can do extra work
 * after finishing.
 */
class FinalizableCrowdsale :public TimedCrowdsale {
  // using SafeMath for uint256_t;
public:
  bool  isFinalized = false;

  // event Finalized();
FinalizableCrowdsale():TimedCrowdsale(0,1){}
  /**
   * @dev Must be called after crowdsale ends, to do some extra finalization
   * work. Calls the contract's finalization function.
   */
  void finalize();

  /**
   * @dev Can be overridden to add finalization logic. The overriding function
   * should call super.finalization() to ensure the chain of finalization is
   * executed entirely.
   */
  void finalization();

};
