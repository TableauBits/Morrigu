#ifndef MRG_ENTRYPOINT
#define MRG_ENTRYPOINT

#include "Application.h"
#include "Logger.h"

extern MRG::Application* MRG::createApplication();

int main(int argc, char** argv)
{
	MRG::Logger::init("APP");
	MRG_ENGINE_INFO("Finished engine initialisation.");

	auto app = MRG::createApplication();
	MRG_INFO("Finished client application initialisation.");
	app->run();
	delete app;
}

#endif