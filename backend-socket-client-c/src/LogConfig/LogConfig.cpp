 #include "LogConfig.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

void setup_logging()
{
    // Create console sink
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::info);

    // Create file sink
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("app_logs.txt", true);
    file_sink->set_level(spdlog::level::info);

    // Create a logger with both sinks
    auto logger = std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list{console_sink, file_sink});
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%L%$] %v");
    spdlog::flush_on(spdlog::level::info);
}
