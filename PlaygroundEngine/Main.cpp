#include <stdio.h>

#include "PGSystem.h"
#include "PGApplication.h"

extern "C" {
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

int main(int argc, char** argv)
{
    SystemInitArguments initArguments = {};
    initArguments.executablePath = argv[0];
    initArguments.gameLibraryPath = argv[1];

    PGSystem system = PGSystem();
    system.InitializeSystem(&initArguments);

    system.RunMainLoop();

    return 0;
}
