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

template <typename T>
using AnimValue = std::shared_ptr<AnimType<T>>;

template <typename T>
class TimelineStream {
public:
    TimelineStream() = default;
    explicit TimelineStream( AnimValue<T> source) : source( std::move( source )) {}

    TimelineStream& k( float _timeAt, const T& _value ) {
        keyframeTimes.emplace_back( _timeAt );
        keyframeValues.emplace_back( _value );
        return *this;
    }

    bool getKeyFrameIndexAt( float _timeElapsed, uint64_t& _index, float& _delta ) {

        if ( keyframeValues.size() < 2 ) return false;

        for ( uint64_t t = 0; t < keyframeValues.size()-1; t++ ) {
            if ( inRangeEx( _timeElapsed, keyframeTimes[t], keyframeTimes[t+1]) ) {
                _delta = interpolateInverse( keyframeTimes[t], keyframeTimes[t+1], _timeElapsed );
                _index = t;
                return true;
            }
        }

        return true;
    }

    T valueAt( float _timeElapsed ) {
        auto value = source->value;
        uint64_t keyFrameIndex = 0;
        float delta = 0.0f;
        if ( getKeyFrameIndexAt( _timeElapsed, keyFrameIndex, delta ) ) {
            uint64_t p1 = keyFrameIndex;
            uint64_t p2 = keyFrameIndex+1;
            switch ( velocityType ) {
                case AnimVelocityType::Linear:
                    value = interpolate( keyframeValues[p1], keyframeValues[p2], delta );
//            case AnimVelocityType::Cosine:
//                value = JMATH::lerp( asinf( JMATH::saturate( deltaTimeNotIncludingDelay / finalTime ) ) / M_PI_2, keyframes[0], keyframes[1] );
//                break;
//            case AnimVelocityType::Exp:
//                value = JMATH::lerp( ( expf( JMATH::saturate( deltaTimeNotIncludingDelay / finalTime ) ) - 1.0f ) / ( M_E - 1.0f ), keyframes[0], keyframes[1] );
//                break;
//            case AnimVelocityType::Hermite:
//			    value = traversePathHermite( cameraPath, deltaTimeNotIncludingDelay );
//                break;
                default:
                    break;
            }
        }
        return value;
    }

    void update( float _timeElapsed ) {
        source->value = valueAt(_timeElapsed);
    }

private:
    AnimValue<T> source;
    std::vector<float> keyframeTimes;    // Times and values are strictly internal and _must_ be always CRUD-ed together
    std::vector<T> keyframeValues;       // Times and values are strictly internal and _must_ be always CRUD-ed together
    AnimVelocityType velocityType = AnimVelocityType::Linear;
};

//class TimelineStreamUpdater {
//
//    uint64_t Index() const { return currIndex; }
//    void Index( uint64_t _i) { currIndex = _i; }
//private:
//    uint64_t currIndex = 0;
//};

template<typename V>
using TimelineMap = std::unordered_map<uint64_t, TimelineStream<V>>;
using TimelineIndex = uint64_t;

class Timeline {
    struct TimelineMapSpec {
        TimelineMap<float>      tmapf;
        TimelineMap<V2f>        tmapV2;
        TimelineMap<V3f>        tmapV3;
        TimelineMap<V4f>        tmapV4;
        TimelineMap<Quaternion> tmapQ;

        const static TimelineIndex   tiNorm  = 1000000000;

        const static TimelineIndex   tiFloat = 0;
        const static TimelineIndex   tiV2f   = tiFloat+tiNorm;
        const static TimelineIndex   tiV3f   = tiV2f+tiNorm;
        const static TimelineIndex   tiV4f   = tiV3f+tiNorm;
        const static TimelineIndex   tiQuat  = tiV4f+tiNorm;

        const static TimelineIndex   tiFloatIndex   = tiFloat/tiNorm;
        const static TimelineIndex   tiV2fIndex     = tiV2f  /tiNorm;
        const static TimelineIndex   tiV3fIndex     = tiV3f  /tiNorm;
        const static TimelineIndex   tiV4fIndex     = tiV4f  /tiNorm;
        const static TimelineIndex   tiQuatIndex    = tiQuat /tiNorm;

        void update( TimelineIndex _k ) {
            auto ki = _k / tiNorm;
            switch (ki) {
                case tiFloatIndex:
                    tmapf[_k].update( GameTime::getCurrTimeStamp() );
                    break;
                case tiV2fIndex:
                    tmapV2[_k].update( GameTime::getCurrTimeStamp() );
                    break;
                case tiV3fIndex:
                    tmapV3[_k].update( GameTime::getCurrTimeStamp() );
                    break;
                case tiV4fIndex:
                    tmapV4[_k].update( GameTime::getCurrTimeStamp() );
                    break;
                case tiQuatIndex:
                    tmapQ[_k].update( GameTime::getCurrTimeStamp() );
                    break;
                default:
                    break;
            }
        }

        static TimelineIndex add( const TimelineStream<float>& _stream ) {
            TimelineIndex i = tiFloat + TimelineMapSpec::mkf++;
            timelines.tmapf.insert( { i, _stream } );
            return i;
        }

        static TimelineIndex add( const TimelineStream<V2f>& _stream ) {
            TimelineIndex i = tiV2f + TimelineMapSpec::mkf++;
            timelines.tmapV2.insert( { i, _stream } );
            return i;
        }
        static TimelineIndex add( const TimelineStream<V3f>& _stream ) {
            TimelineIndex i = tiV3f + TimelineMapSpec::mkf++;
            timelines.tmapV3.insert( { i, _stream } );
            return i;
        }
        static TimelineIndex add( const TimelineStream<V4f>& _stream ) {
            TimelineIndex i = tiV4f + TimelineMapSpec::mkf++;
            timelines.tmapV4.insert( { i, _stream } );
            return i;
        }
        static TimelineIndex add( const TimelineStream<Quaternion>& _stream ) {
            TimelineIndex i = tiQuat + TimelineMapSpec::mkf++;
            timelines.tmapQ.insert( { i, _stream } );
            return i;
        }

        static TimelineIndex mkf;
    };
public:
    static void update() {
        for ( auto k : activeTimelines ) {
            timelines.update( k );
        }
    }

    template <typename T>
    static TimelineIndex add( const TimelineStream<T>& _stream ) {
        auto ti = timelines.add( _stream );
        activeTimelines.insert( ti );
        return ti;
    }

private:
    static std::unordered_set<TimelineIndex> activeTimelines;
    static TimelineMapSpec timelines;
};

uint64_t Timeline::TimelineMapSpec::mkf = 0;
Timeline::TimelineMapSpec Timeline::timelines;
std::unordered_set<TimelineIndex> Timeline::activeTimelines;

void f1( SceneLayout* _layout, [[maybe_unused]] Scene* _p ) {

    _layout->addBox( Name::Foxtrot, 0.0f, 1.0f, 0.0f, 1.0f, CameraControls::Fly );

    _p->postActivate( [](Scene* _p) {
        auto c = _p->CM().getCamera( Name::Foxtrot);
        c->goTo( Vector3f{0.0f, 1.0f, 3.0f}, 0.0f);
//        Timeline::add(TimelineStream<V3f>{c->PosAnim()}.k(0.0f, V3f::ZERO).k(2.0f, V3f::ONE*10.0f).k(3.0f, V3f::Y_AXIS*5.0f));

//        for ( int t = 0; t < 43432; t++ ) {
//            Timeline::update();
//        }
        UISB{ UIShapeType::Text3d, "Hego", 0.6f }.c(Color4f::AQUAMARINE).build(_p->RSG().RR());
    } );
}

void renderSandbox( [[maybe_unused]] Scene* p ) {
}

int main( int argc, [[maybe_unused]] char *argv[] ) {

    EventHorizon ev{ std::make_shared<SceneLayout>(f1, renderSandbox, nullptr), LoginFields::Daemon(), InitializeWindowFlags::HalfSize };

    return 0;
}
