#include <touchstone/touchstone.h>
#include <vector>
#include <algorithm>
#include <deque>
#include <random>

BENCHMARK_SET(
    "nth_element"
  , NumEpochs{10}
  , NumTrials{100}
  , ExpoentialRange{1, 10, 2}
) 
{
  int n    = touchstone::n();
  auto rng = touchstone::random_number_generator();
  std::vector<int> v(n);
  std::deque<int> d(n);
  std::generate_n(v.begin(), n, rng);
  std::copy_n(v.begin(), n, d.begin());
  std::uniform_int_distribution<> dist(0, n-1);
  int i = dist(rng);
  int x;
  BENCHMARK("deque") {
    auto first = d.begin();
    auto ith   = d.begin() + i;
    auto last  = d.end();
    std::nth_element(first, ith, last);
    x = *ith;
  }
  BENCHMARK("vector") {
    auto first = v.begin();
    auto ith   = v.begin() + i;
    auto last  = v.end();
    std::nth_element(first, ith, last);
    x = *ith;
  }
  touchstone::do_not_optimize_away(x);
}

