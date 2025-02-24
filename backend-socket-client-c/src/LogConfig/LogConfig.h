#pragma once
#include <spdlog/spdlog.h> // Ensure spdlog is included

// Declare the setup_logging function in the global namespace
void setup_logging();

// Logging macros with variadic arguments
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)