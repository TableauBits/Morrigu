#ifndef MRG_ENTRYPOINT
#define MRG_ENTRYPOINT

#include "Application.h"

extern MRG::Application* MRG::createApplication();

int main(int argc, char** argv)
{
	auto app = MRG::createApplication();
	app->run();
	delete app;
}

#endif