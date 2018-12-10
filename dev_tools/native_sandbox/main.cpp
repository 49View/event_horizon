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

std::shared_ptr<Profile> makeEnglishDoorProfile( const std::string& _name, const std::vector<Vector2f>& vv2fs,
                                                    const std::vector<float>& vfs ) {

    std::vector<Vector2f> points;

    points.emplace_back( 1.0f, -1.0f );
    points.emplace_back( 1.0f, 0.0f );

    points.emplace_back( 0.99f, -0.1f );
    points.emplace_back( 0.98f, -0.1f );
    points.emplace_back( 0.85f, -0.4f );
    points.emplace_back( 0.82f, -0.9f );
    points.emplace_back( 0.80f, -0.9f );
    points.emplace_back( 0.72f, -1.0f );
    points.emplace_back( 0.62f, -0.85f );
    points.emplace_back( 0.01f, -0.1f );
    points.emplace_back( 0.07f, -0.1f );

    points.emplace_back( 0.0f, -0.0f );
    points.emplace_back( 0.0f, -1.0f );

    for ( auto& p : points ) p*= vv2fs[0];

    std::shared_ptr<Profile> doorFrame_profile = std::make_shared<Profile>(_name);
    doorFrame_profile->createArbitrary( points );

    return doorFrame_profile;
}

template< size_t Tx = 2, size_t Ty = 3>
void englishDoor( DependencyMaker& _rr, const Vector2f& _doorSize,
                  const std::array<Vector2f, Ty>& _pratios = { Vector2f{0.288f, 0.314f}, Vector2f{0.288f, 0.314f}, Vector2f{0.288f, 0.111f} },
                  const std::array<float, Ty>& ygapRatios = { 0.071f, 0.091f, 0.045f },
                  float _doorThinkness = 0.045f ) {

    std::vector<Vector2fList> holes;
    Rect2f quad{ Vector2f::ZERO, _doorSize };

    static const size_t numPanels = Tx*Ty;
    Vector2fList hole;
    std::array<Vector2f, Ty> dsizes{};
    for ( size_t i = 0; i < Ty; i++ ) {
        dsizes[i] = {_pratios[i].x()*_doorSize.x(), _pratios[i].y()*_doorSize.y()};
    }
    float xgapDelta = (quad.width() - (dsizes[0].x()*Tx)) / (Tx+1);
    std::vector<float> ygaps;
    ygaps.reserve( ygapRatios.size());
    for ( const auto& yg : ygapRatios ) {
        ygaps.emplace_back( yg * _doorSize.y() );
    }

    std::array<Vector2f, numPanels> o{};
    std::array<Rect2f, numPanels> orect{};
    std::array<Rect2f, numPanels> orectinner{};
    size_t tc = 0;
    for ( size_t ty = 0; ty < Ty; ty++ ) {
        for ( size_t tx = 0; tx < Tx; tx++ ) {
            float ppy = ty == 0 ? 0.0f : dsizes[ty-1].y() + ygaps[ty];
            float currY = tc == 0 ? ygaps[0] : o[tc-1].y() + (tx == 0 ? ppy : 0.0f);

            o[tc] = { xgapDelta + (tx*(dsizes[ty].x()+xgapDelta)), currY };

            orect[tc] = Rect2f{ o[tc], o[tc]+dsizes[ty] };

            holes.emplace_back( orect[tc].points() );
            orectinner[tc] = orect[tc];
            orectinner[tc].shrink(_doorThinkness);
            ++tc;
        }
    }

    auto silhouette = Triangulator::execute2d(quad.points(), holes, 0.000001f);
    float hf = _doorThinkness*0.5f;

    GeomBuilder{ silhouette, hf, Vector3f::Z_AXIS_NEG, ReverseFlag::True }.build(_rr);
    GeomBuilder{ silhouette, -hf, Vector3f::Z_AXIS, ReverseFlag::False }.build(_rr);

    GeomBuilder{ ProfileBuilder{Vector2f::Y_AXIS*hf, -Vector2f::Y_AXIS*hf}, quad.points() }.ff(FollowerFlags::WrapPath).build( _rr );

    ProfileBuilder pb;
    pb.cf(1.0f).cv2( Vector2f{ -_doorThinkness, _doorThinkness*0.4f } ).func(makeEnglishDoorProfile);
    ProfileBuilder pbr;
    pbr.cf(-1.0f).cv2( Vector2f{ -_doorThinkness, -_doorThinkness*0.4f } ).func(makeEnglishDoorProfile);

    for ( int i = 0; i < numPanels; i++ ) {
        GeomBuilder{ pb, orect[i].points() }.ff(FollowerFlags::WrapPath).at(Vector3f::Z_AXIS*hf).build( _rr );
        GeomBuilder{ orectinner[i].points3dcw(0.0f), Vector3f::Z_AXIS_NEG, ReverseFlag::True }.at(Vector3f::Z_AXIS*hf).build( _rr );

        GeomBuilder{ pbr, orect[i].points() }.ff(FollowerFlags::WrapPath).at(Vector3f::Z_AXIS_NEG*hf).build( _rr );
        GeomBuilder{ orectinner[i].points3dcw(0.0f), Vector3f::Z_AXIS, ReverseFlag::False }.at(Vector3f::Z_AXIS_NEG*hf).build( _rr );
    }
}

void f1( SceneLayout* _layout, [[maybe_unused]] Scene* _p ) {

    englishDoor( _p->RSG(), Vector2f{0.762, 1.97} );

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
