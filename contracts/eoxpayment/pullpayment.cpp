// 

#include "./escrow.hpp"
#include "./pullpayment.hpp"

/**
 * @title PullPayment
 * @dev Base class supporting async send for pull payments. Inherit from this
 * class and use asyncTransfer instead of send or transfer.
 */
// class PullPayment {
//   Escrow  escrow;

// // public:
//   PullPayment::PullPayment()  {
//     escrow = new Escrow();
//   }

  /**
  * @dev Withdraw accumulated balance, called by payee.
  */
  void PullPayment::withdrawpay()  {
    account_name payee = _self;
    escrow.withdraw(payee);
  }

  /**
  * @dev Returns the credit owed to an account_name.
  * @param _dest The creditor's account_name.
  */
  uint64_t PullPayment::payments(account_name _dest) 
  // public view returns (uint64_t)
   {
     return escrow.depositsOf(_dest);
  }

  /**
  * @dev Called by the payer to store the sent amount as credit to be pulled.
  * @param _dest The destination account_name of the funds.
  * @param _amount The amount to transfer.
  */
  void PullPayment::ptransfer(account_name _dest, uint64_t _amount)  {
    escrow.deposit(_dest,_amount);
  }
// }
EOSIO_ABI(PullPayment, (ptransfer)(withdrawpay))
