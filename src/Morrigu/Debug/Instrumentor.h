#ifndef MRG_INSTRUMENTOR
#define MRG_INSTRUMENTOR

#include "Core/Core.h"
#include "Core/Logger.h"

#include <fmt/core.h>
#include <fmt/ostream.h>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>

namespace MRG
{
	struct ProfileResult
	{
		std::string name;
		long long start, end;
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

			json << ",{";
			json << R"("cat":"function",)";
			json << fmt::format(R"("dur":{},)", result.end - result.start);
			json << fmt::format(R"("name":"{}",)", name);
			json << R"("ph":"X",)";
			json << R"("pid":0,)";
			json << fmt::format(R"("tid":{},)", result.tid);
			json << fmt::format(R"("ts":{})", result.start);
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
		InstrumentationTimer(const char* name) : m_name(name), m_stopped(false)
		{
			m_startTimePoint = std::chrono::high_resolution_clock::now();
		}

		~InstrumentationTimer()
		{
			if (!m_stopped)
				stop();
		}

		void stop()
		{
			auto endTimePoint = std::chrono::high_resolution_clock::now();

			auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimePoint).time_since_epoch().count();
			auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

			Instrumentor::get().writeProfile({m_name, start, end, std::this_thread::get_id()});

			m_stopped = true;
		}

	private:
		const char* m_name;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimePoint;
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