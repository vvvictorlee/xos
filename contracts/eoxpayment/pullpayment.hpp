// 
#include <eosiolib/eosio.hpp>
#include "./escrow.hpp"


/**
 * @title PullPayment
 * @dev Base class supporting async send for pull payments. Inherit from this
 * class and use asyncTransfer instead of send or transfer.
 */
class PullPayment:public eosio::contract {
  Escrow  escrow;

public:
  PullPayment( account_name self ):contract(self){}

  /**
  * @dev Withdraw accumulated balance, called by payee.
  */
 /// @abi action
  void withdrawpay();

  /**
  * @dev Returns the credit owed to an account_name.
  * @param _dest The creditor's account_name.
  */
  uint64_t payments(account_name _dest);

  /**
  * @dev Called by the payer to store the sent amount as credit to be pulled.
  * @param _dest The destination account_name of the funds.
  * @param _amount The amount to transfer.
  */
 /// @abi action
  void ptransfer(account_name _dest, uint64_t _amount);
};
