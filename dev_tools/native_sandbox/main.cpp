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

void f1( SceneLayout* _layout, [[maybe_unused]] Scene* _p ) {

    GeomBuilder geom;

    Vector2fList tv;
    std::vector<Vector2fList> holes;
    Rect2f quad{ Vector2f::ZERO, Vector2f{0.762, 1.97} };
    int q = 0;
    for ( auto& p : quad.points() ) {
        tv.emplace_back( p );
    }

    Rect2f quadl{ Vector2f{0.1f, 0.1f}, Vector2f{0.5f, 0.4f} };
    Vector2fList hole;
    for ( auto& p : quadl.points() ) {
        hole.emplace_back( p );
    }
    holes.emplace_back( hole );

    Triangulator tri{ tv, holes };
    auto tvo = tri.get2dTrianglesTuple();

    q = 0;
    for ( const auto& [v1,v2,v3] : tvo ) {
       LOGR( "V%d: %s %s %s", q++, v1.toString().c_str(), v2.toString().c_str(), v3.toString().c_str() );
        std::vector<Vector3f> plist;
        plist.emplace_back(Vector3f{ v1, 0.0f});
        plist.emplace_back(Vector3f{ v2, 0.0f});
        plist.emplace_back(Vector3f{ v3, 0.0f});
        geom.addPoly( PolyLine{ plist, Vector3f::Z_AXIS, ReverseFlag::True } );
    }

    geom.build(_p->RSG());

    _layout->addBox( Name::Foxtrot, 0.0f, 1.0f, 0.0f, 1.0f, CameraControls::Fly );

    _p->postActivate( [](Scene* _p) {
        _p->CM().getCamera( Name::Foxtrot)->goTo( Vector3f{0.0f, 1.0f, 3.0f}, 0.0f);
    } );
}

void renderSandbox( [[maybe_unused]] Scene* p ) {
}

int main( int argc, [[maybe_unused]] char *argv[] ) {

    EventHorizon ev{ std::make_shared<SceneLayout>(f1, renderSandbox, nullptr), LoginFields::Daemon(), InitializeWindowFlags::HalfSize };

    return 0;
}
