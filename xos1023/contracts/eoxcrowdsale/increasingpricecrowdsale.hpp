// 
#pragma once
#include "./timecrowdsale.hpp"



/**
 * @title IncreasingPriceCrowdsale
 * @dev Extension of Crowdsale class that increases the price of tokens linearly in time.
 * Note that what should be provided to the constructor is the initial and final _rates_, that is,
 * the amount of tokens per wei contributed. Thus, the initial rate must be greater than the final rate.
 */
class IncreasingPriceCrowdsale : public TimedCrowdsale {
  // using SafeMath for uint64_t;
public:
  uint64_t  initialRate;
  uint64_t  finalRate;

  /**
   * @dev Constructor, takes initial and final rates of tokens received per wei contributed.
   * @param _initialRate Number of tokens a buyer gets per wei at the start of the crowdsale
   * @param _finalRate Number of tokens a buyer gets per wei at the end of the crowdsale
   */
  IncreasingPriceCrowdsale(uint64_t _initialRate, uint64_t _finalRate) ;

  /**
   * @dev Returns the rate of tokens per wei at the present time.
   * Note that, as price _increases_ with time, the rate _decreases_.
   * @return The number of tokens a buyer gets per wei at a given time
   */
  uint64_t getCurrentRate();

  /**
   * @dev Overrides parent method taking into account variable rate.
   * @param _weiAmount The value in wei to be converted into tokens
   * @return The number of tokens _weiAmount wei will buy at present time
   */
  uint64_t _getTokenAmount(uint64_t _weiAmount);

};
