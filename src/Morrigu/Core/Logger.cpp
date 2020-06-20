#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace MRG
{
	std::shared_ptr<spdlog::logger> Logger::s_engineLogger;
	std::shared_ptr<spdlog::logger> Logger::s_clientLogger;

	void Logger::init(const std::string& loggerName)
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_engineLogger = spdlog::stdout_color_mt("MORRIGU");
		s_clientLogger = spdlog::stdout_color_mt(loggerName);

		s_engineLogger->set_level(spdlog::level::trace);
		s_clientLogger->set_level(spdlog::level::trace);
	}
}  // namespace MRG