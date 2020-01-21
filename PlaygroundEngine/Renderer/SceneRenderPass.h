#pragma once

#include "HWRendererAPI.h"
#include "PGRenderObject.h"

#include "BaseRenderPass.h"
#include "../PGProfiler.h"

class SceneRenderPass : public RenderPass {
public:
    virtual ~SceneRenderPass() = default;

    void SetRenderObjects(const std::vector<PGRenderObject*>& renderObjects);

    virtual void Execute(HWRendererAPI* rendererAPI) override;
protected:
    std::vector<PGRenderObject*> m_RenderObjects;
};
