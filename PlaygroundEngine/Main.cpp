#include <stdio.h>

#include "PGSystem.h"
#include "PGApplication.h"

int main(int argc, char** argv)
{
    printf("Hello World!\n");

    SystemInitArguments initArguments = {};
    initArguments.executablePath = argv[0];
    initArguments.gameLibraryPath = argv[1];

    PGSystem system = PGSystem();
    system.InitializeSystem(&initArguments);

    system.RunMainLoop();

    return 0;
}
