#include "Logger.h"

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

		s_engineLogger = std::make_shared<spdlog::logger>("MORRIGU", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_engineLogger);
#ifdef MRG_DEBUG
		s_engineLogger->set_level(spdlog::level::trace);
		s_engineLogger->flush_on(spdlog::level::trace);
#else
		s_engineLogger->set_level(spdlog::level::info);
		s_engineLogger->flush_on(spdlog::level::info);
#endif

		s_clientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_clientLogger);
#ifdef MRG_DEBUG
		s_clientLogger->set_level(spdlog::level::trace);
		s_clientLogger->flush_on(spdlog::level::trace);
#else
		s_clientLogger->set_level(spdlog::level::info);
		s_clientLogger->flush_on(spdlog::level::info);
#endif
	}
}  // namespace MRG