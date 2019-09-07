#include "Application.h"

#include <stdio.h>
Application::~Application() {
    printf("Destructor \n");
}

void Application::OnInit() {
    printf("init \n");

}

void Application::OnUpdate() {
    printf("update \n");

}

void Application::OnRender() {
    printf("Render \n");

}

void Application::OnExit() {
    printf("Exit \n");

}

