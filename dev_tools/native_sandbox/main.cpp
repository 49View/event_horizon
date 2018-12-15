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


std::shared_ptr<Profile> makeValanceProfile( const std::string& _name, const std::vector<Vector2f>& vv2fs,
                                                       const std::vector<float>& vfs ) {

    V2f size = vv2fs[0];
    float bump = size.x()*0.8f;
    float w1 = size.x()-bump;
    ProfileMaker pm{_name};

    pm.sd(10).l(V2f::ZERO).lx( w1 ).ay( bump ).ly( size.y()-bump*2.0f ).ay( bump ).lx( -w1 );

    return pm.make();
}

void drawBlinds( const Rect2f& _rect, Scene* _p ) {

    const Vector2f valanceSize{0.005f, 0.03f};
    ProfileBuilder pv;
    pv.cv2(valanceSize).func(makeValanceProfile);

//    GeomBuilder{ pv, lineRL(V2f::X_AXIS, 1.0f), 0.0f, V3f::UP_AXIS }.build(_p->RSG());
    GeomBuilder{ pv, Rect2f{1.0f} }.build(_p->RSG());

    return;
    // Blind constants
    float bw = _rect.width();
    float bh = _rect.height();
    Vector2f slatSize{ bw*0.99f, 0.025f };
    float slatThickness= 0.003f;
    int numslats = static_cast<int>( bh / slatSize.y() );

    V3f slat3dSize{ slatSize.x(), slatThickness, slatSize.y() };
    float deltaInc = 1.0f / static_cast<float>(numslats);
    float delta = 0.0f;
    for ( int t = 0; t < numslats; t++ ) {
        GeomBuilder{ ShapeType::Pillow }.s(slat3dSize).at(Vector3f::UP_AXIS*(delta*bh)).build(_p->RSG());
        delta += deltaInc;
    }
}

void f1( SceneLayout* _layout, [[maybe_unused]] Scene* _p ) {

    _layout->addBox( Name::Foxtrot, 0.0f, 1.0f, 0.0f, 1.0f, CameraControls::Fly );

    _p->postActivate( [](Scene* _p) {
        _p->CM().getCamera( Name::Foxtrot)->goTo( Vector3f{0.0f, 1.0f, 3.0f}, 0.0f);
        drawBlinds( Rect2f{1.0f}, _p );
    } );
}

void renderSandbox( [[maybe_unused]] Scene* p ) {
}

int main( int argc, [[maybe_unused]] char *argv[] ) {

    EventHorizon ev{ std::make_shared<SceneLayout>(f1, renderSandbox, nullptr), LoginFields::Daemon(), InitializeWindowFlags::HalfSize };

    return 0;
}
