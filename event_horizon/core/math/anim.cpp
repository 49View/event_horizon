#include "anim.h"

#include <core/game_time.h>

uint64_t Timeline::TimelineMapSpec::mkf = 1;

Timeline::TimelineMapSpec Timeline::timelines;
std::unordered_set<TimelineIndex> Timeline::activeTimelines;
std::unordered_map<std::string, TimelineIndexVector> Timeline::timelineGroups;

//std::map<std::string, float> AnimUtil::toggleDelayForInverseMap;
//
//void AnimTimeline::addToStream( float animTime, float animDelay, std::function<void( float, float )> f ) {
//	if ( entries.size() == 0 ) { // In case we allocate the first stream with this function
//		addStream();
//	}
//	std::vector<AnimTimelineEntry>& currEntry = entries.back();
//	currEntry.push_back( { animTime, animDelay, f } );
//	maxStreamTime = max( animTime + animDelay, maxStreamTime );
//}

//template <typename T>
//void AnimTimeline::addToStream(std::shared_ptr<AnimType<T>> source_shared, const T& to, float animTime, float delay, AnimLoopType lt) {
//	addToStream(animTime, delay, [&](float _animTime, float _delay) { animateTo(source_shared, to, _animTime, _delay, lt); });
//}

//void AnimTimeline::addStream() {
//	currTimePointer += maxStreamTime;
//	maxStreamTime = 0.0f;
//	timeline.push_back( currTimePointer );
//	entries.push_back( std::vector<AnimTimelineEntry>() );
//}
//
//void AnimTimeline::addStream( float animTime, float animDelay, std::function<void( float, float )> f ) {
//	addToStream( animTime, animDelay, f );
//	addStream();
//}
//
//void AnimTimeline::start() {
//	currTimePointer = 0.0f;
//	streamIndexPlaying = 0;
//	isPlaying = true;
//}
//
//void AnimTimeline::update( float deltaTime ) {
//	if ( isPlaying ) {
//		if ( currTimePointer + deltaTime > timeline[streamIndexPlaying] ) {
//			if ( !AnimUpdateTimeline::isAnythingRunningFromTimeline() ) {
//				for ( auto& w : entries[streamIndexPlaying] ) {
//					w.animFunction( w.animTime, w.animDelay );
//				}
//				streamIndexPlaying++;
//				if ( static_cast<size_t >(streamIndexPlaying) >= timeline.size() ) {
//					isPlaying = false;
//					needsToBeRemoved = true;
//					if ( endTimelineCallback ) {
//						endTimelineCallback();
//					}
//				}
//			}
//		}
//		currTimePointer += deltaTime;
//	}
//}
//
//std::vector<std::shared_ptr<AnimTimeline>>	  AnimUpdateTimeline::timelines;
//std::set<std::shared_ptr<AnimType<float>>>	  AnimUpdateTimeline::timelineF;
//std::set<std::shared_ptr<AnimType<Vector2f>>> AnimUpdateTimeline::timelineV2;
//std::set<std::shared_ptr<AnimType<Vector3f>>> AnimUpdateTimeline::timelineV3;
//std::set<std::shared_ptr<AnimType<Vector4f>>> AnimUpdateTimeline::timelineV4;
//
//std::map<uint64_t, std::shared_ptr<AnimType<float>>>    AnimUpdateTimeline::smapF;
//std::map<uint64_t, std::shared_ptr<AnimType<Vector2f>>> AnimUpdateTimeline::smapV2;
//std::map<uint64_t, std::shared_ptr<AnimType<Vector3f>>> AnimUpdateTimeline::smapV3;
//std::map<uint64_t, std::shared_ptr<AnimType<Vector4f>>> AnimUpdateTimeline::smapV4;
//
//uint64_t AnimUpdateTimeline::mkf = 0;
//
//void AnimUpdateTimeline::startTimeline( std::shared_ptr<AnimTimeline> tl ) {
//	tl->start();
//	timelines.push_back( tl );
//}
//
//std::shared_ptr<AnimType<float>> AnimUpdateTimeline::create( float val ) {
//	auto ret = std::make_shared<AnimType<float>>(val);
//	smapF[mkf++] = ret;
//	return ret;
//}
//
//std::shared_ptr<AnimType<Vector2f>> AnimUpdateTimeline::create( Vector2f val ) {
//	auto ret = std::make_shared<AnimType<Vector2f>>(val);
//	smapV2[mkf++] = ret;
//	return ret;
//}
//
//std::shared_ptr<AnimType<Vector3f>> AnimUpdateTimeline::create( Vector3f val ) {
//	auto ret = std::make_shared<AnimType<Vector3f>>(val);
//	smapV3[mkf++] = ret;
//	return ret;
//}
//
//std::shared_ptr<AnimType<Vector4f>> AnimUpdateTimeline::create( Vector4f val ) {
//	auto ret = std::make_shared<AnimType<Vector4f>>(val);
//	smapV4[mkf++] = ret;
//	return ret;
//}
//
//void AnimUpdateTimeline::insert( std::shared_ptr<AnimType<float>>& anim ) {
//	timelineF.insert( anim );
//}
//
//void AnimUpdateTimeline::insert( std::shared_ptr<AnimType<Vector2f>>& anim ) {
//	timelineV2.insert( anim );
//}
//
//void AnimUpdateTimeline::insert( std::shared_ptr<AnimType<Vector3f>>& anim ) {
//	timelineV3.insert( anim );
//}
//
//void AnimUpdateTimeline::insert( std::shared_ptr<AnimType<Vector4f>>& anim ) {
//	timelineV4.insert( anim );
//}
//
//void AnimUpdateTimeline::remove( std::shared_ptr<AnimType<float>>& anim ) {
//	timelineF.erase( timelineF.find( anim ) );
//}
//
//void AnimUpdateTimeline::remove( std::shared_ptr<AnimType<Vector2f>>& anim ) {
//	timelineV2.erase( timelineV2.find( anim ) );
//}
//
//void AnimUpdateTimeline::remove( std::shared_ptr<AnimType<Vector3f>>& anim ) {
//	timelineV3.erase( timelineV3.find( anim ) );
//}
//
//void AnimUpdateTimeline::remove( std::shared_ptr<AnimType<Vector4f>>& anim ) {
//	timelineV4.erase( timelineV4.find( anim ) );
//}
//
//void AnimUpdateTimeline::toggle( const std::string& name ) {
//	for ( auto& anim : timelineF ) {
//		if ( anim->name == name ) anim->toggle();
//	}
//	for ( auto& anim : timelineV2 ) {
//		if ( anim->name == name ) anim->toggle();
//	}
//	for ( auto& anim : timelineV3 ) {
//		if ( anim->name == name ) anim->toggle();
//	}
//	for ( auto& anim : timelineV4 ) {
//		if ( anim->name == name ) anim->toggle();
//	}
//}
//
//void AnimUpdateTimeline::update() {
//	// Removes, for some reason I don't understand I cannot use remove_if, bugger
//	// ND Dado: here is the reason: http://stackoverflow.com/questions/11141651/why-cant-i-remove-a-string-from-a-stdset-with-stdremove-if
//	float delta = GameTime::getCurrTimeStep();
//	for ( auto it = timelineF.begin(); it != timelineF.end();) {
//		if ( ( *it )->NeedsToBeRemoved() ) { it = timelineF.erase( it ); } else { ++it; }
//	}
//	for ( auto it = timelineV2.begin(); it != timelineV2.end();) {
//		if ( ( *it )->NeedsToBeRemoved() ) { it = timelineV2.erase( it ); } else { ++it; }
//	}
//	for ( auto it = timelineV3.begin(); it != timelineV3.end();) {
//		if ( ( *it )->NeedsToBeRemoved() ) { it = timelineV3.erase( it ); } else { ++it; }
//	}
//	for ( auto it = timelineV4.begin(); it != timelineV4.end();) {
//		if ( ( *it )->NeedsToBeRemoved() ) { it = timelineV4.erase( it ); } else { ++it; }
//	}
//	for ( auto it = timelines.begin(); it != timelines.end();) {
//		if ( ( *it )->NeedsToBeRemoved() ) { it = timelines.erase( it ); } else { ++it; }
//	}
//
//	for ( auto& anim : timelineF ) {
//		anim->update( delta );
//	}
//	for ( auto& anim : timelineV2 ) {
//		anim->update( delta );
//	}
//	for ( auto& anim : timelineV3 ) {
//		anim->update( delta );
//	}
//	for ( auto& anim : timelineV4 ) {
//		anim->update( delta );
//	}
//	for ( auto& anim : timelines ) {
//		anim->update( delta );
//	}
//}
//
//bool AnimUpdateTimeline::isAnythingRunningFromTimeline() {
//	for ( auto& anim : timelineF ) {
//		if ( anim->isAnimating() && anim->BelongsToTimeline() ) return true;
//	}
//	for ( auto& anim : timelineV2 ) {
//		if ( anim->isAnimating() && anim->BelongsToTimeline() ) return true;
//	}
//	for ( auto& anim : timelineV3 ) {
//		if ( anim->isAnimating() && anim->BelongsToTimeline() ) return true;
//	}
//	for ( auto& anim : timelineV4 ) {
//		if ( anim->isAnimating() && anim->BelongsToTimeline() ) return true;
//	}
//	return false;
//}
//
//template <typename T>
//void AnimType<T>::update( float delta ) {
//	if ( isAnimating() ) {
//		deltaTime += delta;
//
//		// This is to handle delay
//		if ( deltaTime - delayTime < 0.0f ) {
//			return;
//		}
//		if ( firstFrameOfAnimation ) {
//			if ( startAnimCallback1 != nullptr ) startAnimCallback1( value );
//			firstFrameOfAnimation = false;
//		}
//
//		if ( loopType == AnimLoopType::Reverse ) {
//			if ( deltaTime >= finalTime*0.5f ) {
//				keyframes[1] = keyframes[0];
//			}
//		}
//
//		float deltaTimeNotIncludingDelay = deltaTime - delayTime;
//		switch ( velocityType ) {
//			case AnimVelocityType::Linear:
//			value = JMATH::lerp( JMATH::saturate( deltaTimeNotIncludingDelay / finalTime ), keyframes[0], keyframes[1] );
//			break;
//			case AnimVelocityType::Cosine:
//			value = JMATH::lerp( asinf( JMATH::saturate( deltaTimeNotIncludingDelay / finalTime ) ) / M_PI_2, keyframes[0], keyframes[1] );
//			break;
//			case AnimVelocityType::Exp:
//			value = JMATH::lerp( ( expf( JMATH::saturate( deltaTimeNotIncludingDelay / finalTime ) ) - 1.0f ) / ( M_E - 1.0f ), keyframes[0], keyframes[1] );
//			break;
//			case AnimVelocityType::Hermite:
////			value = traversePathHermite( cameraPath, deltaTimeNotIncludingDelay );
////			value = JMATH::lerp( ( expf( JMATH::saturate( deltaTimeNotIncludingDelay / finalTime ) ) - 1.0f ) / ( M_E - 1.0f ), source, target );
//			break;
//			default:
//			break;
//		}
//
//		if ( deltaTimeNotIncludingDelay > finalTime ) {
//			if ( lastFrameOfAnimation ) deltaTime = 0.0f;
//			switch ( loopType ) {
//			case AnimLoopType::Linear:
//			case AnimLoopType::Reverse:
//			finalTime = 0.0f;
//			lastFrameOfAnimation = true;
//			needsToBeRemoved = true;
//			break;
//
//			case AnimLoopType::Loop:
//			if ( lastFrameOfAnimation ) {
//				if ( endAnimCallback1 ) endAnimCallback1( value );
//				lastFrameOfAnimation = false;
//			} else {
//				lastFrameOfAnimation = true;
//			}
//			break;
//			case AnimLoopType::Bounce:
//			if ( numBounces > 0 ) {
//				bToggleWay = !bToggleWay;
//				delayTime = 0.0f;
//				deltaTime = 0.0f;
//				finalTime = finalTimeToBe * numBounces;
//				if ( bToggleWay ) {
//					keyframes[1] = origTarget * ( 1.0f - ( bouncePerc * ( 1.0f / ( origNumBounces - ( numBounces - 1 ) ) ) ) );
//					keyframes[0] = value;
//				} else {
//					keyframes[1] = origTarget + ( origTarget * ( bouncePerc * ( 1.0f - ( 1.0f / numBounces ) ) ) );
//					keyframes[0] = value;
//				}
//				//						LOGI("Bouce %d finalTime %f source %f value %f target %f", numBounces, finalTime, source, value, target);
//				numBounces--;
//			} else {
//				finalTime = 0.0f;
//				needsToBeRemoved = true;
//			}
//			break;
//			case AnimLoopType::Toggle:
//			if ( lastFrameOfAnimation ) {
//				finalTime = 0.0f;
//				if ( !bToggleWay ) {
//					if ( endAnimCallback1 ) endAnimCallback1( value );
//				} else {
//					if ( endAnimCallback2 ) endAnimCallback2( value );
//				}
//				bToggleWay = !bToggleWay;
//				lastFrameOfAnimation = false;
//			} else {
//				lastFrameOfAnimation = true;
//			}
//			break;
//			default:
//			break;
//			}
//		}
//
//		if ( updateAnimCallback ) updateAnimCallback( value );
//		if ( lastFrameOfAnimation ) {
//			if ( endAnimCallback1 ) endAnimCallback1( value );
//		}
//	}
//}


void Timeline::TimelineMapSpec::update( TimelineIndex _k ) {
    auto ki = _k / tiNorm;
    switch (ki) {
        case tiIntIndex:
            tmapi[_k].update( GameTime::getCurrTimeStamp() );
            break;
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

void Timeline::TimelineMapSpec::reset( TimelineIndex _k ) {
    auto ki = _k / tiNorm;
    switch (ki) {
        case tiIntIndex:
            tmapi[_k].reset(GameTime::getCurrTimeStamp());
            break;
        case tiFloatIndex:
            tmapf[_k].reset(GameTime::getCurrTimeStamp());
            break;
        case tiV2fIndex:
            tmapV2[_k].reset(GameTime::getCurrTimeStamp());
            break;
        case tiV3fIndex:
            tmapV3[_k].reset(GameTime::getCurrTimeStamp());
            break;
        case tiV4fIndex:
            tmapV4[_k].reset(GameTime::getCurrTimeStamp());
            break;
        case tiQuatIndex:
            tmapQ[_k].reset(GameTime::getCurrTimeStamp());
            break;
        default:
            break;
    }
}

bool Timeline::TimelineMapSpec::isActive( TimelineIndex _k ) const {
    auto ki = _k / tiNorm;
    switch (ki) {
        case tiIntIndex:
            return tmapi.at(_k).isActive();
        case tiFloatIndex:
            return tmapf.at(_k).isActive();
        case tiV2fIndex:
            return tmapV2.at(_k).isActive();
        case tiV3fIndex:
            return tmapV3.at(_k).isActive();
        case tiV4fIndex:
            return tmapV4.at(_k).isActive();
        case tiQuatIndex:
            return tmapQ.at(_k).isActive();
        default:
            break;
    }
    return false;
}

void Timeline::TimelineMapSpec::visit( TimelineIndex _k, AnimVisitCallback _callback ) {
    auto ki = _k / tiNorm;
    switch (ki) {
        case tiIntIndex:
            tmapi.at(_k).visit(_callback, tiFloatIndex);
            break;
        case tiFloatIndex:
            tmapf.at(_k).visit(_callback, tiFloatIndex);
            break;
        case tiV2fIndex:
            tmapV2.at(_k).visit(_callback, tiV2fIndex);
            break;
        case tiV3fIndex:
            tmapV3.at(_k).visit(_callback, tiV3fIndex);
            break;
        case tiV4fIndex:
            tmapV4.at(_k).visit(_callback, tiV4fIndex);
            break;
        case tiQuatIndex:
            tmapQ.at(_k).visit(_callback, tiQuatIndex);
            break;
        default:
            break;
    }
}
