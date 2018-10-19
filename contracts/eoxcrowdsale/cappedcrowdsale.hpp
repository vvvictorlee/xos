// 
#pragma once

#include "./crowdsale.hpp"


/**eosiocpp  -g refundablecrowdsale.abi refundablecrowdsale.cpp
 * @title CappedCrowdsale
 * @dev Crowdsale with a limit for total contributions.
 */
class CappedCrowdsale : public Crowdsale {
  // using SafeMath for uint64_t;

  uint64_t  cap;

  /**
   * @dev Constructor, takes maximum amount of wei accepted in the crowdsale.
   * @param _cap Max amount of wei to be contributed
   */
  CappedCrowdsale(uint64_t _cap);

  /**
   * @dev Checks whether the cap has been reached.
   * @return Whether the cap was reached
   */
  bool capReached();

  /**
   * @dev Extend parent behavior requiring purchase to respect the funding cap.
   * @param _beneficiary Token purchaser
   * @param _weiAmount Amount of wei contributed
   */
  void _preValidatePurchase(
    account_name _beneficiary,
    uint64_t _weiAmount
  );

};
