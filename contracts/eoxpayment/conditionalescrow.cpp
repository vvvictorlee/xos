// pragma solidity ^0.4.23;


#include "./conditionalescrow.hpp"

/**
 * @title ConditionalEscrow
 * @dev Base abstract escrow to only allow withdrawal if a condition is met.
 */
// class ConditionalEscrow :public Escrow {
  /**
  * @dev Returns whether an account_name is allowed to withdraw their funds. To be
  * implemented by derived contracts.
  * @param _payee The destination account_name of the funds.
  */
  // bool withdrawalAllowed(account_name _payee);
  //  public view returns (bool);

   void ConditionalEscrow::withdraw(account_name _payee) 
   {
    eosio_assert(withdrawalAllowed(_payee),"");
    super::withdraw(_payee);
  }
// }
