#Touchstone

A small C++ benchmarking library that lets you create benchmarks in a style similar
to the [Catch](https://github.com/philsquared/Catch/) testing framework.

###Quickstart

Suppose you want to compare the STL functions ```std::sort``` and ```std::stable_sort```.
Write
```C++
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
    // v will be unsorted when this benchmark section is run
    // because benchmark sections are run excusively
    std::stable_sort(v.begin(), v.end());
  }  
  touchstone::do_not_optimize_away(v[0]);
}
```
Touchstone calls the benchmarking function across the range of values ```n=10``` to ```n=100```. 
For each value of ```n```, the function is called ```NumEpochs * NumTrials``` times. Every epoch 
is estimated ```NumTrials``` times, each time using a random number generator seeded with
the same value. As with [Catch](https://github.com/philsquared/Catch/), only a single ```BENCHMARK```
section is executed every time the function is run, eliminating the need for fixtures.

Results are written to a tab-separated file, which can be easily plotted.
![Alt text](/examples/sort_benchmark.png?raw=true)
