#ifndef MRG_INSTRUMENTOR
#define MRG_INSTRUMENTOR

#include "Core/Core.h"
#include "Core/Logger.h"

#include <fmt/core.h>
#include <fmt/ostream.h>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>

namespace MRG
{
	using FloatMs = std::chrono::duration<double, std::micro>;

	struct ProfileResult
	{
		std::string name;

		FloatMs start;
		std::chrono::microseconds elapsedTime;
		std::thread::id tid;
	};

	// will be expanded in the future (hopefully)
	struct InstrumentationSession
	{
		std::string name;
	};

	// this is using the chrome tracing format. More info here for example:
	// https://www.gamasutra.com/view/news/176420/Indepth_Using_Chrometracing_to_view_your_inline_profiling_data.php
	class Instrumentor
	{
	public:
		void beginSession(const std::string& name, const std::string& filepath)
		{
			std::lock_guard lock(m_mutex);
			if (m_currentSession) {
				if (Logger::getEngineLogger()) {
					MRG_ENGINE_ERROR(
					  "Instrumentor::beginSession('{}') called when session '{}' was already active!", name, m_currentSession->name);
				}
				internalEndSession();
			}
			m_outputStream.open(filepath);

			if (m_outputStream.is_open()) {
				writePrologue();
				m_currentSession = new InstrumentationSession{name};
			} else {
				if (Logger::getEngineLogger()) {
					MRG_ENGINE_ERROR("Instrumentor could not open file '{}'", filepath);
				}
			}
		}

		void endSession()
		{
			std::lock_guard lock(m_mutex);
			internalEndSession();
		}

		void writeProfile(const ProfileResult& result)
		{
			std::stringstream json;

			// escaping names to be JSON compliant
			std::string name = result.name;
			std::replace(name.begin(), name.end(), '"', '\'');

			json << std::setprecision(3) << std::fixed;
			json << ",{";
			json << R"("cat":"function",)";
			json << R"("dur":)" << result.elapsedTime.count() << ",";
			json << fmt::format(R"("name":"{}",)", name);
			json << R"("ph":"X",)";
			json << R"("pid":0,)";
			json << fmt::format(R"("tid":{},)", result.tid);
			json << R"("ts":)" << result.start.count();
			json << '}';

			std::lock_guard lock(m_mutex);
			if (m_currentSession) {
				m_outputStream << json.str();
				m_outputStream.flush();
			}
		}

		static Instrumentor& get()
		{
			static Instrumentor instance;
			return instance;
		}

	private:
		void writePrologue()
		{
			m_outputStream << R"({"otherData": {},"traceEvents":[{})";
			m_outputStream.flush();
		}

		void writeEpilogue()
		{
			m_outputStream << "]}";
			m_outputStream.flush();
		}

		void internalEndSession()
		{
			if (m_currentSession) {
				writeEpilogue();
				m_outputStream.close();
				delete m_currentSession;
				m_currentSession = nullptr;
			}
		}

		std::mutex m_mutex;
		InstrumentationSession* m_currentSession = nullptr;
		std::ofstream m_outputStream;
	};

	class InstrumentationTimer
	{
	public:
		// clang-format off
		InstrumentationTimer(const char* name) : m_name(name), m_stopped(false)
		{
			m_startTimePoint = std::chrono::steady_clock::now();
		}
		// clang-format on

		~InstrumentationTimer()
		{
			if (!m_stopped)
				stop();
		}

		void stop()
		{
			auto endTimePoint = std::chrono::steady_clock::now();

			auto start = FloatMs{m_startTimePoint.time_since_epoch()};
			auto elapsed = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch() -
			               std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimePoint).time_since_epoch();

			Instrumentor::get().writeProfile({m_name, start, elapsed, std::this_thread::get_id()});

			m_stopped = true;
		}

	private:
		const char* m_name;
		std::chrono::time_point<std::chrono::steady_clock> m_startTimePoint;
		bool m_stopped;
	};
}  // namespace MRG

// clang-format off
#define MRG_PROFILING 1
#if MRG_PROFILING
	#define MRG_PROFILE_BEGIN_SESSION(name, filepath) ::MRG::Instrumentor::get().beginSession(name, filepath)
	#define MRG_PROFILE_END_SESSION() ::MRG::Instrumentor::get().endSession()
	#define MRG_PROFILE_SCOPE(name) ::MRG::InstrumentationTimer MRG_PREPOC_EVALUATOR(timer,__LINE__)(name); // we need this workaround to uniquely define timers
	#define MRG_PROFILE_FUNCTION() MRG_PROFILE_SCOPE(MRG_FUNCSIG)
#else
	#define MRG_PROFILE_BEGIN_SESSION(name, filepath)
	#define MRG_PROFILE_END_SESSION()
	#define MRG_PROFILE_SCOPE(name)
	#define MRG_PROFILE_FUNCTION()
#endif
// clang-format on

#endif