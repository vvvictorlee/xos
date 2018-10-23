
#pragma once
#include "./escrow.hpp"


/**
 * @title ConditionalEscrow
 * @dev Base abstract escrow to only allow withdrawal if a condition is met.
 */
class ConditionalEscrow :public Escrow {
  /**
  * @dev Returns whether an account_name is allowed to withdraw their funds. To be
  * implemented by derived contracts.
  * @param _payee The destination account_name of the funds.
  */
 public:
  bool withdrawalAllowed(account_name _payee);
  //  public view returns (bool);

   void withdraw(account_name _payee) ;
};
