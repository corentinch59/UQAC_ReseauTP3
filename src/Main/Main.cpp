#include <Main/Main.hpp>
#include <spdlog/spdlog.h>
#include <exception>

int DUOEntry(int argc, char* argv[], int(*mainFunc)(int argc, char* argv[]))
{
	try
	{
		return mainFunc(argc, argv);
	}
	catch (const std::exception& e)
	{
		spdlog::critical("unhandled exception: {0}", e.what());
		return EXIT_FAILURE;
	}
	catch (...)
	{
		spdlog::critical("unhandled non-standard exception");
		return EXIT_FAILURE;
	}
}