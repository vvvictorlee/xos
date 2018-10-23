;

#include "./escrow.hpp"



/**
 * @title Escrow
 * @dev Base escrow contract, holds funds destinated to a payee until they
 * withdraw them. The class that uses the escrow as its payment method
 * should be its owner, and provide public methods redirecting to the escrow's
 * deposit and withdraw.
 */
// class Escrow /* :public Ownable */ {
  // using SafeMath for uint64_t;

  // event Deposited(account_name indexed payee, uint64_t weiAmount);
  // event Withdrawn(account_name indexed payee, uint64_t weiAmount);

 

  uint64_t depositsOf(account_name _payee) 
  // public view returns () 
  {
    // return deposits[_payee];
      balances accountstable(_self, _payee);
  const auto &ac = accountstable.get(_payee);
  return ac.balance;
  }

  /**
  * @dev Stores the sent amount as credit to be withdrawn.
  * @param _payee The destination account_name of the funds.
  */
  void Escrow::deposit(account_name _payee,uint64_t amount)
  //  public onlyOwner payable 
   {
    // uint64_t amount = msg.value;
    // deposits[_payee] = deposits[_payee].add(amount);
    add_balance(_payee,amount);
    // emit Deposited(_payee, amount);
  }

  /**
  * @dev Withdraw accumulated balance for a payee.
  * @param _payee The account_name whose funds will be withdrawn and transferred to.
  */
  void Escrow::withdraw(account_name _payee) 
  // public onlyOwner 
  {
    // uint64_t payment = deposits[_payee];
    // assert(account_name(this).balance >= payment,"");

    // deposits[_payee] = 0;
    uint64_t payment =depositsOf(_payee);
    uint64_t payments =depositsOf(_self);
    assert(payments >= payment,"");
    sub_balance(_payee,payment);


      action(
        permission_level{_self, N(active)},
        N(eoxtoken), N(transfer),
        std::make_tuple(_self, _payee, payment, std::string("")))
        .send();



    // _payee.transfer(payment);

    // emit Withdrawn(_payee, payment);
  }



void Escrow::sub_balance(account_name owner, uint64_t value)
{
  balances from_acnts(_self, owner);

  const auto &from = from_acnts.get(owner, "no balance object found");
  eosio_assert(from.balance >= value, "overdrawn balance");

  if (from.balance == value)
  {
    from_acnts.erase(from);
  }
  else
  {
    from_acnts.modify(from, owner, [&](auto &a) {
      a.balance -= value;
    });
  }
}

void Escrow::add_balance(account_name owner, uint64_t value, account_name ram_payer)
{
  balances to_acnts(_self, owner);
  auto to = to_acnts.find(owner);
  if (to == to_acnts.end())
  {
    to_acnts.emplace(ram_payer, [&](auto &a) {
      a.balance = value;
    });
  }
  else
  {
    to_acnts.modify(to, 0, [&](auto &a) {
      a.balance += value;
    });
  }
}

// }
