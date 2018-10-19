// 
#pragma once
#include <eosiolib/eosio.hpp>
#include "../eoxtoken/eoxtoken.hpp"

/**
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
class Crowdsale:public eosio::contract{
  // using SafeMath for uint64_t;
  // using SafeERC20 for ERC20;
  public:
  Crowdsale( account_name self ):contract(self){}

 
  // The token being sold
  EOXToken*  token=nullptr;

  // Address where funds are collected
  account_name  wallet;

  // How many token units a buyer gets per wei.
  // The rate is the conversion between wei and the smallest and indivisible token unit.
  // So, if you are using a rate of 1 with a DetailedERC20 token with 3 decimals called TOK
  // 1 wei will give you 1 unit, or 0.001 TOK.
  uint64_t  rate;

  // Amount of wei raised
  uint64_t  weiRaised;

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
  Crowdsale(uint64_t _rate, account_name _wallet, EOXToken* _token);

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
  /// @abi action
   void buytokens(account_name _beneficiary,uint64_t value);

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
  void _preValidatePurchase(
    account_name _beneficiary,
    uint64_t _weiAmount
  );

  /**
   * @dev Validation of an executed purchase. Observe state and use revert statements to undo rollback when valid conditions are not met.
   * @param _beneficiary Address performing the token purchase
   * @param _weiAmount Value in wei involved in the purchase
   */
  void _postValidatePurchase(
    account_name _beneficiary,
    uint64_t _weiAmount
  );

  /**
   * @dev Source of tokens. Override this method to modify the way in which the crowdsale ultimately gets and sends its tokens.
   * @param _beneficiary Address performing the token purchase
   * @param _tokenAmount Number of tokens to be emitted
   */
  void _deliverTokens(
    account_name _beneficiary,
    uint64_t _tokenAmount
  );

  /**
   * @dev Executed when a purchase has been validated and is ready to be executed. Not necessarily emits/sends tokens.
   * @param _beneficiary Address receiving the tokens
   * @param _tokenAmount Number of tokens to be purchased
   */
  void _processPurchase(
    account_name _beneficiary,
    uint64_t _tokenAmount
  );

  /**
   * @dev Override for extensions that eosio_assert an internal state to check for validity (current user contributions, etc.)
   * @param _beneficiary Address receiving the tokens
   * @param _weiAmount Value in wei involved in the purchase
   */
  void _updatePurchasingState(
    account_name _beneficiary,
    uint64_t _weiAmount
  );

  /**
   * @dev Override to extend the way in which ether is converted to tokens.
   * @param _weiAmount Value in wei to be converted into tokens
   * @return Number of tokens that can be purchased with the specified _weiAmount
   */
  uint64_t _getTokenAmount(uint64_t _weiAmount);

  /**
   * @dev Determines how ETH is stored/forwarded on purchases.
   */
  void _forwardFunds(uint64_t amount);
};
