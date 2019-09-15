#pragma once

#include <PGSystem.h>
#include <PGGameApplication.h>
#include <PGCamera.h>

class Application : public IApplication {
public:
    Application(PGSystem* system);
    ~Application() override;

    void OnInit() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnUIRender() override;
    void OnExit() override;

private:
    PGSystem* m_System;

    std::shared_ptr<IIndexBuffer> m_IndexBuffer;
    std::shared_ptr<IVertexBuffer> m_VertexBuffer;

    PGCamera m_Camera;

};
REGISTER_GAME_APPLICATION(Application)
