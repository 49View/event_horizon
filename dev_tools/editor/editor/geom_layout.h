//
// Created by Dado on 2018-10-29.
//

#pragma once

#include <render_scene_graph/layouts/layout_helper.hpp>

void callbackGeom( const std::string& _filename, const SerializableContainer& _data );
void callbackGeomGLTF( const std::string& _filename );
void callbackGeomSVG( const std::string& _filename, const std::string& svgString );

class ImGuiGeoms : public LayoutBoxRenderer {
public:
    virtual ~ImGuiGeoms() = default;
    using LayoutBoxRenderer::LayoutBoxRenderer;
protected:
    void renderImpl( SceneGraph& _sg, RenderOrchestrator& _rsg, JMATH::Rect2f& f ) override;
};


