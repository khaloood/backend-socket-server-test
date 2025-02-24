#include "DataDumper.h"

DataDumper::DataDumper(DataStorage &storage) : dataStorage_(storage) {}

DataDumper::~DataDumper()
{
    stop();
}

void DataDumper::start()
{
    running_ = true;
    dumpThread_ = std::thread(&DataDumper::run, this);
}

void DataDumper::stop()
{
    running_ = false;
    if (dumpThread_.joinable())
    {
        dumpThread_.join();
    }
}
void DataDumper::run()
{
    while (running_)
    {
        for (int i = 0; i < DUMP_INTERVAL ; ++i)
        { 
            if (!running_)
                return;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        if (!running_)
            return;

        BENCHMARK_START(DumpToFile);

        std::ofstream file("numbers_dump.bin", std::ios::binary | std::ios::trunc);
        if (!file)
        {
            spdlog::error("Error: Unable to open file for dumping.");
            continue;
        }

        for (int i = 0; i < NUM_BITS; ++i)
        {
            if (dataStorage_.isNumberPresent_(i))
            {
                file.write(reinterpret_cast<const char *>(&i), sizeof(i));
            }
        }

        file.close();
        BENCHMARK_END(DumpToFile);
    }
}
