#include <touchstone/touchstone.h>
#include <vector>
#include <algorithm>
#include <random>

BENCHMARK_SET(
    "sort"
  , NumEpochs{10}
  , NumTrials{100}
  , LinearRange{10, 100}
) 
{
  int n = touchstone::n();
  std::vector<int> v(n);
  std::generate_n(v.begin(), n, touchstone::random_number_generator());
  BENCHMARK("unstable") {
    std::sort(v.begin(), v.end());
  }
  BENCHMARK("stable") {
    std::stable_sort(v.begin(), v.end());
  }
  touchstone::do_not_optimize_away(v[0]);
}
