#pragma once

#ifdef _WIN32
	#ifdef DBGAME_EXPORT
		#define DBGAME_API __declspec(dllexport)
	#else
		#define DBGAME_API __declspec(dllimport)
	#endif
#else
	#ifdef DB_IGNORE_DLLEXPORT
		#define DBGAME_API __attribute__((visibility("default")))
	#else
		#define DBGAME_API __attribute__((visibility("default")))
	#endif
#endif