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
    void renderImpl( SceneGraph& _sg, RenderOrchestrator& _rsg, JMATH::Rect2f& f ) override;
};

void callbackMaterial( const std::string& _filename, const SerializableContainer& _data );