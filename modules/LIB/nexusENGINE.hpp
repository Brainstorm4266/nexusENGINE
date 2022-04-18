#pragma once
#ifndef UNICODE
	#define UNICODE
#endif
#ifdef UNICODE
	#ifndef _UNICODE
		#define _UNICODE
	#endif
#endif
#include "win.hpp"
#include "nexusObject.hpp"
#include "windowprototype.hpp"
#ifdef NEXUSENGINE_EXPORTS
#define NEXUSENGINE_API __declspec(dllexport)
#else
#define NEXUSENGINE_API __declspec(dllimport)
#endif
extern "C" NEXUSENGINE_API void init();
#ifndef RUN_NEXUSENGINE_AS_MAIN
#error API not finished.
#endif