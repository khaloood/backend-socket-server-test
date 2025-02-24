

#include "DataStorage.h"
#include "../LogConfig/LogConfig.h"
#include <fstream>
#include <numeric>

// ---------------- Constructor ----------------
DataStorage::DataStorage()
{
    spdlog::info("DataStorage initialized with {} elements.", BITSET_SIZE);
}

// ---------------- Add Number ----------------
void DataStorage::addNumber(uint16_t number)
{
    BENCHMARK_START(AddNumber);

    if (number >= NUM_BITS)
    {
        BENCHMARK_END(AddNumber);
        return;
    }

    int index = number / 32;
    int bit = number % 32;

    // ✅ Atomic OR operation to avoid locking
    bitset_[index].fetch_or((1 << bit), std::memory_order_relaxed);

    // ✅ Non-blocking push to queue
    queue_.push(number);

    BENCHMARK_END(AddNumber);
}

// ---------------- Check if Number Exists ----------------
bool DataStorage::isNumberPresent(uint16_t number) const
{
    BENCHMARK_START(CheckNumberExists);

    int index = number / 32;
    int bit = number % 32;
    bool exists = (bitset_[index].load(std::memory_order_relaxed) & (1 << bit)) != 0;

    BENCHMARK_END(CheckNumberExists);
    return exists;
}

// ✅ No benchmark logging for this function (to use in performance-sensitive sections)
bool DataStorage::isNumberPresent_(uint16_t number) const
{
    int index = number / 32;
    int bit = number % 32;
    return (bitset_[index].load(std::memory_order_relaxed) & (1 << bit)) != 0;
}

// ---------------- Process Number ----------------
double DataStorage::processNumber(uint16_t number)
{
    BENCHMARK_START(ProcessNumber);

    spdlog::info("Processing number: {}", number);

    // ✅ Step 1: Early exit if the number is already present
    if (isNumberPresent_(number)) // Avoids logging overhead
    {
        double cachedAvg = lastComputedAvg_.load(std::memory_order_relaxed);
        spdlog::info("Number {} already exists. Returning cached average: {:.2f}", number, cachedAvg);
        BENCHMARK_END(ProcessNumber);
        return cachedAvg;
    }

    // ✅ Step 2: Add the number to storage
    addNumber(number);
    spdlog::info("Added number {} to storage.", number);

    // ✅ Step 3: Atomic updates for calculations (using `compare_exchange_weak`)
    double square = static_cast<double>(number * number);

    double prevSum = sumOfSquares_.load(std::memory_order_relaxed);
    double newSum;
    do {
        newSum = prevSum + square;
    } while (!sumOfSquares_.compare_exchange_weak(prevSum, newSum, std::memory_order_relaxed));

    int prevCount = totalCount_.fetch_add(1, std::memory_order_relaxed) + 1;

    // ✅ Step 4: Compute new average and update cache
    double newAvg = newSum / prevCount;
    
    double cachedAvg = lastComputedAvg_.load(std::memory_order_relaxed);
    while (!lastComputedAvg_.compare_exchange_weak(cachedAvg, newAvg, std::memory_order_relaxed));

    spdlog::info("Updated average square: {:.2f}", newAvg);

    BENCHMARK_END(ProcessNumber);
    return newAvg;
}


// ---------------- Get Bitset Value ----------------
uint32_t DataStorage::getBitsetValue(int index) const
{
    BENCHMARK_START(GetBitsetValue);
    uint32_t value = bitset_[index].load(std::memory_order_relaxed);
    BENCHMARK_END(GetBitsetValue);
    return value;
}
