#pragma once

#include <functional>
#include <touchstone/benchmark.h>
#include <touchstone/enumeration_range.h>
#include <touchstone/state.h>
#include <boost/container/flat_set.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <memory>
#include <algorithm>
#include <numeric>

namespace touchstone {

class BenchmarkSet {
  static const int kNullBenchmarkId = -1;
 public:
  using Runner = std::function<void(BenchmarkSet&)>;

  BenchmarkSet(std::string&& name, Runner&& runner)
    : _name(std::move(name))
    , _runner(std::move(runner))
  {
  }

  int add(const Benchmark& benchmark) {
    int id = _benchmarks.size();
    auto was_inserted = _benchmarks.insert(benchmark);
    if(!was_inserted.second) {
      std::cerr << "bechmark " << benchmark.name() << " is not unique for benchmark "
                << _name << std::endl;
      std::abort();
    }
    was_inserted.first->set_id(id);
    return id;
  }

  void run() {
    assert(_runner && "runner is not set");
    assert(_num_trials > 0 && "num trials is not set");
    assert(_num_epochs > 0 && "num epochs is not set");

    // run through the benchmark set a single time
    // to invoke callbacks that add each individual
    // benchmark
    State::get_n() = _enumeration_range->begin(); //make sure we use a valid value of n
    _runner(*this);

    // repeatedly invoke the runner function until each benchmark 
    // has been run range_size x num_epochs x num_trials times. 
    // The global random number generator should be identically
    // seeded for every (range_value, trial_id) pair.
    std::random_device random_device;
    for(_epoch_id=0; _epoch_id<_num_epochs; ++_epoch_id) {
      _n        = _enumeration_range->begin();
      int last  = _enumeration_range->end();
      for(; _n < last; _n = _enumeration_range->advance(_n)) {
        State::get_n() = _n;
        auto random_seed = random_device();
        for(auto& benchmark : _benchmarks) {
          _current_benchmark = &benchmark;
          for(int trial_id=0; trial_id<_num_trials; ++trial_id) {
            State::get_random_number_generator().seed(random_seed);
            _runner(*this);
          }
        }
      }
    }
    _current_benchmark = nullptr;
  }

  void record_elapse(double elapse) {
    _current_benchmark->add_result(_n, _epoch_id, elapse);
  }

  void set_enumeration_range(std::shared_ptr<const EnumerationRange>&& enumeration_range) {
    _enumeration_range = std::move(enumeration_range);
  }

  void print_results() const {
    std::ofstream out(_name + ".tsv");
    print_header(out);

    int last = _enumeration_range->end();
    for(int n = _enumeration_range->begin()
      ; n<last
      ; n = _enumeration_range->advance(n))
    {
      print_row(n, out);
    }
    
  }

  const std::string& name() const { return _name; }
  void set_num_epochs(int num_epochs) { _num_epochs = num_epochs; }
  void set_num_trials(int num_trials) { _num_trials = num_trials; }
  int num_epochs() const { return _num_epochs; }
  int num_trials() const { return _num_trials; }
  int current_benchmark_id() const { 
    if(_current_benchmark)
      return _current_benchmark->id();
    else
      return kNullBenchmarkId;
  }
 private:
  void print_header(std::ostream& out) const {
    out << "n";
    for(const auto& benchmark : _benchmarks) {
      const auto& name = benchmark.name();
      out << "\t" << name << "_avg"
          << "\t" << name << "_worst";
    }
    out << "\n";
  }

  void print_row(int n, std::ostream& out) const {
    out << n;
    for(const auto& benchmark : _benchmarks) {
      print_benchmark_results(n, benchmark, out);
    }
    out << "\n";
  }

  void print_benchmark_results(int n
                             , const Benchmark& benchmark
                             , std::ostream& out) const 
  {
    const auto& epoch_trials = benchmark.get_epochs(n);
    std::vector<double> epochs;
    epochs.reserve(_num_epochs);

    // estimate the elapse time for each (range_value, epoch_id) pair. This
    // follows the practice of the facebooks folly library and uses the
    // minimum of the collection as the estimator.
    for(const auto& trials : epoch_trials) {
      assert(!trials.empty() && "the trial cannot be empty");
      double min = *std::min_element(trials.begin(), trials.end());
      epochs.push_back(min);
    }

    double avg   = std::accumulate(epochs.begin(), epochs.end(), 0) / epochs.size();
    double worst;
    if(epochs.size() > 1)
      worst =  *std::max_element(epochs.begin(), epochs.end());
    else
      worst = avg;

    out << "\t" << avg << "\t" << worst;
  }

  std::string _name;
  Runner _runner;
  int _num_epochs = 1;
  int _num_trials = 1;
  std::shared_ptr<const EnumerationRange> _enumeration_range = std::make_shared<EnumerationRange>();
  boost::container::flat_set<Benchmark> _benchmarks;

  int _n;
  int _epoch_id;
  Benchmark* _current_benchmark = nullptr;
};

inline
bool operator==(const BenchmarkSet& lhs, const BenchmarkSet& rhs) {
  return lhs.name() == rhs.name();
}

inline
bool operator<(const BenchmarkSet& lhs, const BenchmarkSet& rhs) {
  return lhs.name() < rhs.name();
}

} //end namespace
