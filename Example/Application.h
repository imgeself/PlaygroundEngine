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
    void OnUIRender() override;
    void OnExit() override;

private:
    PGSystem* m_System;

    std::shared_ptr<IIndexBuffer> m_IndexBuffer;
    std::shared_ptr<IVertexBuffer> m_VertexBuffer;

    std::shared_ptr<IShaderProgram> m_CubeShader;
    std::shared_ptr<IShaderProgram> m_LightCubeShader;

    Mesh* m_CubeMesh;
    Mesh* m_LightCubeMesh;

    PGScene m_Scene;

};
REGISTER_GAME_APPLICATION(Application)
