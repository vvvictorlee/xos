
#include <eosiolib/transaction.hpp>
#include <eosiolib/dispatcher.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/system.h>
#include "./conditionalescrow.hpp"
#include "./refundescrow.hpp"

using namespace eosio;
/**
 * @title RefundEscrow
 * @dev Escrow that holds funds for a beneficiary, deposited from multiple parties.
 * The class owner may close the deposit period, and allow for either withdrawal
 * by the beneficiary, or refunds to the depositors.
 */
// class RefundEscrow :public ConditionalEscrow {
//   public:
//   enum State { Active, Refunding, Closed }

  // event Closed();
  // event RefundsEnabled();

  // State  state;
  // account_name  beneficiary;

  /**
   * @dev Constructor.
   * @param _beneficiary The beneficiary of the deposits.
   */
  RefundEscrow::RefundEscrow(account_name self):contract(self)  {
    account_name _beneficiary = self;
    eosio_assert(_beneficiary != account_name(0),"");
    beneficiary = _beneficiary;
    state = State::Active;
  }

  /**
   * @dev Stores funds that may later be refunded.
   * @param _refundee The account_name funds will be sent to if a refund occurs.
   */
  void RefundEscrow::depositr(account_name _refundee,uint64_t amount) 
  // public payable 
  {
    eosio_assert(state == State::Active,"");
    Escrow::deposit(_refundee,amount);
  }

  /**
   * @dev Allows for the beneficiary to withdraw their funds, rejecting
   * further deposits.
   */
  void RefundEscrow::close()
  //  public onlyOwner 
   {
    eosio_assert(state == State::Active,"");
    state = State::Closed;
    // emit Closed();
  }

  /**
   * @dev Allows for refunds to take place, rejecting further deposits.
   */
  void RefundEscrow::enrefund()
  //  public onlyOwner 
  {
    eosio_assert(state == State::Active,"");
    state = State::Refunding;
    // emit RefundsEnabled();
  }

  /**
   * @dev Withdraws the beneficiary's funds.
   */
  void RefundEscrow::bwithdraw() 
  // public 
  {
    eosio_assert(state == State::Closed,"");
    uint64_t payment = depositsOf(_self);
    // beneficiary.transfer(account_name(this).balance);
     action(
        permission_level{_self, N(active)},
        N(eoxtoken), N(transfer),
        std::make_tuple(_self, beneficiary, payment, std::string("")))
        .send();
  }

  /**
   * @dev Returns whether refundees can withdraw their deposits (be refunded).
   */
  bool RefundEscrow::withdrawalAllowed(account_name _payee) 
  // public view returns (bool) 
  {
    return state == State::Refunding;
  }
// }
EOSIO_ABI(RefundEscrow, (depositr)(close)(enrefund)(bwithdraw))