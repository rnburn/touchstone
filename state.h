#pragma once
#include <random>

namespace touchstone {

using RandomNumberGenerator = std::mt19937;

class State {
  friend class BenchmarkSet;
  friend int n();
  friend const RandomNumberGenerator& random_number_generator();

  static int& get_n() {
    static int n;
    return n;
  }

  static RandomNumberGenerator& get_random_number_generator() {
    static RandomNumberGenerator random_number_generator;
    return random_number_generator;
  }
};

inline int n() { return State::get_n(); }

inline const RandomNumberGenerator& random_number_generator() {
  return State::get_random_number_generator();
}

}  // end namespace
