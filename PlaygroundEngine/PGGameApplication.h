#pragma once

#include "Core.h"
#include "PGApplication.h"
#include "PGSystem.h"

typedef IApplication* (*GameApplicationProc)(PGSystem* system);
#define REGISTER_GAME_APPLICATION(classname) extern "C" DLL_EXPORT IApplication* GetGameApplication(PGSystem* system) { return new classname(system); }


