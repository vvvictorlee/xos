// 

#include <eosiolib/eosio.hpp>
#include "./crowdsale.hpp"
#include "./timecrowdsale.hpp"

/**eosiocpp  -g timecrowdsale.abi timecrowdsale.cpp
 * @title TimedCrowdsale
 * @dev Crowdsale accepting contributions only within a time frame.
 */
// class TimedCrowdsale : public Crowdsale {
//   // using SafeMath for uint64_t;
// public:
//   uint64_t  openingTime;
//   uint64_t  closingTime;

  // /**
  //  * @dev Reverts if not in crowdsale time range.
  //  */
  // modifier onlyWhileOpen {
  //   // solium-disable-next-line security/no-block-members
  //   eosio_assert(block.timestamp >= openingTime && block.timestamp <= closingTime);
  //   _;
  // }

  /**
   * @dev Constructor, takes crowdsale opening and closing times.
   * @param _openingTime Crowdsale opening time
   * @param _closingTime Crowdsale closing time
   */
  TimedCrowdsale::TimedCrowdsale(uint64_t _openingTime, uint64_t _closingTime): Crowdsale(0,0,0) {
    // solium-disable-next-line security/no-block-members
    eosio_assert(_openingTime >= now(),"");
    eosio_assert(_closingTime >= now()+10000,"");

    openingTime = _openingTime;
    closingTime = _closingTime;
  }

  /**
   * @dev Checks whether the period in which the crowdsale is open has already elapsed.
   * @return Whether crowdsale period has elapsed
   */
  bool TimedCrowdsale::hasClosed()
  //  public view returns (bool) 
  {
    // solium-disable-next-line security/no-block-members
    return now() > closingTime;
  }

  /**
   * @dev Extend parent behavior requiring to be within contributing period
   * @param _beneficiary Token purchaser
   * @param _weiAmount Amount of wei contributed
   */
  void TimedCrowdsale::_preValidatePurchase(
    account_name _beneficiary,
    uint64_t _weiAmount
  )
    // internal
    // onlyWhileOpen
  {
    Crowdsale::_preValidatePurchase(_beneficiary, _weiAmount);
  }

// }
