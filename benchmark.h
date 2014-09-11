#pragma once

#include <string>
#include <tuple>
#include <vector>
#include <boost/container/flat_map.hpp>
#include <iostream>

namespace touchstone {

class Benchmark {
  using ResultCollection = std::vector<double>;
  using EpochCollection  = boost::container::flat_map<int, ResultCollection>;
 public:
  Benchmark(std::string&& name)
    : _name(std::move(name))
  {}

  const auto& name() const { return _name; }

  void add_result(int n, int epoch_id, double elapse) {
    _results[n][epoch_id].push_back(elapse);
  }

  const auto& get_results(int n) const {
    auto i = _results.find(n);
    if(i == _results.end()) {
      std::cerr << "no results found for n = " << n << std::endl;
      std::abort();
    }
    return i->second;
  }

  void set_id(int id) { _id = id; }
  int id() const { return _id; }
 private:
  int _id;
  std::string _set_name;
  std::string _name;
  boost::container::flat_map<int, EpochCollection> _results;
};

inline
bool operator==(const Benchmark& lhs, const Benchmark& rhs) {
  return lhs.name() == rhs.name();
}

inline
bool operator<(const Benchmark& lhs, const Benchmark& rhs) {
  return lhs.name() < rhs.name();
}

} //end namespace
