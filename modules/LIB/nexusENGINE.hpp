#pragma once

#ifdef NEXUSENGINE_EXPORTS
#define NEXUSENGINE_API __declspec(dllexport)
#else
#define NEXUSENGINE_API __declspec(dllimport)
#endif

extern "C" NEXUSENGINE_API void init();
