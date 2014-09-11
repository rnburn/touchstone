#pragma once
#include <touchstone/benchmark_set.h>
#include <type_traits>
#include <iostream>
#include <cstdlib>

namespace touchstone {

class PositiveIntegralOption {
 public:
  PositiveIntegralOption(int value)
    : _value(value)
  {
    if(value <= 0) {
      std::cerr << "value " << value << " must be positive" << std::endl;
      std::abort();
    }
  }

  int value() const { return _value; }
 private:
  int _value;
};

struct NumEpochs : PositiveIntegralOption {
  using PositiveIntegralOption::PositiveIntegralOption;
};

struct NumTrials : PositiveIntegralOption {
  using PositiveIntegralOption::PositiveIntegralOption;
};

inline
void set_benchmark_option(BenchmarkSet& benchmark_set, NumEpochs num_epochs) {
  benchmark_set.set_num_epochs(num_epochs.value());
}

inline
void set_benchmark_option(BenchmarkSet& benchmark_set, NumTrials num_trials) {
  benchmark_set.set_num_trials(num_trials.value());
}

template<class T
       , typename std::enable_if<std::is_base_of<EnumerationRange, T>::value, int>::type = 0>
void set_benchmark_option(BenchmarkSet& benchmark_set, T enumeration_range) {
  benchmark_set.set_enumeration_range(std::make_shared<const T>(enumeration_range));
}

inline
void set_benchmark_options(BenchmarkSet& benchmark_set) {}

// uses a variadic template to set the options on a bnechmark set.
// options are identified by their type which allows the options
// to be specified in any order
template<class Option, class... OptionsRest>
void set_benchmark_options(BenchmarkSet& benchmark_set
                         , Option option
                         , OptionsRest... options_rest)
{
  set_benchmark_option(benchmark_set, option);
  set_benchmark_options(benchmark_set, options_rest...);
}


} //end namespace
