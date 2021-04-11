//
// Created by mathi on 2021-04-06.
//

#ifndef MORRIGU_LOGGING_H
#define MORRIGU_LOGGING_H

#include <spdlog/spdlog.h>

#include <memory>

namespace MRG
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename... Args>
	[[nodiscard]] constexpr Scope<T> createScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename... Args>
	[[nodiscard]] constexpr Ref<T> createRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	class Logger
	{
	public:
		static void init();

		[[nodiscard]] static auto& getEngineLogger() { return s_engineLogger; };
		[[nodiscard]] static auto& getClientLogger() { return s_clientLogger; };

	private:
		static Ref<spdlog::logger> s_engineLogger;
		static Ref<spdlog::logger> s_clientLogger;
	};

}  // namespace MRG

// clang-format off
#define MRG_ENGINE_TRACE(...)   ::MRG::Logger::getEngineLogger()->trace(__VA_ARGS__);
#define MRG_ENGINE_INFO(...)    ::MRG::Logger::getEngineLogger()->info(__VA_ARGS__);
#define MRG_ENGINE_WARN(...)    ::MRG::Logger::getEngineLogger()->warn(__VA_ARGS__);
#define MRG_ENGINE_ERROR(...)   ::MRG::Logger::getEngineLogger()->error(__VA_ARGS__);
#define MRG_ENGINE_FATAL(...)   ::MRG::Logger::getEngineLogger()->critical(__VA_ARGS__);

#define MRG_TRACE(...)          ::MRG::Logger::getClientLogger()->trace(__VA_ARGS__);
#define MRG_INFO(...)           ::MRG::Logger::getClientLogger()->info(__VA_ARGS__);
#define MRG_WARN(...)           ::MRG::Logger::getClientLogger()->warn(__VA_ARGS__);
#define MRG_ERROR(...)          ::MRG::Logger::getClientLogger()->error(__VA_ARGS__);
#define MRG_FATAL(...)          ::MRG::Logger::getClientLogger()->critical(__VA_ARGS__);
// clang-format on

#endif  // MORRIGU_LOGGING_H
