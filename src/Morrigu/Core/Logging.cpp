//
// Created by mathi on 2021-04-06.
//

#include "Logging.h"

#include "Core.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <vector>

namespace MRG
{
    std::shared_ptr<spdlog::logger> Logger::s_engineLogger;
    std::shared_ptr<spdlog::logger> Logger::s_clientLogger;

    void Logger::init()
    {
        std::vector<spdlog::sink_ptr> logSinks;
        logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Morrigu.log", true));

        logSinks[0]->set_pattern("%^[%T] %n: %v%$");
        logSinks[1]->set_pattern("[%T] [%l] %n: %v");

#ifdef MRG_DEBUG
        const auto level = spdlog::level::trace;
#else
        const auto level = spdlog::level::info;
#endif

        s_engineLogger = std::make_shared<spdlog::logger>("MORRIGU", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_engineLogger);
        s_engineLogger->set_level(level);
        s_engineLogger->flush_on(level);

        s_clientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_clientLogger);
        s_clientLogger->set_level(level);
        s_clientLogger->flush_on(level);
    }
}  // namespace MRG