#ifndef MRG_CLASS_LOGGER
#define MRG_CLASS_LOGGER

#include <spdlog/spdlog.h>

#include <memory>

namespace MRG
{
	class Logger
	{
	public:
		static void init();

		[[nodiscard]] static std::shared_ptr<spdlog::logger>& getEngineLogger() { return s_engineLogger; };
		[[nodiscard]] static std::shared_ptr<spdlog::logger>& getClientLogger() { return s_clientLogger; };

	private:
		static std::shared_ptr<spdlog::logger> s_engineLogger;
		static std::shared_ptr<spdlog::logger> s_clientLogger;
	};

}  // namespace MRG

// clang-format off
#define MRG_ENGINE_TRACE(...)   ::MRG::Logger::getEngineLogger()->trace(__VA_ARGS__)
#define MRG_ENGINE_INFO(...)    ::MRG::Logger::getEngineLogger()->info(__VA_ARGS__)
#define MRG_ENGINE_WARN(...)    ::MRG::Logger::getEngineLogger()->warn(__VA_ARGS__)
#define MRG_ENGINE_ERROR(...)   ::MRG::Logger::getEngineLogger()->error(__VA_ARGS__)
#define MRG_ENGINE_FATAL(...)   ::MRG::Logger::getEngineLogger()->critical(__VA_ARGS__)

#define MRG_TRACE(...)          ::MRG::Logger::getClientLogger()->trace(__VA_ARGS__)
#define MRG_INFO(...)           ::MRG::Logger::getClientLogger()->info(__VA_ARGS__)
#define MRG_WARN(...)           ::MRG::Logger::getClientLogger()->warn(__VA_ARGS__)
#define MRG_ERROR(...)          ::MRG::Logger::getClientLogger()->error(__VA_ARGS__)
#define MRG_FATAL(...)          ::MRG::Logger::getClientLogger()->critical(__VA_ARGS__)
// clang-format on

#endif
