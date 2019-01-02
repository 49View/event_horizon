#include <utility>

#include <utility>

#include <utility>

#include <utility>

#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include "render_scene_graph/event_horizon.h"

#include <core/file_manager.h>
#include <core/math/anim.h>
#include <core/node.hpp>
#include <graphics/platform_graphics.hpp>
#include <graphics/shader_manager.h>
#include <poly/geom_builder.h>
#include <poly/polyclipping/clipper.hpp>
#include <poly/triangulator.hpp>
#include <poly/polypartition.h>
#include <poly/profile.hpp>
#include <poly/ui_shape_builder.h>

void f1( SceneLayout* _layout, [[maybe_unused]] Scene* _p ) {

    _layout->addBox( Name::Foxtrot, 0.0f, 1.0f, 0.0f, 0.8f, CameraControls::Fly );
    _layout->addBox( SceneLayoutDefaultNames::Console, 0.0f, 1.0f, 1.0f-0.2f, 1.0f );

    _p->postActivate( [](Scene* _p) {
        auto c = _p->CM().getCamera(Name::Foxtrot);
        c->goTo( Vector3f{0.0f, 1.0f, 3.0f}, 0.0f);

        auto cube = GB{ShapeType::Cube}.buildr(_p->RSG());
//        auto pin = GB{ GeomBuilderType::file, "pin" }.buildr(_p->RSG());
        auto text = UISB{ UIShapeType::Text3d, "Hello", 0.6f }.c(Color4f::AQUAMARINE).buildr(_p->RSG());

        auto v = _p->RSG().Nodes();

        LOGR( "* %s", cube->Hash().c_str() );
        LOGR( "* %s", text->Hash().c_str() );

//        LOGR( "* %s", pin->Hash().c_str() );
//        for ( const auto& [k,pg] : v ) {
//            LOGR( "- %s", std::string(pg->Hash()).c_str() );
//        }
        const std::string cName = "urca";
        TimelineStream<V3f>{cube->PosAnim()}.
                  k(0.0f, Vector3f{0.0f, 1.0f, 0.0f}).
                  k(5.0f, Vector3f{3.0f, 1.0f, 3.0f}).
                  k(15.0f, V3f::Y_AXIS*1.0f).
                  add(cName);
        TimelineStream<V3f>{text->ScaleAnim()}.
                k(0.0f, Vector3f{0.0f, 0.0f, 0.0f}).
                k(4.0f, Vector3f{0.0f, 0.0f, 0.0f}).
                k(4.31f, Vector3f{1.0f, 1.0f, 1.0f}).
                add(cName);
        Timeline::play(cName);

    } );
}

void renderSandbox( [[maybe_unused]] Scene* p ) {
}

int main( int argc, [[maybe_unused]] char *argv[] ) {

    EventHorizon ev{ std::make_shared<SceneLayout>(f1, renderSandbox, nullptr), LoginFields::Daemon(), InitializeWindowFlags::HalfSize };

    return 0;
}
