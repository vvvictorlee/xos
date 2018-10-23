// 


#include "../eoxtoken/eoxtoken.hpp"

#include "./crowdsale.hpp"
/**eosiocpp  -g crowdsale.abi crowdsale.cpp
 * @title Crowdsale
 * @dev Crowdsale is a base class for managing a token crowdsale,
 * allowing investors to purchase tokens with ether. This class implements
 * such functionality in its most fundamental form and can be extended to provide additional
 * functionality and/or custom behavior.
 * The external interface represents the basic interface for purchasing tokens, and conform
 * the base architecture for crowdsales. They are *not* intended to be modified / overridden.
 * The internal interface conforms the extensible and modifiable surface of crowdsales. Override
 * the methods to add functionality. Consider using 'super' where appropriate to concatenate
 * behavior.
 */
// class Crowdsale {
//   // using SafeMath for uint64_t;
//   // using SafeERC20 for ERC20;

//   // The token being sold
//   EOXToken  token;

//   // Address where funds are collected
//   account_name  wallet;

//   // How many token units a buyer gets per wei.
//   // The rate is the conversion between wei and the smallest and indivisible token unit.
//   // So, if you are using a rate of 1 with a DetailedERC20 token with 3 decimals called TOK
//   // 1 wei will give you 1 unit, or 0.001 TOK.
//   uint64_t  rate;

//   // Amount of wei raised
//   uint64_t  weiRaised;

  /**
   * Event for token purchase logging
   * @param purchaser who paid for the tokens
   * @param beneficiary who got the tokens
   * @param value weis paid for purchase
   * @param amount amount of tokens purchased
   */
  // event TokenPurchase(
  //   account_name indexed purchaser,
  //   account_name indexed beneficiary,
  //   uint64_t value,
  //   uint64_t amount
  // );

  /**
   * @param _rate Number of token units a buyer gets per wei
   * @param _wallet Address where collected funds will be forwarded to
   * @param _token Address of the token being sold
   */
  Crowdsale::Crowdsale(uint64_t _rate, account_name _wallet, EOXToken* _token):contract(_wallet){
    eosio_assert(_rate > 0,"");
    eosio_assert(_wallet != account_name(0),"");
    // eosio_assert(_token != account_name(0),"");

    rate = _rate;
    wallet = _wallet;
    token = _token;
  }

  // -----------------------------------------
  // Crowdsale external interface
  // -----------------------------------------

  /**
   * @dev fallback function ***DO NOT OVERRIDE***
   */
  // function () external payable {
  //   buyTokens(msg.sender);
  // }

  /**
   * @dev low level token purchase ***DO NOT OVERRIDE***
   * @param _beneficiary Address performing the token purchase
   */
   void Crowdsale::buytokens(account_name _beneficiary,uint64_t value) 
  //  public payable 
   {

    uint64_t weiAmount = value;
    _preValidatePurchase(_beneficiary, weiAmount);

    // calculate token amount to be created
    uint64_t tokens = _getTokenAmount(weiAmount);

    // update state
    weiRaised = weiRaised+weiAmount;

    _processPurchase(_beneficiary, tokens);
    // emit TokenPurchase(
    //   msg.sender,
    //   _beneficiary,
    //   weiAmount,
    //   tokens
    // );

    _updatePurchasingState(_beneficiary, weiAmount);

    _forwardFunds(weiAmount);
    _postValidatePurchase(_beneficiary, weiAmount);
  }

  // -----------------------------------------
  // Internal interface (extensible)
  // -----------------------------------------

  /**
   * @dev Validation of an incoming purchase. Use eosio_assert statements to revert state when conditions are not met. Use `super` in contracts that inherit from Crowdsale to extend their validations.
   * Example from CappedCrowdsale.hpp"'s _preValidatePurchase method: 
   *   super._preValidatePurchase(_beneficiary, _weiAmount);
   *   eosio_assert(weiRaised.add(_weiAmount) <= cap);
   * @param _beneficiary Address performing the token purchase
   * @param _weiAmount Value in wei involved in the purchase
   */
  void Crowdsale::_preValidatePurchase(
    account_name _beneficiary,
    uint64_t _weiAmount
  )
    
  {
    eosio_assert(_beneficiary != account_name(0),"");
    eosio_assert(_weiAmount != 0,"");
  }

  /**
   * @dev Validation of an executed purchase. Observe state and use revert statements to undo rollback when valid conditions are not met.
   * @param _beneficiary Address performing the token purchase
   * @param _weiAmount Value in wei involved in the purchase
   */
  void Crowdsale::_postValidatePurchase(
    account_name _beneficiary,
    uint64_t _weiAmount
  )
    
  {
    // optional override
  }

  /**
   * @dev Source of tokens. Override this method to modify the way in which the crowdsale ultimately gets and sends its tokens.
   * @param _beneficiary Address performing the token purchase
   * @param _tokenAmount Number of tokens to be emitted
   */
  void Crowdsale::_deliverTokens(
    account_name _beneficiary,
    uint64_t _tokenAmount
  )
  {
    token->transfer(_beneficiary, _tokenAmount);
  }

  /**
   * @dev Executed when a purchase has been validated and is ready to be executed. Not necessarily emits/sends tokens.
   * @param _beneficiary Address receiving the tokens
   * @param _tokenAmount Number of tokens to be purchased
   */
  void Crowdsale::_processPurchase(
    account_name _beneficiary,
    uint64_t _tokenAmount
  )
    
  {
    _deliverTokens(_beneficiary, _tokenAmount);
  }

  /**
   * @dev Override for extensions that eosio_assert an internal state to check for validity (current user contributions, etc.)
   * @param _beneficiary Address receiving the tokens
   * @param _weiAmount Value in wei involved in the purchase
   */
  void Crowdsale::_updatePurchasingState(
    account_name _beneficiary,
    uint64_t _weiAmount
  )
    
  {
    // optional override
  }

  /**
   * @dev Override to extend the way in which ether is converted to tokens.
   * @param _weiAmount Value in wei to be converted into tokens
   * @return Number of tokens that can be purchased with the specified _weiAmount
   */
  uint64_t Crowdsale::_getTokenAmount(uint64_t _weiAmount)
    // internal view returns (uint64_t)
  {
    return _weiAmount*rate;
  }

  /**
   * @dev Determines how ETH is stored/forwarded on purchases.
   */
  void Crowdsale::_forwardFunds(uint64_t amount)  {
    // wallet.transfer(msg.value);
    token->transfer(wallet,amount);
  }
// }
EOSIO_ABI(Crowdsale, (buytokens))
