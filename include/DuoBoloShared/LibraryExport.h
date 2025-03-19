#pragma once

#ifdef _WIN32
	#ifdef DB_IGNORE_DLLEXPORT
		#define DBGAME_API
	#else
		#define DBGAME_API __declspec(dllexport)
	#endif
#else
	#ifdef DB_IGNORE_DLLEXPORT
		#define DBGAME_API
	#else
		#define DBGAME_API __attribute__((visibility("default")))
	#endif
#endif