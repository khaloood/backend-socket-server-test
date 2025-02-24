

#pragma once
#include <atomic>
#include <boost/lockfree/queue.hpp>
#include <vector>
#include "../Benchmark/BenchmarkMacros.h"

#define NUM_BITS 1024
#define BITSET_SIZE (NUM_BITS / 32) // 1024 / 32 = 32 elements

class DataStorage
{
public:
    DataStorage();

    void addNumber(uint16_t number);
    bool isNumberPresent(uint16_t number) const;
    bool isNumberPresent_(uint16_t number) const;

    double calculateAverageSquare() const;
    uint32_t getBitsetValue(int index) const;
    double processNumber(uint16_t number);

private:
    std::atomic<uint32_t> bitset_[BITSET_SIZE]{}; // Lock-free bitset using atomic operations
    boost::lockfree::queue<int> queue_{128};      // Lock-free queue for batch processing
    std::atomic<double> lastComputedAvg_{0.0};    // Cached average for fast access
    std::atomic<double> sumOfSquares_{0.0};       // Sum of squares (for calculating avg)
    std::atomic<int> totalCount_{0};              // Count of numbers stored
};
