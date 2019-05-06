//
// Created by Dado on 2019-01-24.
//

#include "layout_helper.hpp"

#include <core/math/rect2f.h>
#include <core/util.h>
#include <graphics/imgui/imgui.h>

bool LayoutBoxRenderer::startRender(JMATH::Rect2f& _r, BoxFlagsT _flags) {
    ImGui::SetNextWindowPos( ImVec2{ _r.origin().x(), _r.origin().y() } );
    ImGui::SetNextWindowSize( ImVec2{ _r.size().x(), _r.size().y() } );
    if ( bVisible ) {
        if ( !ImGui::Begin( name.c_str(), &bVisible, ImGuiWindowFlags_NoResize )) {
            ImGui::End();
            return false;
        }
    } else {
        return false;
    }

    return true;
}

void LayoutBoxRenderer::endRender(JMATH::Rect2f& _r) {
    auto wpos = ImGui::GetWindowPos();
    auto wsize = ImGui::GetWindowSize();
    _r.setOrigin( V2f{wpos.x, wpos.y} );
    _r.setSize( V2f{wsize.x, wsize.y} );
    ImGui::End();
}

void LayoutBoxRenderer::render( SceneGraph& _sg, RenderOrchestrator& _rsg, JMATH::Rect2f& _r, BoxFlagsT _flags ) {

    if ( startRender( _r, _flags) ) {
        renderImpl( _sg, _rsg, _r );
        endRender( _r );
    }

    rect = _r;
}

LayoutBoxRenderer::LayoutBoxRenderer( const std::string& name, bool _visible ) : name( name ), bVisible(_visible) {}

void LayoutBoxRenderer::toggleVisible() {
    bVisible = !bVisible;
}

void LayoutBoxRenderer::setVisible( bool _bVis ) {
    bVisible = _bVis;
}
