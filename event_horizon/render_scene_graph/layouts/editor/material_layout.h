//
// Created by Dado on 2018-10-29.
//

#pragma once
#include <render_scene_graph/layouts/layout_helper.hpp>

class ImGuiMaterials : public LayoutBoxRenderer {
public:
    virtual ~ImGuiMaterials() = default;
    using LayoutBoxRenderer::LayoutBoxRenderer;
protected:
    void renderImpl( SceneOrchestrator *scene, JMATH::Rect2f& f ) override;
};

void callbackMaterial( const std::string& _filename, const std::vector<char>& _data );