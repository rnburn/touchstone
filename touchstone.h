#pragma once

#include <touchstone/benchmark_option.h>
#include <touchstone/benchmark_registry.h>
#include <touchstone/linear_range.h>
#include <touchstone/expoential_range.h>
#include <thread>
#include <type_traits>

namespace touchstone {

template <class Configurer>
int build_benchmark_set(const char* name, BenchmarkSet::Runner&& runner,
                        Configurer configurer) {
  BenchmarkSet result(std::string(name), std::move(runner));
  configurer(result);
  BenchmarkRegistry::instance().add_benchmark_set(result);
  return 0;
}

inline int add_benchmark(const char* name, BenchmarkSet& benchmark_set) {
  return benchmark_set.add(
      Benchmark(std::string(name), benchmark_set.num_epochs()));
}

inline void record_elapse(Clock::time_point start, Clock::time_point finish,
                          BenchmarkSet& benchmark_set) {
  benchmark_set.record_elapse(start, finish);
}

template <class T>
void do_not_optimize_away(T&& x) {
  asm volatile("" : "+r" (x));
}

}  // end namespace

#define INTERNAL_TOUCHSTONE_UNIQUE_NAME_LINE2(name, line) name##line
#define INTERNAL_TOUCHSTONE_UNIQUE_NAME_LINE(name, line) \
  INTERNAL_TOUCHSTONE_UNIQUE_NAME_LINE2(name, line)
#define INTERNAL_TOUCHSTONE_UNIQUE_NAME(name) \
  INTERNAL_TOUCHSTONE_UNIQUE_NAME_LINE(name, __LINE__)

// macro to specify the creation of a benchmark set. it uses an anonymous
// function to
// set options so that the options can be specified without a namespace
// qualifier
#define BENCHMARK_SET(NAME, ...)                                           \
  static void INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_benchmark_set)(   \
      touchstone::BenchmarkSet&);                                          \
  static const int INTERNAL_TOUCHSTONE_UNIQUE_NAME(                        \
      touchstone_benchmark_registration) =                                 \
      touchstone::build_benchmark_set(                                     \
          NAME, INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_benchmark_set), \
          [](touchstone::BenchmarkSet & benchmark_set) {                   \
    using namespace touchstone;                                            \
    set_benchmark_options(benchmark_set, ##__VA_ARGS__);                   \
          });                                                              \
  static void INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_benchmark_set)(   \
      touchstone::BenchmarkSet & touchstone_benchmark_set)

#define BENCHMARK(NAME)                                                 \
  static int INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_benchmark_id) = \
      touchstone::add_benchmark(NAME, touchstone_benchmark_set);        \
  bool INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_is_active) = true;    \
  touchstone::Clock::time_point INTERNAL_TOUCHSTONE_UNIQUE_NAME(        \
      touchstone_start);                                                \
  touchstone::Clock::time_point INTERNAL_TOUCHSTONE_UNIQUE_NAME(        \
      touchstone_finish);                                               \
  for (INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_start) =              \
           touchstone::Clock::now();                                    \
       (touchstone_benchmark_set.current_benchmark_id() ==              \
        INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_benchmark_id)) &&    \
           INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_is_active);       \
       INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_finish) =             \
           touchstone::Clock::now(),                                    \
      INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_is_active) = false,    \
      touchstone::record_elapse(                                        \
           INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_start),           \
           INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_finish),          \
           touchstone_benchmark_set))

#ifdef TOUCHSTONE_CONFIG_MAIN
int main() {
  touchstone::BenchmarkRegistry::instance().run_all();
  return 0;
}
#endif
