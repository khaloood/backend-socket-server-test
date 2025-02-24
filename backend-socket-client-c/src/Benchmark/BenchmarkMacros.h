#pragma once
#include <chrono>
#include <spdlog/spdlog.h>

// ✅ Macro for Benchmarking Functions (Only When ENABLE_BENCHMARK is Defined)
#ifdef ENABLE_BENCHMARK
    #define BENCHMARK_START(name) \
        auto start_##name = std::chrono::high_resolution_clock::now();

    #define BENCHMARK_END(name) \
        auto end_##name = std::chrono::high_resolution_clock::now(); \
        auto duration_##name = std::chrono::duration_cast<std::chrono::nanoseconds>(end_##name - start_##name).count(); \
        spdlog::info("[{}] Execution time: {} nanoseconds", #name, duration_##name);
#else
    #define BENCHMARK_START(name)
    #define BENCHMARK_END(name)
#endif
