#pragma once

#include <string>
#include <tuple>
#include <vector>
#include <boost/container/flat_map.hpp>
#include <iostream>

namespace touchstone {

class Benchmark {
  using TrialCollection = std::vector<double>;
  using EpochCollection  = std::vector<TrialCollection>;
 public:
  Benchmark(std::string&& name, int num_epochs)
    : _name(std::move(name))
    , _num_epochs(num_epochs)
  {}

  const std::string& name() const { return _name; }

  void add_result(int n, int epoch_id, double elapse) {
    auto epoch_collection_i = _results.find(n);
    if(epoch_collection_i == _results.end()) {
      epoch_collection_i = _results.emplace(n, EpochCollection(_num_epochs)).first;
    }
    epoch_collection_i->second[epoch_id].push_back(elapse);
  }

  const EpochCollection& get_epochs(int n) const {
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
  int _num_epochs;
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
