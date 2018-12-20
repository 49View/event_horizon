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

//class AnimUtil {
//public:
//	static std::map<std::string, float> toggleDelayForInverseMap;
//
//	static void setToggleDelayForInverse( const std::string& name, float delay ) {
//		if ( toggleDelayForInverseMap.empty()) {
//			toggleDelayForInverseMap[name] = delay;
//			return;
//		}
//		if ( toggleDelayForInverseMap[name] < delay ) toggleDelayForInverseMap[name] = delay;
//	}
//
//	static float ToggleDelayForInverse( const std::string& name ) {
//		return toggleDelayForInverseMap[name];
//	}
//};
//
//template <typename T> class AnimType;
//
//class AnimTimelineEntry {
//public:
//	float animTime;
//	float animDelay;
//	std::function<void( float, float )> animFunction;
//};
//
//template <typename T, typename F>
//static void animateFromTo( const T& from, const T& to, float animTime, float delay, F cbu, AnimLoopType lt = AnimLoopType::Linear );
//
//template< typename T, typename F>
//std::function<void( float, float )> afsetF( T _fromValue, T _value, F f ) {
//	return [=]( float _animTime, float _delay ) { animateFromTo( _fromValue, _value, _animTime, _delay, f ); };
//}
//
//class AnimTimeline {
//public:
//	AnimTimeline() {
//		endTimelineCallback = nullptr;
//	}
//
//	void addStream();
//
//	template <typename T>
//	void addToStream( std::shared_ptr<AnimType<T>> source_shared, const T& to, float animTime, float delay = 0.0f, AnimLoopType lt = AnimLoopType::Linear ) {
//		addToStream( animTime, delay, [source_shared, to, lt]( float _animTime, float _delay ) { animateTo( source_shared, to, _animTime, _delay, lt ); } );
//	}
//
//	template <typename T>
//	void addToStream( std::shared_ptr<AnimType<T>> source_shared, const T& from, const T& to, float animTime, float delay = 0.0f, AnimLoopType lt = AnimLoopType::Linear ) {
//		addToStream( animTime, delay, [source_shared, from, to, lt]( float _animTime, float _delay ) { animateFromTo( source_shared, from, to, _animTime, _delay, nullptr, lt ); } );
//	}
//
//	template< typename T, typename F>
//	void addToStream( float animTime, const T& _from, const T& _to, F f ) {
//		addToStream( animTime, 0.0f, afsetF<T>( _from, _to, f ) );
//	}
//
//	template< typename T, typename F>
//	void addToStream( float animTime, float delay, const T& _from, const T& _to, F f ) {
//		addToStream( animTime, delay, afsetF<T>( _from, _to, f ) );
//	}
//
//	template <typename T>
//	void addStream( std::shared_ptr<AnimType<T>> source_shared, const T& to, float animTime, float delay = 0.0f, AnimLoopType lt = AnimLoopType::Linear ) {
//		addToStream( animTime, delay, [source_shared, to, lt]( float _animTime, float _delay ) { animateTo( source_shared, to, _animTime, _delay, lt ); } );
//		addStream();
//	}
//
//	template< typename T, typename F>
//	void addStream( float animTime, const T& _from, const T& _to, F f ) {
//		addStream( animTime, 0.0f, afsetF<T>( _from, _to, f ) );
//	}
//
//	template< typename T, typename F>
//	void addStream( float animTime, float _delay, const T& _from, const T& _to, F f ) {
//		addStream( animTime, _delay, afsetF<T>( _from, _to, f ) );
//	}
//
//	void start();
//	void update( float deltaTime );
//	bool NeedsToBeRemoved() const { return needsToBeRemoved; }
//	void setEndTimelineCallback( std::function<void( void )> f ) {
//		//addStream();
//		endTimelineCallback = f;
//	}
//private:
//	void addToStream( float animTime, float animDelay, std::function<void( float, float )> f );
//	void addStream( float animTime, float animDelay, std::function<void( float, float )> f );
//
//private:
//	float currTimePointer = 0.0f;
//	float maxStreamTime = 0.0f;
//	int   streamIndexPlaying = 0;
//	bool  isPlaying = false;
//	bool  needsToBeRemoved = false;
//
//	std::function<void()> endTimelineCallback;
//
//	std::vector<float> timeline;
//	std::vector<std::vector<AnimTimelineEntry>> entries;
//};

//class AnimUpdateTimeline {
//public:
//	static void toggle( const std::string& name );
//	static void update();
//	static bool isAnythingRunningFromTimeline();
//	static void removeAnim( const std::string& name );
//	static void startTimeline( std::shared_ptr<AnimTimeline> tl );
//
//	static std::shared_ptr<AnimType<float>>    create( float val );
//	static std::shared_ptr<AnimType<Vector2f>> create( Vector2f val );
//	static std::shared_ptr<AnimType<Vector3f>> create( Vector3f val );
//	static std::shared_ptr<AnimType<Vector4f>> create( Vector4f val );
//
//	static void insert( std::shared_ptr<AnimType<float>>&    anim );
//	static void insert( std::shared_ptr<AnimType<Vector2f>>& anim );
//	static void insert( std::shared_ptr<AnimType<Vector3f>>& anim );
//	static void insert( std::shared_ptr<AnimType<Vector4f>>& anim );
//
//	static void remove( std::shared_ptr<AnimType<float>>& anim );
//	static void remove( std::shared_ptr<AnimType<Vector2f>>& anim );
//	static void remove( std::shared_ptr<AnimType<Vector3f>>& anim );
//	static void remove( std::shared_ptr<AnimType<Vector4f>>& anim );
//
//private:
//	static std::vector<std::shared_ptr<AnimTimeline>> timelines;
//
//	static std::set<std::shared_ptr<AnimType<float>>> timelineF;
//	static std::set<std::shared_ptr<AnimType<Vector2f>>> timelineV2;
//	static std::set<std::shared_ptr<AnimType<Vector3f>>> timelineV3;
//	static std::set<std::shared_ptr<AnimType<Vector4f>>> timelineV4;
//
//	static std::map<uint64_t, std::shared_ptr<AnimType<float>>>    smapF;
//	static std::map<uint64_t, std::shared_ptr<AnimType<Vector2f>>> smapV2;
//	static std::map<uint64_t, std::shared_ptr<AnimType<Vector3f>>> smapV3;
//	static std::map<uint64_t, std::shared_ptr<AnimType<Vector4f>>> smapV4;
//
//	static uint64_t mkf;
//};


//protected:
//	std::vector<T> keyframes;
//	std::string name;
//	T origTarget;
//	float deltaTime;
//	float finalTime;
//	float finalTimeToBe;
//	float delayTime;
//	float bouncePerc;
//	int   numBounces;
//	int   origNumBounces;
//
//	std::function<void( T )> endAnimCallback1;
//	std::function<void( T )> startAnimCallback1;
//
//	std::function<void( T )> endAnimCallback2;
//	std::function<void( T )> startAnimCallback2;
//
//	std::function<void( T )> updateAnimCallback;
//
//	AnimLoopType		   loopType;
//	AnimVelocityType	   velocityType;
//
//	bool bToggleWay;
//	bool lastFrameOfAnimation;
//	bool firstFrameOfAnimation;
//	bool needsToBeRemoved;
//	bool belongsToTimeline;
//
//public:
//	AnimType() {
//		init();
//	}
//
//	AnimType( const T& v ) {
//		value = v;
//		init();
//	}
//
//	AnimType& operator=( const T& v ) {
//		value = v;
//		return *this;
//	}
//
//	void init() {
//		finalTime = 0.0f;
//		finalTimeToBe = 0.0f;
//		deltaTime = 0.0f;
//		delayTime = 0.0f;
//		name = "";
//		bToggleWay = false;
//		lastFrameOfAnimation = false;
//		needsToBeRemoved = false;
//		belongsToTimeline = false;
//		velocityType = AnimVelocityType::Exp;
//		bouncePerc = 0.1f;
//		numBounces = 0;
//	}
//
//	const std::string& Name() const {
//		return name;
//	}
//	void Name( std::string _name ) {
//		name = _name;
//	}
//
//	void VelocityType( AnimVelocityType vt ) {
//		velocityType = vt;
//	}
//
//	bool BelongsToTimeline() const { return belongsToTimeline; }
//	void BelongsToTimeline( bool _val ) { belongsToTimeline = _val; }
//
//	void setBouncesParams( int _numBounces, float _bouncePerc, T to, float animTime ) {
//		numBounces = _numBounces;
//		bouncePerc = _bouncePerc;
//		origTarget = to;
//		origNumBounces = numBounces;
//		finalTimeToBe = animTime;
//	}
//
//	void update( float delta );
//
//	bool isAnimating() const {
//		return finalTime > 0.0f && !needsToBeRemoved;
//	}
//
//	bool NeedsToBeRemoved() const { return needsToBeRemoved; }
//
//	void set( const T& from, const T& to, float animTime, float delay = 0.0f, std::function<void( T )> cbe = nullptr, std::function<void( T )> cba = nullptr, std::function<void( T )> cbu = nullptr, AnimLoopType lt = AnimLoopType::Linear ) {
//		finalTime = animTime;
//		deltaTime = 0.0f;
//		keyframes.clear();
//		keyframes.push_back( from );
//		keyframes.push_back( to );
//		delayTime = delay;
//		endAnimCallback1 = cbe;
//		startAnimCallback1 = cba;
//		updateAnimCallback = cbu;
//		loopType = lt;
//		needsToBeRemoved = false;
//		bToggleWay = false;
//		lastFrameOfAnimation = false;
//		firstFrameOfAnimation = true;
//	}
//
//	void setTogglePrivate( const std::string& animName, const T& from, const T& to, float animTime, float delay = 0.0f, std::function<void( T )> cbe1 = nullptr, std::function<void( T )> cba1 = nullptr, std::function<void( T )> cbe2 = nullptr, std::function<void( T )> cba2 = nullptr, std::function<void( T )> cbu = nullptr ) {
//		name = animName;
//		keyframes.clear();
//		keyframes.push_back( to );
//		keyframes.push_back( from );
//		delayTime = delay;
//		finalTimeToBe = animTime;
//
//		endAnimCallback1 = cbe1;
//		startAnimCallback1 = cba1;
//
//		endAnimCallback2 = cbe2;
//		startAnimCallback2 = cba2;
//
//		updateAnimCallback = cbu;
//
//		loopType = AnimLoopType::Toggle;
//		bToggleWay = false;
//		needsToBeRemoved = false;
//		lastFrameOfAnimation = false;
//		firstFrameOfAnimation = true;
//	}
//
//protected:
//
//	void toggle() {
//		if ( finalTime > 0.0f ) {
//			bToggleWay ? keyframes[0] = value : keyframes[1] = value;
//		}
//		finalTime = finalTimeToBe;
//		std::swap( keyframes[0], keyframes[1] );
//		deltaTime = 0.0f;
//		if ( !bToggleWay ) {
//			if ( startAnimCallback1 ) startAnimCallback1( value );
//		} else {
//			delayTime = AnimUtil::ToggleDelayForInverse( name ) - ( delayTime + finalTime );
//			if ( startAnimCallback2 ) startAnimCallback2( value );
//		}
//		lastFrameOfAnimation = false;
//	}
//
//	friend class AnimUpdateTimeline;
//};
//
//template <typename T>
//static bool runIfInstant( std::shared_ptr<AnimType<T>> source_shared, float animTime, const T& to, std::function<void( T )> cbe = nullptr ) {
//	if ( animTime == 0.0f ) {
//		source_shared->value = to;
//		std::function<void( T )> endAnimCallback1 = cbe;
//		if ( endAnimCallback1 != nullptr ) cbe( to );
//		return true;
//	}
//	return false;
//}
//
//template <typename T>
//static void animateTo( std::shared_ptr<AnimType<T>> source_shared, const T& to, float animTime, float delay = 0.0f, AnimLoopType lt = AnimLoopType::Linear ) {
//	if ( !runIfInstant( source_shared, animTime, to ) ) {
//		source_shared->set( source_shared->value, to, animTime, delay, nullptr, nullptr, nullptr, lt );
//		AnimUpdateTimeline::insert( source_shared );
//	}
//}
//
//template <typename T, typename F, typename F1, typename F2>
//static void animateTo( std::shared_ptr<AnimType<T>> source_shared, const T& to, float animTime, float delay, F cbe, F1 cba, F2 cbu, AnimLoopType lt = AnimLoopType::Linear ) {
//	std::function<void( T )> cbeCallback = cbe;
//	if ( !runIfInstant( source_shared, animTime, to, cbeCallback ) ) {
//		source_shared->set( source_shared->value, to, animTime, delay, cbe, cba, cbu, lt );
//		AnimUpdateTimeline::insert( source_shared );
//	}
//}
//
//template <typename T, typename F>
//static void animateTo( std::shared_ptr<AnimType<T>> source_shared, const T& to, float animTime, float delay, F cbu, AnimLoopType lt = AnimLoopType::Linear ) {
//	std::function<void( T )> cbuCallback = cbu;
//	if ( !runIfInstant( source_shared, animTime, to, cbuCallback ) ) {
//		source_shared->set( source_shared->value, to, animTime, delay, nullptr, nullptr, cbu, lt );
//		AnimUpdateTimeline::insert( source_shared );
//	}
//}
//
//template <typename T, typename F>
//static void animateFromTo( const T& from, const T& to, float animTime, float delay, F cbu, AnimLoopType lt ) {
//	std::shared_ptr<AnimType<T>> source_shared = AnimUpdateTimeline::create( from );
//	std::function<void( T )> cbuCallback = cbu;
//	if ( !runIfInstant( source_shared, animTime, to, cbuCallback ) ) {
//		source_shared->set( source_shared->value, to, animTime, delay, nullptr, nullptr, cbu, lt );
//		AnimUpdateTimeline::insert( source_shared );
//	}
//}
//
//template <typename T>
//static void animateTo( std::shared_ptr<AnimType<T>> source_shared, const std::string& animName, const T to, float animTime, float delay = 0.0f, AnimLoopType lt = AnimLoopType::Linear ) {
//	if ( !runIfInstant( source_shared, animTime, to ) ) {
//		source_shared->Name( animName );
//		source_shared->set( source_shared->value, to, animTime, delay, nullptr, nullptr, nullptr, lt );
//		AnimUpdateTimeline::insert( source_shared );
//	}
//}
//
//template <typename T, typename F>
//void animateFromTo( std::shared_ptr<AnimType<T>> source_shared, const T& from, const T& to, float animTime, float delay, F cbe, F cba, F cbu, AnimLoopType lt = AnimLoopType::Linear ) {
//	std::function<void( T )> cbeCallback = cbe;
//	if ( !runIfInstant( source_shared, animTime, to, cbeCallback ) ) {
//		source_shared->set( from, to, animTime, delay, cbe, cba, cbu, lt );
//		AnimUpdateTimeline::insert( source_shared );
//	}
//}
//
//template <typename T, typename F>
//void animateFromTo( std::shared_ptr<AnimType<T>> source_shared, const T& from, const T& to, float animTime, float delay, F cbu = nullptr, AnimLoopType lt = AnimLoopType::Linear ) {
//	std::function<void( T )> cbuCallback = cbu;
//	if ( !runIfInstant( source_shared, animTime, to, cbuCallback ) ) {
//		source_shared->set( from, to, animTime, delay, nullptr, nullptr, cbu, lt );
//		AnimUpdateTimeline::insert( source_shared );
//	}
//}
//
//template <typename T, typename F>
//void animateFromToTimeline( std::shared_ptr<AnimType<T>> source_shared, const T& from, const T& to, float animTime, float delay, F cbu = nullptr, AnimLoopType lt = AnimLoopType::Linear ) {
//	std::function<void( T )> cbuCallback = cbu;
//	if ( !runIfInstant( source_shared, animTime, to, cbuCallback ) ) {
//		source_shared->set( from, to, animTime, delay, nullptr, nullptr, cbu, lt );
//		source_shared->BelongsToTimeline( true );
//		AnimUpdateTimeline::insert( source_shared );
//	}
//}
//
//template <typename T, typename F>
//void animateFromToBouncing( std::shared_ptr<AnimType<T>> source_shared, const T& from, const T& to, float animTime, float delay, int _numBounces, float _bouncePerc, F cbe, F cba, F cbu ) {
//	source_shared->setBouncesParams( _numBounces, _bouncePerc, to, animTime );
//	source_shared->set( from, to*( 1.0f + _bouncePerc ), animTime, delay, cbe, cba, cbu, AnimLoopType::Bounce );
//	AnimUpdateTimeline::insert( source_shared );
//}
//
//template <typename T, typename F>
//void animateFromToBouncing( std::shared_ptr<AnimType<T>> source_shared, const T& from, const T& to, float animTime, float delay, int _numBounces, float _bouncePerc, F cbu ) {
//	source_shared->setBouncesParams( _numBounces, _bouncePerc, to, animTime );
//	source_shared->set( from, to*( 1.0f + _bouncePerc ), animTime, delay, nullptr, nullptr, cbu, AnimLoopType::Bounce );
//	AnimUpdateTimeline::insert( source_shared );
//}
//
//template <typename T, typename F>
//void animateFromToAndUpdate( std::shared_ptr<AnimType<T>> source_shared, const T& from, const T& to, float animTime, float delay, F cbu, AnimLoopType lt = AnimLoopType::Linear ) {
//	source_shared->set( from, to, animTime, delay, nullptr, nullptr, cbu, lt );
//	AnimUpdateTimeline::insert( source_shared );
//}
//
//template <typename T, typename F>
//void animateFromToAndUpdateEnd( std::shared_ptr<AnimType<T>> source_shared, const T& from, const T& to, float animTime, float delay, F cbu, F cbe = nullptr, AnimLoopType lt = AnimLoopType::Linear ) {
//	source_shared->set( from, to, animTime, delay, cbe, nullptr, cbu, lt );
//	AnimUpdateTimeline::insert( source_shared );
//}
//
//template <typename T>
//void setToggle( std::shared_ptr<AnimType<T>> source_shared, const std::string& animName, const T& from, const T& to, float _finalTimeToBe, float _delay = 0.0f ) {
//	source_shared->setTogglePrivate( animName, from, to, _finalTimeToBe, _delay, nullptr, nullptr, nullptr, nullptr, nullptr );
//	AnimUtil::setToggleDelayForInverse( animName, _delay + _finalTimeToBe );
//	AnimUpdateTimeline::insert( source_shared );
//}
//
//template <typename T, typename F>
//void setToggle( std::shared_ptr<AnimType<T>> source_shared, const std::string& animName, const T& from, const T& to, float _finalTimeToBe, float _delay, F cbu ) {
//	source_shared->setTogglePrivate( animName, from, to, _finalTimeToBe, _delay, nullptr, nullptr, nullptr, nullptr, cbu );
//	AnimUtil::setToggleDelayForInverse( animName, _delay + _finalTimeToBe );
//	AnimUpdateTimeline::insert( source_shared );
//}
//
//template <typename T, typename F>
//void setToggle( std::shared_ptr<AnimType<T>> source_shared, const std::string& animName, const T& from, const T& to, float _finalTimeToBe, float _delay,
//				F cba1, F cbe1, F cba2, F cbe2, F cbu ) {
//	source_shared->setTogglePrivate( animName, from, to, _finalTimeToBe, _delay, cbe1, cba1, cbe2, cba2, cbu );
//	AnimUtil::setToggleDelayForInverse( animName, _delay + _finalTimeToBe );
//	AnimUpdateTimeline::insert( source_shared );
//}

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
        mbActive = getKeyFrameIndexAt( _timeElapsed, keyFrameIndex, delta );
        if ( mbActive ) {
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
    bool mbActive = true;
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
        TimelineMap<Vector2f>        tmapV2;
        TimelineMap<Vector3f>        tmapV3;
        TimelineMap<Vector4f>        tmapV4;
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

        void update( TimelineIndex _k );

        static TimelineIndex add( const TimelineStream<float>& _stream ) {
            TimelineIndex i = tiFloat + TimelineMapSpec::mkf++;
            timelines.tmapf.insert( { i, _stream } );
            return i;
        }

        static TimelineIndex add( const TimelineStream<Vector2f>& _stream ) {
            TimelineIndex i = tiV2f + TimelineMapSpec::mkf++;
            timelines.tmapV2.insert( { i, _stream } );
            return i;
        }
        static TimelineIndex add( const TimelineStream<Vector3f>& _stream ) {
            TimelineIndex i = tiV3f + TimelineMapSpec::mkf++;
            timelines.tmapV3.insert( { i, _stream } );
            return i;
        }
        static TimelineIndex add( const TimelineStream<Vector4f>& _stream ) {
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
