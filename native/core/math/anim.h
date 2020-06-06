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
#include <core/math/anim_type.hpp>
#include <core/game_time.h>
#include <core/uuid.hpp>
#include <core/util_range.hpp>

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

using KeyFrameTimes_t = std::vector<float>;
using AnimVisitCallback = std::function<void(const std::string&, const std::vector<float>&, TimelineIndex, TimelineIndex, int)>;
using TimelineLinks = std::unordered_map< TimelineIndex, TimelineSet >;
using TimelineGroupCCF = std::function<void()>;
using TimelineUpdateGroupCCF = std::function<void(float)>;

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

namespace TLU {

inline static TimelineIndex getTI( inta _source ) {
    return tiInt + _source->UID();
}
inline static TimelineIndex getTI( floata _source ) {
    return tiFloat + _source->UID();
}
inline static TimelineIndex getTI( V2fa _source ) {
    return tiV2f + _source->UID();
}
inline static TimelineIndex getTI( V3fa _source ) {
    return tiV3f + _source->UID();
}
inline static TimelineIndex getTI( V4fa _source ) {
    return tiV4f + _source->UID();
}
inline static TimelineIndex getTI( Quaterniona _source ) {
    return tiQuat + _source->UID();
}

}

struct AnimUpdateCallback {
    template<typename ...Args>
    explicit AnimUpdateCallback( Args&& ... args ) : data(std::forward<Args>( args )...) {}
    TimelineUpdateGroupCCF operator()() const noexcept {
        return data;
    }
    TimelineUpdateGroupCCF data = nullptr;
};

struct AnimEndCallback {
    template<typename ...Args>
    explicit AnimEndCallback( Args&& ... args ) : data(std::forward<Args>( args )...) {}
    TimelineGroupCCF operator()() const noexcept {
        return data;
    }
    TimelineGroupCCF data;
};

enum class KeyFramePosition {
    Invalid,
    Pre,
    Valid,
    Post
};

template <typename T>
struct KeyFramePair {
    KeyFramePair( float timeStamp, T value ) : time( timeStamp ), value( value ) {}

    float time = 0.0f;
    T value;
    AnimVelocityType velocityType = AnimVelocityType::Hermite;
};

template < typename T>
using KFP = KeyFramePair<T>;

template <typename T>
class TimelineStream {
public:
    TimelineStream() = default;
    explicit TimelineStream( AnimValue<T> _source ) : source( std::move( _source )) {}

    void Source( AnimValue<T> _source ) {
        source = _source;
    }

    void sortOnTime() {
        std::sort( keyframes.begin(), keyframes.end(),
           []( const auto& a, const auto& b ) -> bool { return a.time < b.time; } );
    }

    TimelineStream& k( const KeyFramePair<T>& _kf  ) {
        addKey(_kf);
        return *this;
    }

    TimelineStream& k( float _timeAt, const T& _value ) {
        addKey({_timeAt, _value});
        return *this;
    }

    void addKey( const KeyFramePair<T>& _kf ) {
        bool bUpdateOnly = false;
        for ( auto& kf : keyframes ) {
            if ( kf.time == _kf.time ) {
                kf.value = _kf.value;
                bUpdateOnly = true;
                break;
            }
        }
        if ( !bUpdateOnly ) {
            keyframes.emplace_back( _kf );
            sortOnTime();
        }
    }

    bool deleteKey( size_t _index ) {
        if ( _index >= keyframes.size() ) return false;
        keyframes.erase( keyframes.begin() + _index );
        return keyframes.empty();
    }

    void updateKeyTime( size_t _index, float _time ) {
        if ( _index >= keyframes.size() ) return;
        keyframes[_index].time = _time;
        sortOnTime();
    }

    KeyFramePosition getKeyFrameIndexAt( float _timeElapsed, uint64_t& _index, float& _delta ) {

        if ( keyframes.size() < 2 ) return KeyFramePosition::Invalid;
        if ( _timeElapsed < keyframes[0].time ) return KeyFramePosition::Pre;

        for ( uint64_t t = 0; t < keyframes.size()-1; t++ ) {
            if ( inRangeEx( _timeElapsed, keyframes[t].time, keyframes[t+1].time) ) {
                _delta = interpolateInverse( keyframes[t].time, keyframes[t+1].time, _timeElapsed );
                _index = t;
                return KeyFramePosition::Valid;
            }
        }
        // Time is outside the last keyframe, so clamp it to the end
        _delta = 1.0f;
        _index = keyframes.size()-2;

        return KeyFramePosition::Post;
    }

    void addDelay( float _delay ) {
        if ( _delay <= 0.0f ) return;
        for ( auto& k : keyframes ) {
            k.time += _delay;
        }
    }

    T valueAt( float _timeElapsed ) {
        auto value = source->value;
        uint64_t keyFrameIndex = 0;
        float delta = 0.0f;
        auto keyFramePos = getKeyFrameIndexAt( _timeElapsed, keyFrameIndex, delta );
        source->isAnimating = keyFramePos == KeyFramePosition::Pre ||
                              keyFramePos == KeyFramePosition::Valid ||
                              ( keyFramePos == KeyFramePosition::Post && !triggeredPost );
        if ( keyFramePos == KeyFramePosition::Valid || ( keyFramePos == KeyFramePosition::Post && !triggeredPost ) ) {
            uint64_t p1 = keyFrameIndex;
            uint64_t p2 = keyFrameIndex+1;
            switch ( keyframes[p2].velocityType ) {
                case AnimVelocityType::Linear:
                    value = interpolate( keyframes[p1].value, keyframes[p2].value, delta );
                    break;
                case AnimVelocityType::Cosine:
                    value = interpolate( keyframes[p1].value, keyframes[p2].value, asinf( delta ) / M_PI_2 );
                    break;
                case AnimVelocityType::Exp:
                    value = interpolate( keyframes[p1].value, keyframes[p2].value, static_cast<float>(( expf( delta ) - 1.0f ) / ( M_E - 1.0f )) );
                    break;
                case AnimVelocityType::Hermite: {
                    uint64_t p0 = p1 > 0 ? p1 - 1 : 0;
                    uint64_t p3 = p2 < keyframes.size() - 2 ? p2 + 1 : p2;
                    value = interpolateHermite( keyframes[p0].value, keyframes[p1].value,
                                                keyframes[p2].value, keyframes[p3].value, delta );
                }
                break;
                default:
                    break;
            }
        }
        if ( keyFramePos == KeyFramePosition::Post ) triggeredPost = true;
        return value;
    }

    bool update( float _timeElapsed ) {
        source->value = valueAt(_timeElapsed);
        return isActive();
    }

    int strideDumpForType( TimelineIndex _valueType ) {
        switch (_valueType) {
            case tiIntIndex:
                return 2;
            case tiFloatIndex:
                return 2;
            case tiV2fIndex:
                return 3;
            case tiV3fIndex:
                return 4;
            case tiV4fIndex:
                return 5;
            case tiQuatIndex:
                return 5;
            default:
                return 0;
        }
    }

    void visit( AnimVisitCallback _callback, TimelineIndex _k, TimelineIndex _valueType ) {
        _callback(Name(), dump(), _k, _valueType, strideDumpForType(_valueType) );
    }

    std::vector<float> dump() const {
        std::vector<float> ret;
        if constexpr (std::is_same_v<T, float>) {
            for ( const auto& v : keyframes ) {
                ret.emplace_back( v.time );
                ret.emplace_back( v.value );
            }
        } else if constexpr (std::is_same_v<T, V2f>) {
            for ( const auto& v : keyframes ) {
                ret.emplace_back( v.time );
                ret.emplace_back( v.value.x());
                ret.emplace_back( v.value.y());
            }
        } else if constexpr (std::is_same_v<T, V3f>) {
            for ( const auto& v : keyframes ) {
                ret.emplace_back( v.time );
                ret.emplace_back( v.value.x());
                ret.emplace_back( v.value.y());
                ret.emplace_back( v.value.z());
            }
        } else if constexpr (std::is_same_v<T, V4f>) {
            for ( const auto& v : keyframes ) {
                ret.emplace_back( v.time );
                ret.emplace_back( v.value.x());
                ret.emplace_back( v.value.y());
                ret.emplace_back( v.value.z());
                ret.emplace_back( v.value.w());
            }
        } else if constexpr (std::is_same_v<T, Quaternion>) {
            for ( const auto& v : keyframes ) {
                ret.emplace_back( v.time );
                ret.emplace_back( v.value.x());
                ret.emplace_back( v.value.y());
                ret.emplace_back( v.value.z());
                ret.emplace_back( v.value.w());
            }
        } else if constexpr (std::is_same_v<T, int>) {
            for ( const auto& v : keyframes ) {
                ret.emplace_back( v.time );
                ret.emplace_back( static_cast<float>(v.value) );
            }
        }
        return ret;
    }

    TimelineIndex getTimelineIndex() const {
        return timelineIndex;
    }

    void setTimelineIndex( TimelineIndex timelineIndex ) {
        TimelineStream::timelineIndex = timelineIndex;
    }

    T value() const {
        return source->value;
    }
    bool isActive() const { return source->isAnimating; };
    const std::string& Name() const { return source->Name(); }
private:
    AnimValue<T> source;
    std::vector<KeyFramePair<T>> keyframes;
    TimelineIndex timelineIndex = 0;
    bool triggeredPost = false;
};

template<typename V>
using TimelineMap = std::unordered_map<uint64_t, TimelineStream<V>>;

template <typename V>
class TimelineGroup {
public:
    TimelineGroup() = default;
    explicit TimelineGroup( AnimValue<V> _source ) {
        stream.Source( std::move(_source) );
    }

    void play() {
        animationStartTime = -1.0f;
        stream.addDelay( startTimeOffset );
        bIsPlaying = true;
        bForceOneFrameOnly = false;
    }

    void playOneFrame() {
        animationStartTime = -1.0f;
        bIsPlaying = true;
        bForceOneFrameOnly = true;
    }

    void update() {
        if ( !bIsPlaying ) return;

        if ( ++frameTickCount <= frameTickOffset ) return;

        auto currGameStamp = GameTime::getCurrTimeStamp();
        if ( animationStartTime < 0.0f ) {
            animationStartTime = currGameStamp;
        }

//    uint64_t realFrameTicks = frameTickCount - frameTickOffset;
//    float timeDelta = timeElapsed;
        timeElapsed = ( currGameStamp - animationStartTime);// + animationInitialDelay;
//    timeDelta = timeElapsed - timeDelta;
//    float meanTimeDeltaAvr = meanTimeDelta / realFrameTicks;
//    if ( timeDelta > meanTimeDeltaAvr*3.0f && realFrameTicks > 3 ) {
//        timeElapsed -= timeDelta;
//        timeElapsed += meanTimeDeltaAvr;
//        timeDelta = meanTimeDeltaAvr;
//    }
//    meanTimeDelta += timeDelta;
        bIsPlaying = stream.update( timeElapsed );
        if ( !bIsPlaying ) {
            timeElapsed = 0.0f;
            meanTimeDelta = 0.0f;
            frameTickCount = 0;
            frameTickOffset = 0;
            if ( cuf ) cuf( timeElapsed );
            if ( ccf ) {
                ccf();
            }
        } else {
            if ( cuf ) cuf( timeElapsed );
        }
        if ( bForceOneFrameOnly ) {
            bIsPlaying = false;
            bForceOneFrameOnly = false;
        }
    }

    float animationTime() const {
        return bIsPlaying ? timeElapsed : -1.0f;
    }

    void FrameTickOffset( uint64_t _value ) {
        frameTickOffset = _value;
    }

    void StartTimeOffset( float _value ) {
        startTimeOffset = _value;
    }

    void CCF( TimelineGroupCCF _value ) {
        ccf = _value;
    }
    void CUF( TimelineUpdateGroupCCF _value ) {
        cuf = _value;
    }

    TimelineStream<V> stream;

private:
    float animationStartTime = -1.0f;
    float startTimeOffset = 0.0f;
    float timeElapsed = 0.0f;
    float meanTimeDelta = 0.0f;
    bool bIsPlaying = false;
    bool bForceOneFrameOnly = false;
    uint64_t frameTickOffset = 0;
    uint64_t frameTickCount = 0;
    TimelineGroupCCF ccf = nullptr;
    TimelineUpdateGroupCCF cuf = nullptr;
};

class Timeline {
public:

    static void update() {
       for( auto& [k,g] : timelinei ) {
           g->update();
       }
       for( auto& [k,g] : timelinef ) {
           g->update();
       }
       for( auto& [k,g] : timelineV2 ) {
           g->update();
       }
       for( auto& [k,g] : timelineV3 ) {
           g->update();
       }
       for( auto& [k,g] : timelineV4 ) {
           g->update();
       }
       for( auto& [k,g] : timelineQ ) {
           g->update();
       }
    }

//    static void play( const std::string & _groupName ) {
//        if ( auto it = timelineGroups.find(_groupName); it != timelineGroups.end() ) {
//            it->second.play();
//        }
//    }
//
//    static void playOneFrame( const std::string & _groupName ) {
//        if ( auto it = timelineGroups.find(_groupName); it != timelineGroups.end() ) {
//            it->second.playOneFrame();
//        }
//    }
//
//    static float groupAnimTime( const std::string & _groupName ) {
//        if ( const auto& it = timelineGroups.find(_groupName); it != timelineGroups.end() ) {
//            return it->second.animationTime();
//        }
//        return -1.0f;
//    }
//
//    static void visitGroup( const std::string& _groupName, AnimVisitCallback _callback ) {
//        if ( const auto& it = timelineGroups.find(_groupName); it != timelineGroups.end() ) {
//            it->second.visit( std::move(_callback) );
//        }
//    }

//    static const TimelineMapSpec& Timelines() { return timelines; }
//    static TimelineMapSpec& TimelinesToUpdate() { return timelines; }
//    static const TimelineGroupMap& Groups() { return timelineGroups; }

//    template <typename T, typename ...Args>
//    static void add( const std::string& _group, AnimValue<T> _source, Args&& ... args ) {
//        addGroupIfEmpty(_group);
//        if constexpr ( sizeof...(Args) == 1 ) {
//            timelineGroups[_group].addTimeline(timelines.add( _source, {0.0f, _source->value} ));
//        }
//        (timelineGroups[_group].addTimeline(timelines.add( _source, args )), ...);
//    }

    template <typename T, typename ...Args>
    static std::string play( AnimValue<T>& _source, Args&& ... args ) {
        auto groupName = UUIDGen::make();
        auto tg = std::make_shared<TimelineGroup<T>>(_source);

        (addParam<T>( tg, std::forward<Args>( args )), ...); // Fold expression (c++17)

        if constexpr (std::is_same_v<T, float>) {
            timelinef.emplace( groupName, tg );
        } else if constexpr (std::is_same_v<T, V2f>) {
            timelineV2.emplace( groupName, tg );
        } else if constexpr (std::is_same_v<T, V3f>) {
            timelineV3.emplace( groupName, tg );
        } else if constexpr (std::is_same_v<T, V4f>) {
            timelineV4.emplace( groupName, tg );
        } else if constexpr (std::is_same_v<T, Quaternion>) {
            timelineQ.emplace( groupName, tg );
        } else if constexpr (std::is_same_v<T, int>) {
            timelinei.emplace( groupName, tg );
        }

        tg->play();
        return groupName;
    }

    template <typename T>
    static void stop( AnimValue<T>& _source, const std::string& groupName, const T& _restoreValue ) {
        if constexpr (std::is_same_v<T, float>) {
            timelinef.erase( groupName );
        } else if constexpr (std::is_same_v<T, V2f>) {
            timelineV2.erase( groupName );
        } else if constexpr (std::is_same_v<T, V3f>) {
            timelineV3.erase( groupName );
        } else if constexpr (std::is_same_v<T, V4f>) {
            timelineV4.erase( groupName );
        } else if constexpr (std::is_same_v<T, Quaternion>) {
            timelineQ.erase( groupName );
        } else if constexpr (std::is_same_v<T, int>) {
            timelinei.erase( groupName );
        }
        _source->value = _restoreValue;
    }

    static void sleep( float _seconds = 0.016f, uint64_t frameSkipper = 0, TimelineGroupCCF _ccf = nullptr ) {
        auto groupName = UUIDGen::make();
        auto sleepingBeauty = std::make_shared<AnimType<float>>(0.0f, "Sleeping");

        play( sleepingBeauty, frameSkipper, _ccf, KeyFramePair{ _seconds, 0.0f } );
    }

    static void intermezzo( float _seconds, uint64_t frameSkipper, AnimUpdateCallback _cuf ) {
        auto groupName = UUIDGen::make();
        auto sleepingBeauty = std::make_shared<AnimType<float>>(0.0f, "Intermezzo");

        play( sleepingBeauty, frameSkipper, KeyFramePair{ _seconds, 0.0f }, _cuf );
    }

    template<typename SGT, typename M>
    static void addParam( std::shared_ptr<TimelineGroup<SGT>> tg, const M& _param ) {
        if constexpr ( std::is_same_v<M, KeyFramePair<SGT>> ) {
            tg->stream.k( { 0.0f, tg->stream.value() } );
            tg->stream.k( _param );
        }
        if constexpr ( std::is_same_v<M, std::vector<KeyFramePair<SGT>>> ) {
            for (const auto& elem : _param ) {
                tg->stream.k( elem );
            }
        }
        if constexpr ( std::is_integral_v<M> ) {
            tg->FrameTickOffset(_param);
        }
        if constexpr ( std::is_floating_point_v<M> ) {
            tg->StartTimeOffset(_param);
        }
        if constexpr ( std::is_same_v<M, AnimUpdateCallback> ) {
            tg->CUF(_param());
        }
        if constexpr ( std::is_same_v<M, AnimEndCallback> ) {
            tg->CCF(_param);
        }
    }

//    static bool deleteKey( const std::string& _group, TimelineIndex _ti, uint64_t _index ) {
//        auto ki = timelines.deleteKey(_ti, _index );
//        if ( const auto& linkedKeys = links.find(_ti); linkedKeys != links.end() ) {
//            for ( const auto& lk : linkedKeys->second ) {
//                ki |= timelines.deleteKey(lk, _index );
//            }
//        }
//        return ki;
//    }
//
//    static void updateKeyTime( const std::string& _group, TimelineIndex _ti, uint64_t _index, float _time ) {
//        timelines.updateKeyTime(_ti, _index, _time );
//        if ( const auto& linkedKeys = links.find(_ti); linkedKeys != links.end() ) {
//            for ( const auto& lk : linkedKeys->second ) {
//                timelines.updateKeyTime(lk, _index, _time );
//            }
//        }
//    }
//
//    template <typename T>
//    static void addLinked( const std::string& _group, std::shared_ptr<T> _linkable, float _time ) {
//        auto lKeys = _linkable->addKeyFrame( _group, _time );
//        for ( const auto& k : lKeys ) {
//            for ( const auto& m : lKeys ) {
//                if ( k != m ) links[k].emplace(m);
//            }
//        }
//    }
//
//    template <typename T>
//    static void deleteLinked( const std::string& _group, std::shared_ptr<T> _linkable, float _time ) {
//        _linkable->deleteKeyFrame( _group, _time );
//    }

private:
//    static void addGroupIfEmpty( const std::string& _group ) {
//        if ( const auto& it = timelineGroups.find(_group); it == timelineGroups.end() ) {
//            timelineGroups.emplace( _group, TimelineGroup{} );
//        }
//    }
private:
//    using TimelineGroupMap = std::unordered_map<std::string, TimelineGroup>;
    static std::unordered_map<std::string, std::shared_ptr<TimelineGroup<int>        >> timelinei;
    static std::unordered_map<std::string, std::shared_ptr<TimelineGroup<float>      >> timelinef;
    static std::unordered_map<std::string, std::shared_ptr<TimelineGroup<Vector2f>   >> timelineV2;
    static std::unordered_map<std::string, std::shared_ptr<TimelineGroup<Vector3f>   >> timelineV3;
    static std::unordered_map<std::string, std::shared_ptr<TimelineGroup<Vector4f>   >> timelineV4;
    static std::unordered_map<std::string, std::shared_ptr<TimelineGroup<Quaternion> >> timelineQ;

//    static TimelineGroupMap timelineGroups;
//    static TimelineMapSpec  timelines;
//    static TimelineLinks    links;

    template <typename U>
    friend class TimelineStream;
};
