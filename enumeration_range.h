#pragma once

namespace touchstone {

// base class to specify a range of integral values to run a benchmark over
class EnumerationRange {
 public:
  virtual int begin() const { return 0; }
  virtual int end() const { return 1; }
  virtual int advance(int n) const { return 1; }
};

} //end namespace
