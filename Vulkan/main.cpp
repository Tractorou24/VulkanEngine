#include <iostream>
#include "Application.h"

int main(int argc, char* argv[])
{
	Engine::Application app;
	try
	{
		app.Run();
	}
	catch (const std::exception& execpt)
	{
		std::cerr << "Exception: " << execpt.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
