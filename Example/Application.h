#pragma once

#include <PGSystem.h>
#include <PGGameApplication.h>

class Application : public IApplication {
public:
    Application(PGSystem* system);
    ~Application() override;

    void OnInit() override;
    void OnUpdate() override;
    void OnRender() override;
    void OnExit() override;

private:
    PGSystem* m_System;

};
REGISTER_GAME_APPLICATION(Application)
