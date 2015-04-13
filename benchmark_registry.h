#pragma once

#include <touchstone/benchmark_set.h>

namespace touchstone {

class BenchmarkRegistry {
  using Container = boost::container::flat_set<BenchmarkSet>;

 public:
  void add_benchmark_set(const BenchmarkSet& benchmark_set) {
    auto was_inserted = _benchmark_sets.insert(benchmark_set);
    if (!was_inserted.second) {
      std::cerr << "benchmark " << benchmark_set.name() << " already exits"
                << std::endl;
      std::abort();
    }
  }

  static BenchmarkRegistry& instance() {
    static BenchmarkRegistry registry;
    return registry;
  }

  void run_all() {
    for (auto& benchmark_set : _benchmark_sets) {
      benchmark_set.run();
      benchmark_set.print_results();
    }
  }

 private:
  BenchmarkRegistry() {}
  Container _benchmark_sets;
};

}  // end namespace
