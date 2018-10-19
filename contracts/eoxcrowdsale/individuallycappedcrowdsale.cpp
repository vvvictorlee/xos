// 


#include "./individuallycappedcrowdsale.hpp"



/**eosiocpp  -g individuallycappedcrowdsale.abi individuallycappedcrowdsale.cpp
 * @title IndividuallyCappedCrowdsale
 * @dev Crowdsale with per-user caps.
 */
// class IndividuallyCappedCrowdsale :public Crowdsale, Ownable {
//   // using SafeMath for uint64_t;

//   // mapping(account_name => uint64_t) public contributions;
//   // mapping(account_name => uint64_t) public caps;
//  struct balance
//   {
//     account_name name;
//     uint64_t balance;

//     uint64_t primary_key() const { return name; }
//   };

//   typedef eosio::multi_index<N(balances), balance> balances;

//    struct balance
//   {
//     account_name name;
//     uint64_t balance;

//     uint64_t primary_key() const { return name; }
//   };

//   typedef eosio::multi_index<N(balances), balance> balances;
  /**
   * @dev Sets a specific user's maximum contribution.
   * @param _beneficiary Address to be capped
   * @param _cap Wei limit for individual contribution
   */
  void IndividuallyCappedCrowdsale::setUserCap(account_name _beneficiary, uint64_t _cap) 
  // external onlyOwner 
  {
    // caps[_beneficiary] = _cap;
    caps from_acnts(_self, _beneficiary);
      auto iid = from_acnts.find(_beneficiary);
      from_acnts.modify(iid, _self, [&](auto &a) {
        a.balance = _cap;
      });
  }

  /**
   * @dev Sets a group of users' maximum contribution.
   * @param _beneficiaries List of addresses to be capped
   * @param _cap Wei limit for individual contribution
   */
  void IndividuallyCappedCrowdsale::setGroupCap(
    std::vector<account_name> _beneficiaries,
    uint64_t _cap
  )
    // external
    // onlyOwner
  {
    for (uint64_t i = 0; i < _beneficiaries.size(); i++) {
      setUserCap(_beneficiaries[i],_cap);
      // caps[_beneficiaries[i]] = _cap;
    }
  }

  /**
   * @dev Returns the cap of a specific user.
   * @param _beneficiary Address whose cap is to be checked
   * @return Current cap for individual user
   */
  uint64_t IndividuallyCappedCrowdsale::getUserCap(account_name _beneficiary) 
  // public view returns (uint64_t) 
  {
     caps accountstable(_self, _beneficiary);
  const auto &ac = accountstable.get(_beneficiary);
  return ac.balance;

    // return caps[_beneficiary];
  }

  /**
   * @dev Returns the amount contributed so far by a sepecific user.
   * @param _beneficiary Address of contributor
   * @return User contribution so far
   */
  uint64_t IndividuallyCappedCrowdsale::getUserContribution(account_name _beneficiary)
    // public view returns (uint64_t)
  {
    contributions accountstable(_self, _beneficiary);
  const auto &ac = accountstable.get(_beneficiary);
  return ac.balance;
    // return contributions[_beneficiary];
  }

  /**
   * @dev Extend parent behavior requiring purchase to respect the user's funding cap.
   * @param _beneficiary Token purchaser
   * @param _weiAmount Amount of wei contributed
   */
  void IndividuallyCappedCrowdsale::_preValidatePurchase(
    account_name _beneficiary,
    uint64_t _weiAmount
  )
    // internal
  {
    Crowdsale::_preValidatePurchase(_beneficiary, _weiAmount);
    eosio_assert(getUserContribution(_beneficiary)+(_weiAmount) <= getUserCap(_beneficiary),"");
  }

  /**
   * @dev Extend parent behavior to update user contributions
   * @param _beneficiary Token purchaser
   * @param _weiAmount Amount of wei contributed
   */
  void IndividuallyCappedCrowdsale::_updatePurchasingState(
    account_name _beneficiary,
    uint64_t _weiAmount
  )
    // internal
  {
    Crowdsale::_updatePurchasingState(_beneficiary, _weiAmount);
    // contributions[_beneficiary] = contributions[_beneficiary].add(_weiAmount);

    add_balance(_beneficiary,_weiAmount,_beneficiary);
  }

void IndividuallyCappedCrowdsale::add_balance(account_name owner, uint64_t value, account_name ram_payer)
{
  contributions to_acnts(_self, owner);
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
