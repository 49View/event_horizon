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
#include <graphics/platform_graphics.hpp>
#include <graphics/shader_manager.h>
#include <poly/geom_builder.h>
#include <poly/polyclipping/clipper.hpp>
#include <poly/triangulator.hpp>
#include <poly/polypartition.h>
#include <poly/profile.hpp>
#include <graphics/ui/ui_shape_builder.h>

void f1( SceneLayout* _layout, [[maybe_unused]] Scene* _p ) {

    _layout->addBox( Name::Foxtrot, 0.0f, 1.0f, 0.0f, 1.0f, CameraControls::Fly );

    _p->postActivate( [](Scene* _p) {
        auto c = _p->CM().getCamera( Name::Foxtrot);
        c->goTo( Vector3f{0.0f, 1.0f, 3.0f}, 0.0f);
//        Timeline::add(TimelineStream<V3f>{c->PosAnim()}.k(0.0f, Vector3f{0.0f, 1.0f, 3.0f}).k(10.0f, V3f::ONE*10.0f).k(20.0f, V3f::Y_AXIS*5.0f));

        UISB{ UIShapeType::Text3d, "Hego", 0.6f }.c(Color4f::AQUAMARINE).build(_p->RSG().RR());
    } );
}

void renderSandbox( [[maybe_unused]] Scene* p ) {
}

int main( int argc, [[maybe_unused]] char *argv[] ) {

    EventHorizon ev{ std::make_shared<SceneLayout>(f1, renderSandbox, nullptr), LoginFields::Daemon(), InitializeWindowFlags::HalfSize };

    return 0;
}
