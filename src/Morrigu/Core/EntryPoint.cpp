#ifndef MRG_ENTRYPOINT
#define MRG_ENTRYPOINT

#include "Core/Application.h"
#include "Core/Logger.h"
#include "Debug/Instrumentor.h"

extern MRG::Application* MRG::createApplication();

int main()
{
	MRG::Logger::init();
	MRG_ENGINE_INFO("Finished engine initialisation.");

	MRG_PROFILE_BEGIN_SESSION("Startup", "MRGProfile-Startup.json");
	auto app = MRG::createApplication();
	MRG_PROFILE_END_SESSION();

	MRG_PROFILE_BEGIN_SESSION("Runtime", "MRGProfile-Runtime.json");
	app->run();
	MRG_PROFILE_END_SESSION();

	MRG_PROFILE_BEGIN_SESSION("Shutdown", "MRGProfile-Shutdown.json");
	delete app;
	MRG_PROFILE_END_SESSION();
}

#endif