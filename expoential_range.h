#pragma once

#include <touchstone/enumeration_range.h>
#include <cstdlib>
#include <iostream>

namespace touchstone {

class ExpoentialRange : public EnumerationRange {
 public:
  ExpoentialRange(int a, int b, int base)
    : _a(a)
    , _b(b)
    , _base(base)
  {
    if(a <= 0 || b <= 0 || base <= 0) {
      std::cerr << "all arguments to expoential range must be non-negative" << std::endl;
      std::abort();
    }
  }

  int begin() const override { 
    return power(_base, _a); 
  }
  int end() const override { 
    return power(_base, _b+1); 
  }
  int advance(int n) const override { return n*_base; }
 private:
  static int power(int base, int n) {
    int result = 1;
    for(; n > 0; --n) {
      result *= base;
    }
    return result;
  }
  int _a, _b, _base;
};

} //end namespace

