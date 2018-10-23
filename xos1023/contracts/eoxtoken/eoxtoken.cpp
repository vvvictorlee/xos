// pragma solidity ^0.4.24;
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>

// #include "./ERC20Basic.hpp"
#include "./eoxtoken.hpp"
// #include "../../math/SafeMath.hpp";
// #include "../../math/uint64_t.h";
using namespace eosio;


// 如果接收方是智能合约，则发送方必须通过“approve（）”和“transferFrom（）”两个函数的组合使用机制来转移通证，而如果接收方是外部拥有账户，则发送方必须通过“transfer（）”函数转移通证。
// 如果用户由于失误选择了错误的函数，那么通证将卡在智能合约内（智能合约将无法识别交易），而且并没有可行的方法来提取被卡住的通证。

/**
  * @dev Transfer token for a specified account_name
  * @param _to The account_name to transfer to.
  * @param _value The amount to be transferred.
  */
void EOXToken::transfer(account_name _to, uint64_t _value)
{
  eosio_assert(_value <= balanceOf(_self), "");
  eosio_assert(_to != account_name(), "");

  // balances[msgsender] = balances[msgsender].sub(_value);
  // balances[_to] = balances[_to].add(_value);
  // emit Transfer(msg.sender, _to, _value);
  sub_balance(_self, _value);
  add_balance(_self, _value, _to);
  // return true;
}

void EOXToken::sub_balance(account_name owner, uint64_t value)
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

void EOXToken::add_balance(account_name owner, uint64_t value, account_name ram_payer)
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

/**
  * @dev Gets the balance of the specified account_name.
  * @param _owner The account_name to query the the balance of.
  * @return An uint64_t representing the amount owned by the passed account_name.
  */
uint64_t EOXToken::balanceOf(account_name _owner)
{
  balances accountstable(_self, _owner);
  const auto &ac = accountstable.get(_owner);
  return ac.balance;
  // return balances[_owner];
}

/**
   * @dev Transfer tokens from one account_name to another
   * @param _from account_name The account_name which you want to send tokens from
   * @param _to account_name The account_name which you want to transfer to
   * @param _value uint64_t the amount of tokens to be transferred
   */
void EOXToken::transferfrom(
    account_name _from,
    account_name _to,
    uint64_t _value)
{
  uint64_t v = allowance(_from, _self);
  eosio_assert(_value <= balanceOf(_from), "");
  eosio_assert(_value <= v, "");
  eosio_assert(_to != account_name(), "");
  sub_balance(_from, _value);
  add_balance(_from, _value, _to);
  // balances[_from] = balances[_from].sub(_value);
  // balances[_to] = balances[_to].add(_value);
  // allowed[_from][msg.sender] = allowed[_from][msg.sender].sub(_value);
  sub_abalance(_from, _self, _value);

  // emit Transfer(_from, _to, _value);
  // return true;
}

void EOXToken::sub_abalance(account_name from, account_name sender, uint64_t value)
{
  allowed from_acnts(from, sender);

  auto pridx = from_acnts.get_index<N(name)>();
  auto itr = pridx.lower_bound(from);
  auto itru = pridx.upper_bound(from);

  for (; itr != pridx.end(); ++itr)
  {
    if (itr->name == from &&itr->sname == sender)
    {
      auto iid = from_acnts.find(itr->id);

      if (itr->balance == value)
      {

        from_acnts.erase(iid);
      }
      else
      {
        from_acnts.modify(iid, from, [&](auto &a) {
          a.balance -= value;
        });
      }
      break;
    }
    if (itr == itru)
    {
      break;
    }
  }
  // const auto &from = from_acnts.get(owner, "no balance object found");
  // eosio_assert(from.balance >= value, "overdrawn balance");
}
/**
   * @dev Approve the passed account_name to spend the specified amount of tokens on behalf of msg.sender.
   * Beware that changing an allowance with this method brings the risk that someone may use both the old
   * and the new allowance by unfortunate transaction ordering. One possible solution to mitigate this
   * race condition is to first reduce the spender's allowance to 0 and set the desired value afterwards:
   * https://github.com/ethereum/EIPs/issues/20#issuecomment-263524729
   * @param _spender The account_name which will spend the funds.
   * @param _value The amount of tokens to be spent.
   */
void EOXToken::approve(account_name _spender, uint64_t _value)
{
  // allowed[msg.sender][_spender] = _value;
  // emit Approval(msg.sender, _spender, _value);

  allowed from_acnts(_self, _spender);

  auto pridx = from_acnts.get_index<N(name)>();
  auto itr = pridx.lower_bound(_self);
  auto itru = pridx.upper_bound(_self);

  for (; itr != pridx.end(); ++itr)
  {
    if (itr->name == _self && itr->sname == _spender)
    {
      auto iid = from_acnts.find(itr->id);
      from_acnts.modify(iid, _self, [&](auto &a) {
        a.balance = _value;
      });

      break;
    }
    if (itr == itru)
    {
      break;
    }
  }

  // return true;
}

/**
   * @dev Function to check the amount of tokens that an owner allowed to a spender.
   * @param _owner account_name The account_name which owns the funds.
   * @param _spender account_name The account_name which will spend the funds.
   * @return A uint64_t specifying the amount of tokens still available for the spender.
   */
uint64_t EOXToken::allowance(
    account_name _owner,
    account_name _spender)
// public
// view
// returns (uint64_t)
{
  // return allowed[_owner][_spender];

  allowed accountstable(_owner, _spender);
  // uint32_t zipnumb = 93445;

  auto pridx = accountstable.get_index<N(name)>();
  auto itr = pridx.lower_bound(_owner);
  auto itru = pridx.upper_bound(_owner);
  uint64_t v = 0;
  for (; itr != pridx.end(); ++itr)
  {
    if (itr->name == _owner &&itr->sname == _spender)
    {
      v = itr->balance;
      break;
    }
    if (itr == itru)
    {
      break;
    }
  }

  return v;
}

void EOXToken::add_abalance(account_name from, account_name sender, uint64_t value)
{
  allowed from_acnts(from, sender);

  auto pridx = from_acnts.get_index<N(name)>();
  auto itr = pridx.lower_bound(from);
  auto itru = pridx.upper_bound(from);

  for (; itr != pridx.end(); ++itr)
  {
    if (itr->name == from &&itr->sname == sender)
    {
      auto iid = from_acnts.find(itr->id);

      if (itr->balance == value)
      {
        from_acnts.erase(iid);
      }
      else
      {
        from_acnts.modify(iid, from, [&](auto &a) {
          a.balance += value;
        });
      }
      break;
    }
    if (itr == itru)
    {
      break;
    }
  }
  // return true;
}

/**
   * @dev Increase the amount of tokens that an owner allowed to a spender.
   * approve should be called when allowed[_spender] == 0. To increment
   * allowed value is better to use this function to avoid 2 calls (and wait until
   * the first transaction is mined)
   * From MonolithDAO Token.hpp
   * @param _spender The account_name which will spend the funds.
   * @param _addedValue The amount of tokens to increase the allowance by.
   */
void EOXToken::incapproval(
    account_name _spender,
    uint64_t _addedValue)
{
  // allowed[msg.sender][_spender] = (allowed[msg.sender][_spender].add(_addedValue));
  // emit Approval(msg.sender, _spender, allowed[msg.sender][_spender]);
  add_abalance(_self, _spender, _addedValue);
  // return true;
}

/**
   * @dev Decrease the amount of tokens that an owner allowed to a spender.
   * approve should be called when allowed[_spender] == 0. To decrement
   * allowed value is better to use this function to avoid 2 calls (and wait until
   * the first transaction is mined)
   * From MonolithDAO Token.hpp
   * @param _spender The account_name which will spend the funds.
   * @param _subtractedValue The amount of tokens to decrease the allowance by.
   */
void EOXToken::decapproval(
    account_name _spender,
    uint64_t _subtractedValue)
{
  uint64_t oldValue = allowance(_self, _spender); //allowed[msg.sender][_spender];

  if (_subtractedValue >= oldValue)
  {
    // allowed[msg.sender][_spender] = 0;
    sub_abalance(_self, _spender, 0);
  }
  else
  {
    // allowed[msg.sender][_spender] = oldValue.sub(_subtractedValue);
    sub_abalance(_self, _spender, _subtractedValue);
  }
  // emit Approval(msg.sender, _spender, allowed[msg.sender][_spender]);
  // return true;
}

EOSIO_ABI(EOXToken, (transfer)(transferfrom)(approve)(incapproval)(decapproval))
