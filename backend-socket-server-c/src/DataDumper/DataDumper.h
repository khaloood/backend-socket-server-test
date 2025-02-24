#pragma once
#include <thread>
#include <atomic>
#include <fstream>
#include "../DataStorage/DataStorage.h"
#include "../LogConfig/LogConfig.h"
#include "../Benchmark/BenchmarkMacros.h"

constexpr int DUMP_INTERVAL = 10; // Dump every N seconds

class DataDumper {
public:
    explicit DataDumper(DataStorage& storage);
    ~DataDumper();

    void start();
    void stop();

private:
    void run();  // Loop that dumps data every N seconds

    DataStorage& dataStorage_;
    std::atomic<bool> running_{true};
    std::thread dumpThread_;
};
