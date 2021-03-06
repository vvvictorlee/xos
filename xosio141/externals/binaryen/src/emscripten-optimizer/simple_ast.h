/*
 * Copyright 2015 WebAssembly Community Group participants
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef wasm_simple_ast_h
#define wasm_simple_ast_h

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <ostream>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "parser.h"
#include "snprintf.h"
#include "support/safe_integer.h"
#include "mixed_arena.h"

#define err(str) fprintf(stderr, str "\n");
#define errv(str, ...) fprintf(stderr, str "\n", __VA_ARGS__);
#define printErr err

namespace cashew {

struct Value;
struct Ref;

void dump(const char *str, Ref node, bool pretty=false);

// Reference to a value, plus some operators for convenience
struct Ref {
  Value* inst;

  Ref(Value *v=nullptr) : inst(v) {}

  Value* get() { return inst; }

  Value& operator*() { return *inst; }
  Value* operator->() { return inst; }
  Ref& operator[](unsigned x);
  Ref& operator[](IString x);

  // special conveniences
  bool operator==(const char *str); // comparison to string, which is by value
  bool operator!=(const char *str);
  bool operator==(const IString &str);
  bool operator!=(const IString &str);
  bool operator==(double d) { abort(); return false; } // prevent Ref == number, which is potentially ambiguous; use ->getNumber() == number
  bool operator==(Ref other);
  bool operator!(); // check if null, in effect
};

// Arena allocation, free it all on process exit

// A mixed arena for global allocation only, so members do not
// receive an allocator, they all use the global one anyhow
class GlobalMixedArena : public MixedArena {
public:
  template<class T>
  T* alloc() {
    auto* ret = static_cast<T*>(allocSpace(sizeof(T)));
    new (ret) T();
    return ret;
  }
};

extern GlobalMixedArena arena;

class ArrayStorage : public ArenaVectorBase<ArrayStorage, Ref> {
public:
  void allocate(size_t size) {
    allocatedElements = size;
    data = static_cast<Ref*>(arena.allocSpace(sizeof(Ref) * allocatedElements));
  }
};

struct Assign;
struct AssignName;

// Main value type
struct Value {
  enum Type {
    String = 0,
    Number = 1,
    Array = 2,
    Null = 3,
    Bool = 4,
    Object = 5,
    Assign_ = 6, // ref = target
    AssignName_ = 7
  };

  Type type;

  typedef std::unordered_map<IString, Ref> ObjectStorage;

#ifdef _MSC_VER // MSVC does not allow unrestricted unions: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2544.pdf
  IString str;
#endif
  union { // TODO: optimize
#ifndef _MSC_VER
    IString str;
#endif
    double num;
    ArrayStorage *arr;
    bool boo;
    ObjectStorage *obj;
    Ref ref;
  };

  // constructors all copy their input
  Value() : type(Null), num(0) {}
  explicit Value(const char *s) : type(Null) {
    setString(s);
  }
  explicit Value(double n) : type(Null) {
    setNumber(n);
  }
  explicit Value(ArrayStorage &a) : type(Null) {
    setArray();
    *arr = a;
  }
  // no bool constructor - would endanger the double one (int might convert the wrong way)

  ~Value() {
    free();
  }

  void free() {
    if (type == Array) { arr->clear(); }
    else if (type == Object) delete obj;
    type = Null;
    num = 0;
  }

  Value& setString(const char *s) {
    free();
    type = String;
    str.set(s);
    return *this;
  }
  Value& setString(const IString &s) {
    free();
    type = String;
    str.set(s);
    return *this;
  }
  Value& setNumber(double n) {
    free();
    type = Number;
    num = n;
    return *this;
  }
  Value& setArray(ArrayStorage &a) {
    free();
    type = Array;
    arr = arena.alloc<ArrayStorage>();
    *arr = a;
    return *this;
  }
  Value& setArray(size_t size_hint=0) {
    free();
    type = Array;
    arr = arena.alloc<ArrayStorage>();
    arr->reserve(size_hint);
    return *this;
  }
  Value& setNull() {
    free();
    type = Null;
    return *this;
  }
  Value& setBool(bool b) { // Bool in the name, as otherwise might overload over int
    free();
    type = Bool;
    boo = b;
    return *this;
  }
  Value& setObject() {
    free();
    type = Object;
    obj = new ObjectStorage();
    return *this;
  }
  Value& setAssign(Ref target, Ref value);
  Value& setAssignName(IString target, Ref value);

  bool isString() { return type == String; }
  bool isNumber() { return type == Number; }
  bool isArray()  { return type == Array; }
  bool isNull()   { return type == Null; }
  bool isBool()   { return type == Bool; }
  bool isObject() { return type == Object; }
  bool isAssign() { return type == Assign_; }
  bool isAssignName() { return type == AssignName_; }

  bool isBool(bool b) { return type == Bool && b == boo; } // avoid overloading == as it might overload over int

  // convenience function to check if something is an array and
  // also has a certain string as the first element. This is a
  // very common operation as the first element defines the node
  // type for most ast nodes
  bool isArray(IString name) {
    return isArray() && (*this)[0] == name;
  }

  const char* getCString() {
    ASSERT_THROW(isString());
    return str.str;
  }
  IString& getIString() {
    ASSERT_THROW(isString());
    return str;
  }
  double& getNumber() {
    ASSERT_THROW(isNumber());
    return num;
  }
  ArrayStorage& getArray() {
    ASSERT_THROW(isArray());
    return *arr;
  }
  bool& getBool() {
    ASSERT_THROW(isBool());
    return boo;
  }

  Assign* asAssign();
  AssignName* asAssignName();

  int32_t getInteger() { // convenience function to get a known integer
    ASSERT_THROW(fmod(getNumber(), 1) == 0);
    int32_t ret = getNumber();
    ASSERT_THROW(double(ret) == getNumber()); // no loss in conversion
    return ret;
  }

  Value& operator=(const Value& other) {
    free();
    switch (other.type) {
      case String:
        setString(other.str);
        break;
      case Number:
        setNumber(other.num);
        break;
      case Array:
        setArray(*other.arr);
        break;
      case Null:
        setNull();
        break;
      case Bool:
        setBool(other.boo);
        break;
      default:
        abort(); // TODO
    }
    return *this;
  }

  bool operator==(const Value& other) {
    if (type != other.type) return false;
    switch (other.type) {
      case String:
        return str == other.str;
      case Number:
        return num == other.num;
      case Array:
        return this == &other; // if you want a deep compare, use deepCompare
      case Null:
        break;
      case Bool:
        return boo == other.boo;
      case Object:
        return this == &other; // if you want a deep compare, use deepCompare
      default:
        abort();
    }
    return true;
  }

  char* parse(char* curr) {
    #define is_json_space(x) (x == 32 || x == 9 || x == 10 || x == 13) /* space, tab, linefeed/newline, or return */
    #define skip() { while (*curr && is_json_space(*curr)) curr++; }
    skip();
    if (*curr == '"') {
      // String
      curr++;
      char *close = strchr(curr, '"');
      ASSERT_THROW(close);
      *close = 0; // end this string, and reuse it straight from the input
      setString(curr);
      curr = close+1;
    } else if (*curr == '[') {
      // Array
      curr++;
      skip();
      setArray();
      while (*curr != ']') {
        Ref temp = arena.alloc<Value>();
        arr->push_back(temp);
        curr = temp->parse(curr);
        skip();
        if (*curr == ']') break;
        ASSERT_THROW(*curr == ',');
        curr++;
        skip();
      }
      curr++;
    } else if (*curr == 'n') {
      // Null
      ASSERT_THROW(strncmp(curr, "null", 4) == 0);
      setNull();
      curr += 4;
    } else if (*curr == 't') {
      // Bool true
      ASSERT_THROW(strncmp(curr, "true", 4) == 0);
      setBool(true);
      curr += 4;
    } else if (*curr == 'f') {
      // Bool false
      ASSERT_THROW(strncmp(curr, "false", 5) == 0);
      setBool(false);
      curr += 5;
    } else if (*curr == '{') {
      // Object
      curr++;
      skip();
      setObject();
      while (*curr != '}') {
        ASSERT_THROW(*curr == '"');
        curr++;
        char *close = strchr(curr, '"');
        ASSERT_THROW(close);
        *close = 0; // end this string, and reuse it straight from the input
        IString key(curr);
        curr = close+1;
        skip();
        ASSERT_THROW(*curr == ':');
        curr++;
        skip();
        Ref value = arena.alloc<Value>();
        curr = value->parse(curr);
        (*obj)[key] = value;
        skip();
        if (*curr == '}') break;
        ASSERT_THROW(*curr == ',');
        curr++;
        skip();
      }
      curr++;
    } else {
      // Number
      char *after;
      setNumber(strtod(curr, &after));
      curr = after;
    }
    return curr;
  }

  void stringify(std::ostream &os, bool pretty=false);

  // String operations

  // Number operations

  // Array operations

  size_t size() {
    ASSERT_THROW(isArray());
    return arr->size();
  }

  void setSize(size_t size) {
    ASSERT_THROW(isArray());
    auto old = arr->size();
    if (old != size) arr->resize(size);
    if (old < size) {
      for (auto i = old; i < size; i++) {
        (*arr)[i] = arena.alloc<Value>();
      }
    }
  }

  Ref& operator[](unsigned x) {
    ASSERT_THROW(isArray());
    return (*arr)[x];
  }

  Value& push_back(Ref r) {
    ASSERT_THROW(isArray());
    arr->push_back(r);
    return *this;
  }
  Ref pop_back() {
    ASSERT_THROW(isArray());
    Ref ret = arr->back();
    arr->pop_back();
    return ret;
  }

  Ref back() {
    ASSERT_THROW(isArray());
    if (arr->size() == 0) return nullptr;
    return arr->back();
  }

  int indexOf(Ref other) {
    ASSERT_THROW(isArray());
    for (size_t i = 0; i < arr->size(); i++) {
      if (other == (*arr)[i]) return i;
    }
    return -1;
  }

  Ref map(std::function<Ref (Ref node)> func) {
    ASSERT_THROW(isArray());
    Ref ret = arena.alloc<Value>();
    ret->setArray();
    for (size_t i = 0; i < arr->size(); i++) {
      ret->push_back(func((*arr)[i]));
    }
    return ret;
  }

  Ref filter(std::function<bool (Ref node)> func) {
    ASSERT_THROW(isArray());
    Ref ret = arena.alloc<Value>();
    ret->setArray();
    for (size_t i = 0; i < arr->size(); i++) {
      Ref curr = (*arr)[i];
      if (func(curr)) ret->push_back(curr);
    }
    return ret;
  }

  /*
  void forEach(std::function<void (Ref)> func) {
    for (size_t i = 0; i < arr->size(); i++) {
      func((*arr)[i]);
    }
  }
  */

  // Null operations

  // Bool operations

  // Object operations

  Ref& operator[](IString x) {
    ASSERT_THROW(isObject());
    return (*obj)[x];
  }

  bool has(IString x) {
    ASSERT_THROW(isObject());
    return obj->count(x) > 0;
  }
};

struct Assign : public Value {
  Ref value_;

  Assign(Ref targetInit, Ref valueInit) {
    type = Assign_;
    target() = targetInit;
    value() = valueInit;
  }

  Assign() : Assign(nullptr, nullptr) {}

  Ref& target() {
    return ref;
  }
  Ref& value() {
    return value_;
  }
};

struct AssignName : public Value {
  IString target_;

  AssignName(IString targetInit, Ref valueInit) {
    type = AssignName_;
    target() = targetInit;
    value() = valueInit;
  }

  AssignName() : AssignName(IString(), nullptr) {}

  IString& target() {
    return target_;
  }
  Ref& value() {
    return ref;
  }
};

// AST traversals

// Traverse, calling visit before the children
void traversePre(Ref node, std::function<void (Ref)> visit);

// Traverse, calling visitPre before the children and visitPost after
void traversePrePost(Ref node, std::function<void (Ref)> visitPre, std::function<void (Ref)> visitPost);

// Traverse, calling visitPre before the children and visitPost after. If pre returns false, do not traverse children
void traversePrePostConditional(Ref node, std::function<bool (Ref)> visitPre, std::function<void (Ref)> visitPost);

// Traverses all the top-level functions in the document
void traverseFunctions(Ref ast, std::function<void (Ref)> visit);

// JS printing support

struct JSPrinter {
  static char* numToString(double d, bool finalize=true) {
    bool neg = d < 0;
    if (neg) d = -d;
    // try to emit the fewest necessary characters
    bool integer = fmod(d, 1) == 0;
    #define BUFFERSIZE 1000
    static char full_storage_f[BUFFERSIZE], full_storage_e[BUFFERSIZE]; // f is normal, e is scientific for float, x for integer
    static char *storage_f = full_storage_f + 1, *storage_e = full_storage_e + 1; // full has one more char, for a possible '-'
    auto err_f = std::numeric_limits<double>::quiet_NaN();
    auto err_e = std::numeric_limits<double>::quiet_NaN();
    for (int e = 0; e <= 1; e++) {
      char *buffer = e ? storage_e : storage_f;
      double temp;
      if (!integer) {
        static char format[6];
        for (int i = 0; i <= 18; i++) {
          format[0] = '%';
          format[1] = '.';
          if (i < 10) {
            format[2] = '0' + i;
            format[3] = e ? 'e' : 'f';
            format[4] = 0;
          } else {
            format[2] = '1';
            format[3] = '0' + (i - 10);
            format[4] = e ? 'e' : 'f';
            format[5] = 0;
          }
          snprintf(buffer, BUFFERSIZE-1, format, d);
          sscanf(buffer, "%lf", &temp);
          //errv("%.18f, %.18e   =>   %s   =>   %.18f, %.18e   (%d), ", d, d, buffer, temp, temp, temp == d);
          if (temp == d) break;
        }
      } else {
        // integer
        ASSERT_THROW(d >= 0);
        if (wasm::isUInteger64(d)) {
          unsigned long long uu = wasm::toUInteger64(d);
          bool asHex = e && !finalize;
          snprintf(buffer, BUFFERSIZE-1, asHex ? "0x%llx" : "%llu", uu);
          if (asHex) {
            unsigned long long tempULL;
            sscanf(buffer, "%llx", &tempULL);
            temp = (double)tempULL;
          } else {
            sscanf(buffer, "%lf", &temp);
          }
        } else {
          // too large for a machine integer, just use floats
          snprintf(buffer, BUFFERSIZE-1, e ? "%e" : "%.0f", d); // even on integers, e with a dot is useful, e.g. 1.2e+200
          sscanf(buffer, "%lf", &temp);
        }
        //errv("%.18f, %.18e   =>   %s   =>   %.18f, %.18e, %llu   (%d)\n", d, d, buffer, temp, temp, uu, temp == d);
      }
      (e ? err_e : err_f) = fabs(temp - d);
      //errv("current attempt: %.18f  =>  %s", d, buffer);
      //ASSERT_THROW(temp == d);
      char *dot = strchr(buffer, '.');
      if (dot) {
        // remove trailing zeros
        char *end = dot+1;
        while (*end >= '0' && *end <= '9') end++;
        end--;
        while (*end == '0') {
          char *copy = end;
          do {
            copy[0] = copy[1];
          } while (*copy++ != 0);
          end--;
        }
        //errv("%.18f  =>   %s", d, buffer);
        // remove preceding zeros
        while (*buffer == '0') {
          char *copy = buffer;
          do {
            copy[0] = copy[1];
          } while (*copy++ != 0);
        }
        //errv("%.18f ===>  %s", d, buffer);
      } else if (!integer || !e) {
        // no dot. try to change 12345000 => 12345e3
        char *end = strchr(buffer, 0);
        end--;
        char *test = end;
        // remove zeros, and also doubles can use at most 24 digits, we can truncate any extras even if not zero
        while ((*test == '0' || test - buffer > 24) && test > buffer) test--;
        int num = end - test;
        if (num >= 3) {
          test++;
          test[0] = 'e';
          if (num < 10) {
            test[1] = '0' + num;
            test[2] = 0;
          } else if (num < 100) {
            test[1] = '0' + (num / 10);
            test[2] = '0' + (num % 10);
            test[3] = 0;
          } else {
            ASSERT_THROW(num < 1000);
            test[1] = '0' + (num / 100);
            test[2] = '0' + (num % 100) / 10;
            test[3] = '0' + (num % 10);
            test[4] = 0;
          }
        }
      }
      //errv("..current attempt: %.18f  =>  %s", d, buffer);
    }
    //fprintf(stderr, "options:\n%s\n%s\n (first? %d)\n", storage_e, storage_f, strlen(storage_e) < strlen(storage_f));
    char *ret;
    if (err_e == err_f) {
      ret = strlen(storage_e) < strlen(storage_f) ? storage_e : storage_f;
    } else {
      ret = err_e < err_f ? storage_e : storage_f;
    }
    if (neg) {
      ret--; // safe to go back one, there is one more char in full_*
      *ret = '-';
    }
    return ret;
  }
};

// cashew builder

class ValueBuilder {
  static Ref makeRawString(const IString& s) {
    return &arena.alloc<Value>()->setString(s);
  }

  static Ref makeNull() {
    return &arena.alloc<Value>()->setNull();
  }

public:
  static Ref makeRawArray(int size_hint=0) {
    return &arena.alloc<Value>()->setArray(size_hint);
  }

  static Ref makeToplevel() {
    return &makeRawArray(2)->push_back(makeRawString(TOPLEVEL))
                            .push_back(makeRawArray());
  }

  static Ref makeString(IString str) {
    return &makeRawArray(2)->push_back(makeRawString(STRING))
                            .push_back(makeRawString(str));
  }

  static Ref makeBlock() {
    return &makeRawArray(2)->push_back(makeRawString(BLOCK))
                            .push_back(makeRawArray());
  }

  static Ref makeName(IString name) {
    return makeRawString(name);
  }

  static void setBlockContent(Ref target, Ref block) {
    if (target[0] == TOPLEVEL) {
      target[1]->setArray(block[1]->getArray());
    } else if (target[0] == DEFUN) {
      target[3]->setArray(block[1]->getArray());
    } else abort();
  }

  static void appendToBlock(Ref block, Ref element) {
    ASSERT_THROW(block[0] == BLOCK);
    block[1]->push_back(element);
  }

  static Ref makeCall(Ref target) {
    return &makeRawArray(3)->push_back(makeRawString(CALL))
                            .push_back(target)
                            .push_back(makeRawArray());
  }
  static Ref makeCall(Ref target, Ref arg) {
    Ref ret = &makeRawArray(3)->push_back(makeRawString(CALL))
                               .push_back(target)
                               .push_back(makeRawArray());
    ret[2]->push_back(arg);
    return ret;
  }
  static Ref makeCall(IString target) {
    Ref ret = &makeRawArray(3)->push_back(makeRawString(CALL))
                               .push_back(makeName(target))
                               .push_back(makeRawArray());
    return ret;
  }
  static Ref makeCall(IString target, Ref arg) {
    Ref ret = &makeRawArray(3)->push_back(makeRawString(CALL))
                               .push_back(makeName(target))
                               .push_back(makeRawArray(1));
    ret[2]->push_back(arg);
    return ret;
  }
  static Ref makeCall(IString target, Ref arg1, Ref arg2) {
    Ref ret = &makeRawArray(3)->push_back(makeRawString(CALL))
                               .push_back(makeName(target))
                               .push_back(makeRawArray(2));
    ret[2]->push_back(arg1);
    ret[2]->push_back(arg2);
    return ret;
  }
  static Ref makeCall(IString target, Ref arg1, Ref arg2, Ref arg3, Ref arg4) {
    Ref ret = &makeRawArray(3)->push_back(makeRawString(CALL))
                               .push_back(makeName(target))
                               .push_back(makeRawArray(4));
    ret[2]->push_back(arg1);
    ret[2]->push_back(arg2);
    ret[2]->push_back(arg3);
    ret[2]->push_back(arg4);
    return ret;
  }
  static Ref makeCall(IString target, Ref arg1, Ref arg2, Ref arg3, Ref arg4, Ref arg5, Ref arg6, Ref arg7, Ref arg8) {
    Ref ret = &makeRawArray(3)->push_back(makeRawString(CALL))
                               .push_back(makeName(target))
                               .push_back(makeRawArray(8));
    ret[2]->push_back(arg1);
    ret[2]->push_back(arg2);
    ret[2]->push_back(arg3);
    ret[2]->push_back(arg4);
    ret[2]->push_back(arg5);
    ret[2]->push_back(arg6);
    ret[2]->push_back(arg7);
    ret[2]->push_back(arg8);
    return ret;
  }
  static Ref makeCall(IString target, Ref arg1, Ref arg2, Ref arg3, Ref arg4, Ref arg5, Ref arg6, Ref arg7, Ref arg8, Ref arg9, Ref arg10, Ref arg11, Ref arg12, Ref arg13, Ref arg14, Ref arg15, Ref arg16) {
    Ref ret = &makeRawArray(3)->push_back(makeRawString(CALL))
                               .push_back(makeName(target))
                               .push_back(makeRawArray(16));
    ret[2]->push_back(arg1);
    ret[2]->push_back(arg2);
    ret[2]->push_back(arg3);
    ret[2]->push_back(arg4);
    ret[2]->push_back(arg5);
    ret[2]->push_back(arg6);
    ret[2]->push_back(arg7);
    ret[2]->push_back(arg8);
    ret[2]->push_back(arg9);
    ret[2]->push_back(arg10);
    ret[2]->push_back(arg11);
    ret[2]->push_back(arg12);
    ret[2]->push_back(arg13);
    ret[2]->push_back(arg14);
    ret[2]->push_back(arg15);
    ret[2]->push_back(arg16);
    return ret;
  }

  static void appendToCall(Ref call, Ref element) {
    ASSERT_THROW(call[0] == CALL);
    call[2]->push_back(element);
  }

  static Ref makeStatement(Ref contents) {
    return contents;
  }

  static Ref makeDouble(double num) {
    return &arena.alloc<Value>()->setNumber(num);
  }
  static Ref makeInt(uint32_t num) {
    return makeDouble(double(num));
  }
  static Ref makeNum(double num) {
    return makeDouble(num);
  }

  static Ref makeUnary(IString op, Ref value) {
    return &makeRawArray(3)->push_back(makeRawString(UNARY_PREFIX))
                            .push_back(makeRawString(op))
                            .push_back(value);
  }

  static Ref makeBinary(Ref left, IString op, Ref right) {
    if (op == SET) {
      if (left->isString()) {
        return &arena.alloc<AssignName>()->setAssignName(left->getIString(), right);
      } else {
        return &arena.alloc<Assign>()->setAssign(left, right);
      }
    } else if (op == COMMA) {
      return &makeRawArray(3)->push_back(makeRawString(SEQ))
                              .push_back(left)
                              .push_back(right);
    } else {
      return &makeRawArray(4)->push_back(makeRawString(BINARY))
                              .push_back(makeRawString(op))
                              .push_back(left)
                              .push_back(right);
    }
  }

  static Ref makePrefix(IString op, Ref right) {
    return &makeRawArray(3)->push_back(makeRawString(UNARY_PREFIX))
                            .push_back(makeRawString(op))
                            .push_back(right);
  }

  static Ref makeFunction(IString name) {
    return &makeRawArray(4)->push_back(makeRawString(DEFUN))
                            .push_back(makeRawString(name))
                            .push_back(makeRawArray())
                            .push_back(makeRawArray());
  }

  static void appendArgumentToFunction(Ref func, IString arg) {
    ASSERT_THROW(func[0] == DEFUN);
    func[2]->push_back(makeRawString(arg));
  }

  static Ref makeVar(bool is_const=false) {
    return &makeRawArray(2)->push_back(makeRawString(VAR))
                            .push_back(makeRawArray());
  }

  static void appendToVar(Ref var, IString name, Ref value) {
    ASSERT_THROW(var[0] == VAR);
    Ref array = &makeRawArray(1)->push_back(makeRawString(name));
    if (!!value) array->push_back(value);
    var[1]->push_back(array);
  }

  static Ref makeReturn(Ref value) {
    return &makeRawArray(2)->push_back(makeRawString(RETURN))
                            .push_back(!!value ? value : makeNull());
  }

  static Ref makeIndexing(Ref target, Ref index) {
    return &makeRawArray(3)->push_back(makeRawString(SUB))
                            .push_back(target)
                            .push_back(index);
  }

  static Ref makeIf(Ref condition, Ref ifTrue, Ref ifFalse) {
    return &makeRawArray(4)->push_back(makeRawString(IF))
                            .push_back(condition)
                            .push_back(ifTrue)
                            .push_back(!!ifFalse ? ifFalse : makeNull());
  }

  static Ref makeConditional(Ref condition, Ref ifTrue, Ref ifFalse) {
    return &makeRawArray(4)->push_back(makeRawString(CONDITIONAL))
                            .push_back(condition)
                            .push_back(ifTrue)
                            .push_back(ifFalse);
  }

  static Ref makeSeq(Ref left, Ref right) {
    return &makeRawArray(3)->push_back(makeRawString(SEQ))
                            .push_back(left)
                            .push_back(right);
  }

  static Ref makeDo(Ref body, Ref condition) {
    return &makeRawArray(3)->push_back(makeRawString(DO))
                            .push_back(condition)
                            .push_back(body);
  }

  static Ref makeWhile(Ref condition, Ref body) {
    return &makeRawArray(3)->push_back(makeRawString(WHILE))
                            .push_back(condition)
                            .push_back(body);
  }

  static Ref makeFor(Ref init, Ref condition, Ref inc, Ref body) {
    return &makeRawArray(5)->push_back(makeRawString(FOR))
                            .push_back(init)
                            .push_back(condition)
                            .push_back(inc)
                            .push_back(body);
  }

  static Ref makeBreak(IString label) {
    return &makeRawArray(2)->push_back(makeRawString(BREAK))
                            .push_back(!!label ? makeRawString(label) : makeNull());
  }

  static Ref makeContinue(IString label) {
    return &makeRawArray(2)->push_back(makeRawString(CONTINUE))
                            .push_back(!!label ? makeRawString(label) : makeNull());
  }

  static Ref makeLabel(IString name, Ref body) {
    return &makeRawArray(3)->push_back(makeRawString(LABEL))
                            .push_back(makeRawString(name))
                            .push_back(body);
  }

  static Ref makeSwitch(Ref input) {
    return &makeRawArray(3)->push_back(makeRawString(SWITCH))
                            .push_back(input)
                            .push_back(makeRawArray());
  }

  static void appendCaseToSwitch(Ref switch_, Ref arg) {
    ASSERT_THROW(switch_[0] == SWITCH);
    switch_[2]->push_back(&makeRawArray(2)->push_back(arg)
                                           .push_back(makeRawArray()));
  }

  static void appendDefaultToSwitch(Ref switch_) {
    ASSERT_THROW(switch_[0] == SWITCH);
    switch_[2]->push_back(&makeRawArray(2)->push_back(makeNull())
                                           .push_back(makeRawArray()));
  }

  static void appendCodeToSwitch(Ref switch_, Ref code, bool explicitBlock) {
    ASSERT_THROW(switch_[0] == SWITCH);
    ASSERT_THROW(code[0] == BLOCK);
    if (!explicitBlock) {
      for (size_t i = 0; i < code[1]->size(); i++) {
        switch_[2]->back()->back()->push_back(code[1][i]);
      }
    } else {
      switch_[2]->back()->back()->push_back(code);
    }
  }

  static Ref makeDot(Ref obj, IString key) {
    return &makeRawArray(3)->push_back(makeRawString(DOT))
                            .push_back(obj)
                            .push_back(makeRawString(key));
  }

  static Ref makeDot(Ref obj, Ref key) {
    ASSERT_THROW(key->isString());
    return makeDot(obj, key->getIString());
  }

  static Ref makeNew(Ref call) {
    return &makeRawArray(2)->push_back(makeRawString(NEW))
                            .push_back(call);
  }

  static Ref makeArray() {
    return &makeRawArray(2)->push_back(makeRawString(ARRAY))
                            .push_back(makeRawArray());
  }

  static void appendToArray(Ref array, Ref element) {
    ASSERT_THROW(array[0] == ARRAY);
    array[1]->push_back(element);
  }

  static Ref makeObject() {
    return &makeRawArray(2)->push_back(makeRawString(OBJECT))
                            .push_back(makeRawArray());
  }

  static void appendToObject(Ref array, IString key, Ref value) {
    ASSERT_THROW(array[0] == OBJECT);
    array[1]->push_back(&makeRawArray(2)->push_back(makeRawString(key))
                                         .push_back(value));
  }

  static Ref makeSub(Ref obj, Ref index) {
    return &makeRawArray(2)->push_back(makeRawString(SUB))
                            .push_back(obj)
                            .push_back(index);
  }

  static Ref makePtrShift(Ref ptr, int shifts) {
    return makeBinary(ptr, RSHIFT, makeInt(shifts));
  }
};

// Tolerates 0.0 in the input; does not trust a +() to be there.
class DotZeroValueBuilder : public ValueBuilder {
public:
  static Ref makeDouble(double num) {
    return makePrefix(PLUS, ValueBuilder::makeDouble(num));
  }
};

} // namespace cashew

#endif // wasm_simple_ast_h
