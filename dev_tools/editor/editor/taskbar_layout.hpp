//
// Created by Dado on 2019-01-23.
//

#pragma once

#include <render_scene_graph/layouts/layout_helper.hpp>

class ImGuiTaskbar : public LayoutBoxRenderer {
public:
    virtual ~ImGuiTaskbar() = default;
    using LayoutBoxRenderer::LayoutBoxRenderer;
protected:
    void renderImpl( SceneGraph& _sg, RenderOrchestrator& _rsg, JMATH::Rect2f& f ) override;
};


