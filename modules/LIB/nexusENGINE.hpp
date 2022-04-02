#pragma once
#ifndef __NEXUS_ENGINE_IMPORTED__
#define __NEXUS_ENGINE_IMPORTED__
#include "nexusObject.hpp"
#include "windowprototype.hpp"
#ifdef NEXUSENGINE_EXPORTS
#define NEXUSENGINE_API __declspec(dllexport)
#else
#define NEXUSENGINE_API __declspec(dllimport)
#endif
extern "C" NEXUSENGINE_API void init();
#endif
#ifdef NDEBUG
#error API not finished.
#endif