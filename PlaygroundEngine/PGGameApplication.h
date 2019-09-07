#pragma once

#include "Core.h"
#include "PGApplication.h"

typedef IApplication* (*GameApplicationProc)();
#define REGISTER_GAME_APPLICATION(classname) extern "C" DLL_EXPORT IApplication* GetGameApplication() { return new classname(); }


