// 
#include <eosiolib/eosio.hpp>
#include "./increasingpricecrowdsale.hpp"


/**eosiocpp  -g increasingpricecrowdsale.abi increasingpricecrowdsale.cpp
 * @title IncreasingPriceCrowdsale
 * @dev Extension of Crowdsale class that increases the price of tokens linearly in time.
 * Note that what should be provided to the constructor is the initial and final _rates_, that is,
 * the amount of tokens per wei contributed. Thus, the initial rate must be greater than the final rate.
 */
// class IncreasingPriceCrowdsale : public TimedCrowdsale {
//   // using SafeMath for uint64_t;

//   uint64_t  initialRate;
//   uint64_t  finalRate;

  /**
   * @dev Constructor, takes initial and final rates of tokens received per wei contributed.
   * @param _initialRate Number of tokens a buyer gets per wei at the start of the crowdsale
   * @param _finalRate Number of tokens a buyer gets per wei at the end of the crowdsale
   */
  IncreasingPriceCrowdsale::IncreasingPriceCrowdsale(uint64_t _initialRate, uint64_t _finalRate):TimedCrowdsale(0,0) {
    eosio_assert(_initialRate >= _finalRate,"");
    eosio_assert(_finalRate > 0,"");
    initialRate = _initialRate;
    finalRate = _finalRate;
  }

  /**
   * @dev Returns the rate of tokens per wei at the present time.
   * Note that, as price _increases_ with time, the rate _decreases_.
   * @return The number of tokens a buyer gets per wei at a given time
   */
  uint64_t IncreasingPriceCrowdsale::getCurrentRate() 
  // public view returns (uint64_t) 
  {
    // solium-disable-next-line security/no-block-members
    uint64_t elapsedTime = now()-(openingTime);
    uint64_t timeRange = closingTime-(openingTime);
    uint64_t rateRange = initialRate-(finalRate);
    // return initialRate.sub(elapsedTime.mul(rateRange).div(timeRange));
    return initialRate-(elapsedTime*(rateRange)/(timeRange));
  }

  /**
   * @dev Overrides parent method taking into account variable rate.
   * @param _weiAmount The value in wei to be converted into tokens
   * @return The number of tokens _weiAmount wei will buy at present time
   */
  uint64_t IncreasingPriceCrowdsale::_getTokenAmount(uint64_t _weiAmount)
    // internal view returns (uint64_t)
  {
    uint64_t currentRate = getCurrentRate();
    return currentRate*(_weiAmount);
  }

// }
