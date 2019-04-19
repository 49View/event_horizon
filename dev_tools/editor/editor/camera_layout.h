//
// Created by Dado on 2018-10-29.
//

#pragma once

#include <render_scene_graph/layouts/layout_helper.hpp>

class ImGuiCamera : public LayoutBoxRenderer {
public:
    virtual ~ImGuiCamera() = default;
    using LayoutBoxRenderer::LayoutBoxRenderer;
protected:
    void renderImpl( SceneOrchestrator *scene, JMATH::Rect2f& f ) override;
};
