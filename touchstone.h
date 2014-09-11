#pragma once

#include <touchstone/benchmark_option.h>
#include <touchstone/benchmark_registry.h>
#include <touchstone/linear_range.h>
#include <touchstone/expoential_range.h>
#include <chrono>
#include <thread>
#include <type_traits>

namespace touchstone {

using Clock = std::conditional<std::chrono::high_resolution_clock::is_steady
                             , std::chrono::high_resolution_clock
                             , std::chrono::steady_clock>::type;

template<class Configurer>
int build_benchmark_set(const char* name
                      , BenchmarkSet::Runner&& runner
                      , Configurer configurer) 
{
  BenchmarkSet result(std::string(name), std::move(runner));
  configurer(result);
  BenchmarkRegistry::instance().add_benchmark_set(result);
  return 0;
}

inline
int add_benchmark(const char* name, BenchmarkSet& benchmark_set) {
  return benchmark_set.add(Benchmark(std::string(name), benchmark_set.num_epochs()));
}

inline
void record_elapse(Clock::time_point start
                 , Clock::time_point finish
                 , BenchmarkSet& benchmark_set)
{
  auto duration = finish - start;
  benchmark_set.record_elapse(
    std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count()
  );
}


// copied from the Celero project (https://github.com/DigitalInBlue/Celero)
//
// Celero
//
// C++ Benchmarking Library
//
// Copyright 2013 John Farrier 
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
template<class T> 
void do_not_optimize_away(T&& x)
{
  //
  // We must always do this test, but it will never pass.
  //
  // A new thread::id does not represent a thread.
  // getpid() and _getpid() were considered here, but
  // there are limitations on Windows:
  // http://msdn.microsoft.com/en-us/library/t2y34y40.aspx
  //
  if(std::this_thread::get_id() != std::this_thread::get_id())
  {
    // This forces the value to never be optimized away
    // by taking a reference then using it.
    const auto* p = &x;
    putchar(*reinterpret_cast<const char*>(p));

    // If we do get here, kick out because something has gone wrong.
    std::abort();
  }
}

} //end namespace

#define INTERNAL_TOUCHSTONE_UNIQUE_NAME_LINE2( name, line ) name##line
#define INTERNAL_TOUCHSTONE_UNIQUE_NAME_LINE( name, line ) INTERNAL_TOUCHSTONE_UNIQUE_NAME_LINE2( name, line )
#define INTERNAL_TOUCHSTONE_UNIQUE_NAME( name ) INTERNAL_TOUCHSTONE_UNIQUE_NAME_LINE( name, __LINE__ )

// macro to specify the creation of a benchmark set. it uses an anonymous function to
// set options so that the options can be specified without a namespace qualifier
#define BENCHMARK_SET(NAME, ...) \
 static void INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_benchmark_set)(touchstone::BenchmarkSet&); \
 static const int INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_benchmark_registration) = \
      touchstone::build_benchmark_set(NAME \
                                   , INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_benchmark_set) \
                                   , [](touchstone::BenchmarkSet& benchmark_set) { \
                                        using namespace touchstone; \
                                        set_benchmark_options(benchmark_set, ##__VA_ARGS__); \
                                     }); \
  static void INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_benchmark_set) \
    (touchstone::BenchmarkSet& touchstone_benchmark_set) 

#define BENCHMARK(NAME) \
  static int INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_benchmark_id) = \
      touchstone::add_benchmark(NAME, touchstone_benchmark_set); \
  bool INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_is_active) = true; \
  touchstone::Clock::time_point INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_start); \
  touchstone::Clock::time_point INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_finish); \
  for( \
    INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_start) = touchstone::Clock::now() \
  ; (touchstone_benchmark_set.current_benchmark_id() == \
      INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_benchmark_id)) \
 && INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_is_active) \
  ; INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_finish) = touchstone::Clock::now() \
  , INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_is_active) = false \
  , touchstone::record_elapse(INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_start) \
                            , INTERNAL_TOUCHSTONE_UNIQUE_NAME(touchstone_finish) \
                            , touchstone_benchmark_set)) 

  
#ifdef TOUCHSTONE_CONFIG_MAIN
int main() {
  touchstone::BenchmarkRegistry::instance().run_all();
  return 0;
}
#endif
