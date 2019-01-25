//
// Created by Dado on 2018-10-29.
//

#pragma once

#include <render_scene_graph/layouts/layout_helper.hpp>

class ImGuiTimeline : public LayoutBoxRenderer {
public:
    virtual ~ImGuiTimeline() = default;
    using LayoutBoxRenderer::LayoutBoxRenderer;
protected:
    void renderImpl( Scene *scene, JMATH::Rect2f& f ) override;
};

namespace GuiTimeline {
    std::string TimeLineName();
    float CurrentTime();
}
