//
// Created by Dado on 2018-10-29.
//

#pragma once

#include <render_scene_graph/layouts/layout_helper.hpp>

class ImGuiCloudEntitiesGeom : public LayoutBoxRenderer {
public:
    virtual ~ImGuiCloudEntitiesGeom() = default;
    using LayoutBoxRenderer::LayoutBoxRenderer;
protected:
    void renderImpl( Scene *scene, JMATH::Rect2f& f ) override;
};
