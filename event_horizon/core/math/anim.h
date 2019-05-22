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

template <typename T>
class TimelineStream {
public:
    TimelineStream() = default;
    explicit TimelineStream( AnimValue<T> source ) : source( std::move( source )) {}

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
        if ( _timeElapsed < 0.0f ) return KeyFramePosition::Pre;

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

    T valueAt( float _timeElapsed ) {
        auto value = source->value;
        uint64_t keyFrameIndex = 0;
        float delta = 0.0f;
        auto keyFramePos = getKeyFrameIndexAt( _timeElapsed, keyFrameIndex, delta );
        source->isAnimating = keyFramePos == KeyFramePosition::Valid ||
                              ( keyFramePos == KeyFramePosition::Post && !triggeredPost );
        if ( keyFramePos == KeyFramePosition::Post ) triggeredPost = true;
        if ( source->isAnimating ) {
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
using TimelineIndexVector = std::set<TimelineIndex>;

struct TimelineMapSpec {
    TimelineMap<int>        tmapi;
    TimelineMap<float>      tmapf;
    TimelineMap<Vector2f>   tmapV2;
    TimelineMap<Vector3f>   tmapV3;
    TimelineMap<Vector4f>   tmapV4;
    TimelineMap<Quaternion> tmapQ;

    void visit( TimelineIndex _k,  AnimVisitCallback _callback );
    bool update( TimelineIndex _k, float _timeElapsed );
    bool isActive( TimelineIndex _k ) const;
    bool deleteKey( TimelineIndex _k, uint64_t _index );
    void updateKeyTime( TimelineIndex _k, uint64_t _index, float _time );

#define addTimeLineMapValue(tmt) auto it = tmt.find(ti); \
    if ( it == tmt.end() ) { \
        tmt.emplace( ti, _source ); \
        it = tmt.begin(); \
    } \
    it->second.k(_values); \
    return ti; \

    TimelineIndex add( inta _source, const KeyFramePair<int>& _values ) {
        TimelineIndex ti = tiInt + _source->UID();
        addTimeLineMapValue(tmapi)
    }

    TimelineIndex add( floata _source, const KeyFramePair<float>& _values ) {
        TimelineIndex ti = tiFloat + _source->UID();
        addTimeLineMapValue(tmapf)
    }

    TimelineIndex add( V2fa _source, const KeyFramePair<Vector2f>& _values ) {
        TimelineIndex ti = tiV2f + _source->UID();
        addTimeLineMapValue(tmapV2)
    }
    TimelineIndex add( V3fa _source, const KeyFramePair<Vector3f>& _values ) {
        TimelineIndex ti = tiV3f + _source->UID();
        addTimeLineMapValue(tmapV3)
    }
    TimelineIndex add( V4fa _source, const KeyFramePair<Vector4f>& _values ) {
        TimelineIndex ti = tiV4f + _source->UID();
        addTimeLineMapValue(tmapV4)
    }
    TimelineIndex add( Quaterniona _source, const KeyFramePair<Quaternion>& _values ) {
        TimelineIndex ti = tiQuat + _source->UID();
        addTimeLineMapValue(tmapQ)
    }

    static TimelineIndex mkf;
};

using TimelineGroupCCF = std::function<void()>;

class Timeline {
public:
    class TimelineGroup {
    public:
        void play( float _startTimeOffset = 0.0f, uint64_t _frameTickOffset = 0, TimelineGroupCCF _ccf = nullptr);
        void playOneFrame( float _startTimeOffset = 0.0f );
        void update();
        void visit( AnimVisitCallback _callback );
        void addTimeline( TimelineIndex _ti );
        float animationTime() const;

    private:
        TimelineIndexVector timelines;
        float animationStartTime = -1.0f;
        float animationInitialDelay = 0.0f;
        float timeElapsed = 0.0f;
        float meanTimeDelta = 0.0f;
        bool bIsPlaying = false;
        bool bForceOneFrameOnly = false;
        uint64_t frameTickOffset = 0;
        uint64_t frameTickCount = 0;
        TimelineGroupCCF ccf = nullptr;
    };

    using TimelineGroupMap = std::unordered_map<std::string, TimelineGroup>;

    static void update() {
        for( auto& [k,g] : timelineGroups ) {
            g.update();
        }
    }

    static void play( const std::string & _groupName,
                      float _startTimeOffset = 0.0f,
                      uint64_t frameTickOffset = 0,
                      TimelineGroupCCF _ccf = nullptr ) {
        if ( auto it = timelineGroups.find(_groupName); it != timelineGroups.end() ) {
            it->second.play( _startTimeOffset, frameTickOffset, _ccf );
        }
    }

    static void playOneFrame( const std::string & _groupName, float _startTimeOffset ) {
        if ( auto it = timelineGroups.find(_groupName); it != timelineGroups.end() ) {
            it->second.playOneFrame( _startTimeOffset );
        }
    }

    static float groupAnimTime( const std::string & _groupName ) {
        if ( const auto& it = timelineGroups.find(_groupName); it != timelineGroups.end() ) {
            return it->second.animationTime();
        }
        return -1.0f;
    }

    static void visitGroup( const std::string& _groupName, AnimVisitCallback _callback ) {
        if ( const auto& it = timelineGroups.find(_groupName); it != timelineGroups.end() ) {
            it->second.visit( _callback );
        }
    }

    static const TimelineMapSpec& Timelines() { return timelines; }
    static TimelineMapSpec& TimelinesToUpdate() { return timelines; }
    static const TimelineGroupMap& Groups() { return timelineGroups; }

    template <typename T, typename ...Args>
    static void add( const std::string& _group, AnimValue<T> _source, Args&& ... args ) {
        addGroupIfEmpty(_group);
        if constexpr ( sizeof...(Args) == 1 ) {
            timelineGroups[_group].addTimeline(timelines.add( _source, {0.0f, _source->value} ));
        }
        (timelineGroups[_group].addTimeline(timelines.add( _source, args )), ...);
    }

    static bool deleteKey( const std::string& _group, TimelineIndex _ti, uint64_t _index ) {
        auto ki = timelines.deleteKey(_ti, _index );
        if ( const auto& linkedKeys = links.find(_ti); linkedKeys != links.end() ) {
            for ( const auto& lk : linkedKeys->second ) {
                ki |= timelines.deleteKey(lk, _index );
            }
        }
        return ki;
    }

    static void updateKeyTime( const std::string& _group, TimelineIndex _ti, uint64_t _index, float _time ) {
        timelines.updateKeyTime(_ti, _index, _time );
        if ( const auto& linkedKeys = links.find(_ti); linkedKeys != links.end() ) {
            for ( const auto& lk : linkedKeys->second ) {
                timelines.updateKeyTime(lk, _index, _time );
            }
        }
    }

    template <typename T>
    static void addLinked( const std::string& _group, std::shared_ptr<T> _linkable, float _time ) {
        auto lKeys = _linkable->addKeyFrame( _group, _time );
        for ( const auto& k : lKeys ) {
            for ( const auto& m : lKeys ) {
                if ( k != m ) links[k].emplace(m);
            }
        }
    }

    template <typename T>
    static void deleteLinked( const std::string& _group, std::shared_ptr<T> _linkable, float _time ) {
        _linkable->deleteKeyFrame( _group, _time );
    }

private:
    static void addGroupIfEmpty( const std::string& _group ) {
        if ( const auto& it = timelineGroups.find(_group); it == timelineGroups.end() ) {
            timelineGroups.emplace( _group, TimelineGroup{} );
        }
    }
private:
    static TimelineGroupMap timelineGroups;
    static TimelineMapSpec  timelines;
    static TimelineLinks    links;

    template <typename U>
    friend class TimelineStream;
};
