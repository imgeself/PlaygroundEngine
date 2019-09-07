#include "Application.h"

#include <stdio.h>

Application::Application(PGSystem* system) 
    : m_System(system) {
}

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
    const float color[] = { 1.0f, 0.0f, 1.0f, 1.0f };
    m_System->GetRendererApi()->ClearScreen(color);
}

void Application::OnExit() {
    printf("Exit \n");

}

