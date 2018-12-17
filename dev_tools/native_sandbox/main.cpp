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


std::shared_ptr<Profile> makeValanceProfile( const std::string& _name, const std::vector<Vector2f>& vv2fs,
                                             const std::vector<float>& vfs ) {
    V2f size = vv2fs[0];
    float bump = size.x()*0.8f;
    float w1 = size.x()-bump;
    float w2 = size.y()-bump*2.0f;

    return ProfileMaker{_name}.sd(10).o().lx(w1).ay(bump).ly(w2).ay(bump).lx(-w1).make();
}

void drawBlinds( const Rect2f& _rect, Scene* _p ) {

    // Blind constants
    float bw = _rect.width();
    float bh = _rect.height();
    const V3f blindSize{ _rect.size(), 0.04f };

    Vector2f slatSize{ bw*0.99f, 0.025f };
    float slatThickness= 0.003f;
    int numslats = static_cast<int>( bh / slatSize.y() );
    const Vector2f valanceSize{0.005f, 0.03f};
    V3f valencePos{ 0.0f, bh-valanceSize.y(), blindSize.z()*0.5f };
    V3f slat3dSize{ slatSize.x(), slatThickness, slatSize.y() };
    V3f slat3dSizeBig{ slatSize.x(), slatThickness*2.0f, slatSize.y() };
    V3f wandSize= { 0.012f, bh*0.6f, 0.012f };
    V3f wandPos = { bw * 0.4f, valencePos.y()-wandSize.y()*0.5f, valencePos.z() };

    GeomBuilder{ ProfileBuilder{}.cv2(valanceSize).func(makeValanceProfile),
                 lineLR(V3f::X_AXIS, 1.0f), V3f::UP_AXIS }.at(valencePos).build(_p->RSG());

    float deltaInc = 1.0f / static_cast<float>(numslats);
    float delta = 0.0f;
    for ( int t = 1; t < numslats; t++ ) {
        GeomBuilder{ ShapeType::Pillow }.s(slat3dSize).at(Vector3f::UP_AXIS*(delta*bh)).build(_p->RSG());
        delta += deltaInc;
    }
    GeomBuilder{ ShapeType::Pillow }.s(slat3dSizeBig).build(_p->RSG());

    GeomBuilder{ ShapeType::Cylinder }.s(wandSize).at(wandPos).build(_p->RSG());
}

void f1( SceneLayout* _layout, [[maybe_unused]] Scene* _p ) {

    _layout->addBox( Name::Foxtrot, 0.0f, 1.0f, 0.0f, 1.0f, CameraControls::Fly );

    _p->postActivate( [](Scene* _p) {
        _p->CM().getCamera( Name::Foxtrot)->goTo( Vector3f{0.0f, 1.0f, 3.0f}, 0.0f);
//        drawBlinds( Rect2f{1.0f}, _p );
        UISB{ UIShapeType::Text3d }.ti("Hello there..........").
                s(1.0f).ta( UiControlFlag::TextAlignCenter ).fh( 0.1f ).rbi(2).vn("urca").
                c(Color4f::AQUAMARINE).build(_p->RSG().RR());

    } );
}

void renderSandbox( [[maybe_unused]] Scene* p ) {
}

int main( int argc, [[maybe_unused]] char *argv[] ) {

    EventHorizon ev{ std::make_shared<SceneLayout>(f1, renderSandbox, nullptr), LoginFields::Daemon(), InitializeWindowFlags::HalfSize };

    return 0;
}
