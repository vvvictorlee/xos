#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>

// #include "./ERC20Basic.hpp"
// #include "../../math/SafeMath.hpp";
// #include "../../math/uint64_t.h";
using namespace eosio;
// typedef uint64_t uint64_t

// 如果接收方是智能合约，则发送方必须通过“approve（）”和“transferFrom（）”两个函数的组合使用机制来转移通证，而如果接收方是外部拥有账户，则发送方必须通过“transfer（）”函数转移通证。
// 如果用户由于失误选择了错误的函数，那么通证将卡在智能合约内（智能合约将无法识别交易），而且并没有可行的方法来提取被卡住的通证。

/**
 * @title Basic token
 * @dev Basic version of StandardToken, with no allowances.
 */
class EOXToken : public contract
{
  public:
   EOXToken( account_name self ):contract(self){}
  // using SafeMath for uint64_t;
  struct balance
  {
    account_name name;
    uint64_t balance;

    uint64_t primary_key() const { return name; }
  };

  typedef eosio::multi_index<N(balances), balance> balances;

  // mapping(account_name => uint64_t) internal balances;

  uint64_t totalSupply_;

  /**
  * @dev Total number of tokens in existence
  */
  inline uint64_t totalSupply();

  /**
  * @dev Transfer token for a specified account_name
  * @param _to The account_name to transfer to.
  * @param _value The amount to be transferred.
  */
  void transfer(account_name _to, uint64_t _value);


void sub_balance( account_name owner, uint64_t value );

void add_balance( account_name owner, uint64_t value, account_name ram_payer );

void sub_abalance(account_name from,account_name sender, uint64_t value);
void add_abalance(account_name from,account_name sender, uint64_t value);

  /**
  * @dev Gets the balance of the specified account_name.
  * @param _owner The account_name to query the the balance of.
  * @return An uint64_t representing the amount owned by the passed account_name.
  */
  uint64_t balanceOf(account_name _owner);


  struct allowedx {
    uint64_t    id;
    account_name name;
     account_name sname;
            uint64_t    balance;
            uint64_t primary_key()const { return id; }
            uint64_t by_name() const { return name; }
            
         };

        typedef eosio::multi_index< N(allowed), allowedx,indexed_by< N(name), const_mem_fun<allowedx, uint64_t, &allowedx::by_name > > >  allowed;

  /**
   * @dev Transfer tokens from one account_name to another
   * @param _from account_name The account_name which you want to send tokens from
   * @param _to account_name The account_name which you want to transfer to
   * @param _value uint64_t the amount of tokens to be transferred
   */
  /// @abi action
  void transferfrom(
    account_name _from,
    account_name _to,
    uint64_t _value
  );

  /**
   * @dev Approve the passed account_name to spend the specified amount of tokens on behalf of msg.sender.
   * Beware that changing an allowance with this method brings the risk that someone may use both the old
   * and the new allowance by unfortunate transaction ordering. One possible solution to mitigate this
   * race condition is to first reduce the spender's allowance to 0 and set the desired value afterwards:
   * https://github.com/ethereum/EIPs/issues/20#issuecomment-263524729
   * @param _spender The account_name which will spend the funds.
   * @param _value The amount of tokens to be spent.
   */
  /// @abi action
  void approve(account_name _spender, uint64_t _value);

// 6. 限额(allowance)
// 为了准确地执行交易，智能合约需要知道的一个最重要数据就是用户的余额。毕竟，用户至少应该拥有与交易金额相等的通证才能进行交易。
// 这就是为什么ERC-20智能合约还包括包含allowance（）函数的原因。 如果用户没有所需的最小通证数，则allowance（）函数会取消该交易。
  /**
   * @dev Function to check the amount of tokens that an owner allowed to a spender.
   * @param _owner account_name The account_name which owns the funds.
   * @param _spender account_name The account_name which will spend the funds.
   * @return A uint64_t specifying the amount of tokens still available for the spender.
   */
  uint64_t allowance(
    account_name _owner,
    account_name _spender
   );
  

  /**
   * @dev Increase the amount of tokens that an owner allowed to a spender.
   * approve should be called when allowed[_spender] == 0. To increment
   * allowed value is better to use this function to avoid 2 calls (and wait until
   * the first transaction is mined)
   * From MonolithDAO Token.hpp
   * @param _spender The account_name which will spend the funds.
   * @param _addedValue The amount of tokens to increase the allowance by.
   */
  /// @abi action
  void incapproval(
    account_name _spender,
    uint64_t _addedValue
  );
  

  /**
   * @dev Decrease the amount of tokens that an owner allowed to a spender.
   * approve should be called when allowed[_spender] == 0. To decrement
   * allowed value is better to use this function to avoid 2 calls (and wait until
   * the first transaction is mined)
   * From MonolithDAO Token.hpp
   * @param _spender The account_name which will spend the funds.
   * @param _subtractedValue The amount of tokens to decrease the allowance by.
   */
  /// @abi action
  void decapproval(account_name _spender,uint64_t _subtractedValue);
 


};
 uint64_t EOXToken::totalSupply()
  {
    return totalSupply_;
  }