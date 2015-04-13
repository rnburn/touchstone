#pragma once

#include <touchstone/enumeration_range.h>
#include <cstdlib>
#include <iostream>

namespace touchstone {

class LinearRange : public EnumerationRange {
 public:
  LinearRange(int first, int last, int step = 1)
      : _first(first), _last(last), _step(step) {
    if (step <= 0) {
      std::cerr << "step must be positive" << std::endl;
      std::abort();
    }
  }

  int begin() const override { return _first; }
  int end() const override { return _last + 1; }
  int advance(int n) const override { return n + _step; }

 private:
  int _first, _last, _step;
};

}  // end namespace
