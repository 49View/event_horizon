//
//  anim.h
//  sixthview
//
//  Created by Dado on 07/06/2016.
//  Copyright © 2016 JFDP Labs. All rights reserved.
//

#pragma once

#include <memory>
#include <functional>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

#include <core/math/vector4f.h>
#include <core/math/quaternion.h>
#include <core/math/math_util.h>

enum class AnimLoopType {
	Linear,
	Reverse,
	Bounce,
	Loop,
	Toggle
};

enum class AnimVelocityType {
	Linear,
	Cosine,
	Exp,
	Hermite
};

static const std::string AnimDefaultGroupName = "default";

using TimelineIndex = uint64_t;
using KeyFrameTimes_t = std::vector<float>;
using AnimVisitCallback = std::function<void(const std::string&, const KeyFrameTimes_t&, const std::vector<float>&, TimelineIndex)>;

const static TimelineIndex   tiNorm  = 1000000000;

constexpr static TimelineIndex   tiFloat = 0;
constexpr static TimelineIndex   tiV2f   = tiFloat + tiNorm;
constexpr static TimelineIndex   tiV3f   = tiV2f   + tiNorm;
constexpr static TimelineIndex   tiV4f   = tiV3f   + tiNorm;
constexpr static TimelineIndex   tiQuat  = tiV4f   + tiNorm;
constexpr static TimelineIndex   tiInt   = tiQuat  + tiNorm;

constexpr static TimelineIndex   tiFloatIndex   = tiFloat / tiNorm;
constexpr static TimelineIndex   tiV2fIndex     = tiV2f   / tiNorm;
constexpr static TimelineIndex   tiV3fIndex     = tiV3f   / tiNorm;
constexpr static TimelineIndex   tiV4fIndex     = tiV4f   / tiNorm;
constexpr static TimelineIndex   tiQuatIndex    = tiQuat  / tiNorm;
constexpr static TimelineIndex   tiIntIndex     = tiInt   / tiNorm;

template <typename T>
struct KeyFramePair {
    KeyFramePair( float timeStamp, T value ) : time( timeStamp ), value( value ) {}

    float time = 0.0f;
    T value;
    AnimVelocityType velocityType = AnimVelocityType::Linear;
};

template <typename T>
class TimelineStream {
public:
    TimelineStream() = default;
    explicit TimelineStream( AnimValue<T> source) : source( std::move( source )) {}

    TimelineStream& k( float _timeAt, const T& _value ) {
        keyframes.emplace_back( _timeAt, _value );
        return *this;
    }

    bool getKeyFrameIndexAt( float _timeElapsed, uint64_t& _index, float& _delta ) {

        if ( keyframes.size() < 2 ) return false;
        if ( _timeElapsed < 0.0f ) return false;

        for ( uint64_t t = 0; t < keyframes.size()-1; t++ ) {
            if ( inRangeEx( _timeElapsed, keyframes[t].time, keyframes[t+1].time) ) {
                _delta = interpolateInverse( keyframes[t].time, keyframes[t+1].time, _timeElapsed );
                _index = t;
                return true;
            }
        }

        return false;
    }

    T valueAt( float _timeElapsed ) {
        auto value = source->value;
        uint64_t keyFrameIndex = 0;
        float delta = 0.0f;
        float currTimeElapsed = _timeElapsed - animationStartTime;
        source->isAnimating = getKeyFrameIndexAt( currTimeElapsed, keyFrameIndex, delta );
        if ( source->isAnimating ) {
            uint64_t p1 = keyFrameIndex;
            uint64_t p2 = keyFrameIndex+1;
            switch ( keyframes[p2].velocityType ) {
                case AnimVelocityType::Linear:
                    value = interpolate( keyframes[p1].value, keyframes[p2].value, delta );
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

    void reset( float _startTime) {
        animationStartTime = _startTime;
    }

    void visit( AnimVisitCallback _callback, TimelineIndex _valueType ) {
        auto lValues = dump();
//        _callback(Name(), keyframeTimes, lValues, _valueType );
    }

    std::vector<float> dump() const {
        std::vector<float> ret;
//        if constexpr (std::is_same_v<T, float>) {
//            for ( const auto& v : keyframes ) ret.emplace_back( v.value );
//        } else if constexpr (std::is_same_v<T, V2f>) {
//            for ( const auto& v : keyframeValues ) {
//                ret.emplace_back( v.x());
//                ret.emplace_back( v.y());
//            }
//        } else if constexpr (std::is_same_v<T, V3f>) {
//            for ( const auto& v : keyframeValues ) {
//                ret.emplace_back( v.x());
//                ret.emplace_back( v.y());
//                ret.emplace_back( v.z());
//            }
//        } else if constexpr (std::is_same_v<T, V4f>) {
//            for ( const auto& v : keyframeValues ) {
//                ret.emplace_back( v.x());
//                ret.emplace_back( v.y());
//                ret.emplace_back( v.z());
//                ret.emplace_back( v.w());
//            }
//        } else if constexpr (std::is_same_v<T, Quaternion>) {
//            for ( const auto& v : keyframeValues ) {
//                ret.emplace_back( v.x());
//                ret.emplace_back( v.y());
//                ret.emplace_back( v.z());
//                ret.emplace_back( v.w());
//            }
//        } else if constexpr (std::is_same_v<T, int>) {
//            for ( const auto& v : keyframeValues ) {
//                ret.emplace_back( v);
//            }
//        }
        return ret;
    }

    TimelineIndex getTimelineIndex() const {
        return timelineIndex;
    }

    void setTimelineIndex( TimelineIndex timelineIndex ) {
        TimelineStream::timelineIndex = timelineIndex;
    }

    bool isActive() const { return source->isAnimating; };

    void add( const std::string& _groupName = AnimDefaultGroupName );

    const std::string& Name() const { return source->Name(); }
private:
    AnimValue<T> source;
    std::vector<KeyFramePair<T>> keyframes;
    TimelineIndex timelineIndex = 0;
    float animationStartTime = 0.0f;
};

template<typename V>
using TimelineMap = std::unordered_map<uint64_t, TimelineStream<V>>;
using TimelineIndexVector = std::vector<TimelineIndex>;
using TimelineGroupMap = std::unordered_map<std::string, TimelineIndexVector>;

class Timeline {
    struct TimelineMapSpec {
        TimelineMap<int>        tmapi;
        TimelineMap<float>      tmapf;
        TimelineMap<Vector2f>   tmapV2;
        TimelineMap<Vector3f>   tmapV3;
        TimelineMap<Vector4f>   tmapV4;
        TimelineMap<Quaternion> tmapQ;

        void visit( TimelineIndex _k,  AnimVisitCallback _callback );
        void update( TimelineIndex _k );
        void reset( TimelineIndex _k );
        bool isActive( TimelineIndex _k ) const;

        static TimelineIndex add( TimelineStream<int>& _stream ) {
            _stream.setTimelineIndex( tiInt + TimelineMapSpec::mkf++ );
            timelines.tmapi.insert( { _stream.getTimelineIndex(), _stream } );
            return _stream.getTimelineIndex();
        }

        static TimelineIndex add( TimelineStream<float>& _stream ) {
            _stream.setTimelineIndex( tiFloat + TimelineMapSpec::mkf++ );
            timelines.tmapf.insert( { _stream.getTimelineIndex(), _stream } );
            return _stream.getTimelineIndex();
        }

        static TimelineIndex add( TimelineStream<Vector2f>& _stream ) {
            _stream.setTimelineIndex( tiV2f + TimelineMapSpec::mkf++ );
            timelines.tmapV2.insert( { _stream.getTimelineIndex(), _stream } );
            return _stream.getTimelineIndex();
        }
        static TimelineIndex add( TimelineStream<Vector3f>& _stream ) {
            _stream.setTimelineIndex( tiV3f + TimelineMapSpec::mkf++ );
            timelines.tmapV3.insert( { _stream.getTimelineIndex(), _stream } );
            return _stream.getTimelineIndex();
        }
        static TimelineIndex add( TimelineStream<Vector4f>& _stream ) {
            _stream.setTimelineIndex( tiV4f + TimelineMapSpec::mkf++ );
            timelines.tmapV4.insert( { _stream.getTimelineIndex(), _stream } );
            return _stream.getTimelineIndex();
        }
        static TimelineIndex add( TimelineStream<Quaternion>& _stream ) {
            _stream.setTimelineIndex( tiQuat + TimelineMapSpec::mkf++ );
            timelines.tmapQ.insert( { _stream.getTimelineIndex(), _stream } );
            return _stream.getTimelineIndex();
        }

        static TimelineIndex mkf;
    };
public:
    static void update() {
        for ( auto k : activeTimelines ) {
            timelines.update( k );
        }
       	for ( auto it = activeTimelines.begin(); it != activeTimelines.end();) {
            if ( !timelines.isActive(*it) ) { it = activeTimelines.erase( it ); } else { ++it; }
        }
    }

    template <typename T>
    static void play( TimelineStream<T>& _stream ) {
        add( _stream );
        activeTimelines.insert( _stream.getTimelineIndex() );
    }

    static void play( const std::string & _groupName ) {
        if ( const auto& it = timelineGroups.find(_groupName); it != timelineGroups.end() ) {
            for ( const auto& gi : it->second ) {
                timelines.reset( gi );
                activeTimelines.insert( gi );
            }
        }
    }

    static void visitGroup( const std::string& _groupName, AnimVisitCallback _callback ) {
        if ( const auto& it = timelineGroups.find(_groupName); it != timelineGroups.end() ) {
            for ( const auto& ti : it->second ) {
                timelines.visit( ti, _callback );
            }
        }
    }

    static const TimelineMapSpec& Timelines() { return timelines; }
    static const TimelineGroupMap& Groups() { return timelineGroups; }

    template <typename T>
    static void add( const std::string& _group, AnimValue<T> _source, std::initializer_list<KeyFramePair<T>>&& _keys ) {

    }

private:
    template <typename T>
    static TimelineIndex add( TimelineStream<T>& _stream, const std::string& _groupName = AnimDefaultGroupName ) {
        timelines.add( _stream );
        if ( const auto& it = timelineGroups.find(_groupName); it != timelineGroups.end() ) {
            it->second.emplace_back( _stream.getTimelineIndex() );
        } else {
            timelineGroups.insert( { _groupName, { _stream.getTimelineIndex() } } );
        }
        return _stream.getTimelineIndex();
    }

private:
    static std::unordered_set<TimelineIndex> activeTimelines;
    static TimelineGroupMap timelineGroups;
    static TimelineMapSpec timelines;

    template <typename U>
    friend class TimelineStream;
};

template<typename T>
void TimelineStream<T>::add( const std::string& _groupName ) {
    Timeline::add( *this, _groupName );
}
