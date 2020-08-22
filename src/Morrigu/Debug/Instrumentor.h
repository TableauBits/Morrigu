#ifndef MRG_INSTRUMENTOR
#define MRG_INSTRUMENTOR

#include "Core/Core.h"

#include <fmt/core.h>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <string>
#include <thread>

namespace MRG
{
	struct ProfileResult
	{
		std::string name;
		long long start, end;
		std::size_t tid;
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
		Instrumentor() : m_currentSession(nullptr), m_profileCount(0) {}

		void beginSession(const std::string& name, const std::string& filepath)
		{
			m_outputStream.open(filepath);
			writePrologue();
			m_currentSession = new InstrumentationSession{name};
		}

		void endSession()
		{
			writeEpilogue();
			m_outputStream.close();
			delete m_currentSession;
			m_currentSession = nullptr;
			m_profileCount = 0;
		}

		void writeProfile(const ProfileResult& result)
		{
			if (m_profileCount++ > 0)
				m_outputStream << ',';

			// escaping names to be JSON compliant
			std::string name = result.name;
			std::replace(name.begin(), name.end(), '"', '\'');

			m_outputStream << '{';
			m_outputStream << R"("cat":"function",)";
			m_outputStream << fmt::format(R"("dur":{},)", result.end - result.start);
			m_outputStream << fmt::format(R"("name":"{}",)", name);
			m_outputStream << R"("ph":"X",)";
			m_outputStream << R"("pid":0,)";
			m_outputStream << fmt::format(R"("tid":{},)", result.tid);
			m_outputStream << fmt::format(R"("ts":{})", result.start);
			m_outputStream << '}';

			m_outputStream.flush();
		}

		void writePrologue()
		{
			m_outputStream << R"({"otherData": {},"traceEvents":[)";
			m_outputStream.flush();
		}

		void writeEpilogue()
		{
			m_outputStream << "]}";
			m_outputStream.flush();
		}

		static Instrumentor& get()
		{
			static Instrumentor instance;
			return instance;
		}

	private:
		InstrumentationSession* m_currentSession;
		std::ofstream m_outputStream;
		int m_profileCount;
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

			auto tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
			Instrumentor::get().writeProfile({m_name, start, end, tid});

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