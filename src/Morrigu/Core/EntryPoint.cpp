//
// Created by Mathis Lamidey on 2021-04-03.
//

#include "Core/Application.h"

int main()
{
	MRG::Logger::init();
	const auto application = MRG::createApplication();

	application->init();
	application->run();
	application->cleanup();

	delete application;
}
