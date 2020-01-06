#pragma once

#include <PGSystem.h>
#include <PGGameApplication.h>
#include <PGCamera.h>

class Application : public IApplication {
public:
    Application(PGSystem* system);
    ~Application() override;

    virtual void OnSystemEvent(SystemEvent event, uint64_t param1, uint64_t param2) override;

    void OnInit() override;
    void OnUpdate(float deltaTime) override;
    void OnUIRender() override;
    void OnExit() override;

private:
    PGSystem* m_System;
    PGCamera m_MainCamera;


    std::shared_ptr<HWIndexBuffer> m_IndexBuffer;
    std::shared_ptr<HWVertexBuffer> m_VertexBuffer;

    Material* m_DefaultMaterial;

    ShaderRef m_PBRShader;

    PGScene m_Scene;

};
REGISTER_GAME_APPLICATION(Application)
