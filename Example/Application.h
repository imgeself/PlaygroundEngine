#pragma once

#include <PGGameApplication.h>

class Application : public IApplication {
public:
    ~Application() override;

    void OnInit() override;
    void OnUpdate() override;
    void OnRender() override;
    void OnExit() override;

};
REGISTER_GAME_APPLICATION(Application)
