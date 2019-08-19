#pragma once

#include "Platform/PGWindow.h"

class PGSystem {
public:
    bool InitializeSystem();
    void RunMainLoop();

private:
    PGWindow* m_Window;
};

