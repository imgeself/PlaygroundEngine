#include <stdio.h>
#include "PGSystem.h"

int main()
{
    printf("Hello World!\n");
    
    PGSystem system = PGSystem();
    system.InitializeSystem();

    system.RunMainLoop();

    return 0;
}
