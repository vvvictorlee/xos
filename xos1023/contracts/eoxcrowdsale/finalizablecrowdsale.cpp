// 



#include "./finalizablecrowdsale.hpp"

/**eosiocpp  -g finalizablecrowdsale.abi finalizablecrowdsale.cpp
 * @title FinalizableCrowdsale
 * @dev Extension of Crowdsale where an owner can do extra work
 * after finishing.
 */
// class FinalizableCrowdsale :public TimedCrowdsale, public Ownable {
//   // using SafeMath for uint256_t;

//   bool  isFinalized = false;

  // event Finalized();

  /**
   * @dev Must be called after crowdsale ends, to do some extra finalization
   * work. Calls the contract's finalization function.
   */
  void FinalizableCrowdsale::finalize()
  //  public onlyOwner 
   {
    eosio_assert(!isFinalized,"");
    eosio_assert(hasClosed(),"");

    finalization();
    // emit Finalized();

    isFinalized = true;
  }

  /**
   * @dev Can be overridden to add finalization logic. The overriding function
   * should call super.finalization() to ensure the chain of finalization is
   * executed entirely.
   */
  void FinalizableCrowdsale::finalization()  {
  }

// }
