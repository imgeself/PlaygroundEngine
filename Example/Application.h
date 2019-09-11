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
    void OnUIRender() override;
    void OnExit() override;

private:
    PGSystem* m_System;
    std::shared_ptr<IIndexBuffer> m_IndexBuffer;

};
REGISTER_GAME_APPLICATION(Application)
