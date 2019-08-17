#pragma once

#include "PGWindow.h"

class PGSystem {
public:
    bool InitializeSystem();
    void RunMainLoop();

private:
    PGWindow* m_Window;
};

