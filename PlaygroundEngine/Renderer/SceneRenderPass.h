#pragma once

#include "HWRendererAPI.h"
#include "PGRenderList.h"

#include "BaseRenderPass.h"
#include "../PGProfiler.h"

class SceneRenderPass : public RenderPass {
public:
    virtual ~SceneRenderPass() = default;

    virtual void Execute(HWRendererAPI* rendererAPI, const RenderList& renderList, SceneRenderPassType scenePassType);
};
