#include "anim.h"

#include <core/game_time.h>

std::unordered_map<std::string, std::shared_ptr<TimelineGroup<int>        >> Timeline::timelinei;
std::unordered_map<std::string, std::shared_ptr<TimelineGroup<float>      >> Timeline::timelinef;
std::unordered_map<std::string, std::shared_ptr<TimelineGroup<Vector2f>   >> Timeline::timelineV2;
std::unordered_map<std::string, std::shared_ptr<TimelineGroup<Vector3f>   >> Timeline::timelineV3;
std::unordered_map<std::string, std::shared_ptr<TimelineGroup<Vector4f>   >> Timeline::timelineV4;
std::unordered_map<std::string, std::shared_ptr<TimelineGroup<Quaternion> >> Timeline::timelineQ;

FadeInOutSwitch::FadeInOutSwitch( float _fullOpacityValue, float _fadeTime ) : fullOpacityValue(_fullOpacityValue), fadeTime(_fadeTime) {
    floatAnim = std::make_shared<AnimType<float >>(fullOpacityValue, UUIDGen::make());
}

float FadeInOutSwitch::value() const {
    return floatAnim->value;
}

floata& FadeInOutSwitch::anim() {
    return floatAnim;
}

void FadeInOutSwitch::setValue( float _value ) {
    floatAnim->value = _value;
}

void FadeInOutSwitch::fade( FadeInternalPhase phase ) {
    auto hasStopped = Timeline::stop(anim(), phase == FadeInternalPhase::In ? outPhaseId : inPhaseId,
                                     value());
    if ( !anim()->isAnimating || hasStopped ) {
        if ( phase == FadeInternalPhase::In ) {
            inPhaseId = Timeline::play(anim(), 0,
                                       KeyFramePair{ fadeTime, fullOpacityValue, AnimVelocityType::Cosine });
        } else {
            outPhaseId = Timeline::play(anim(), 0,
                                        KeyFramePair{ fadeTime, 0.0f, AnimVelocityType::Cosine });
        }
    }
}

void FadeInOutSwitch::fadeIn() {
    fade(FadeInternalPhase::In);
}

void FadeInOutSwitch::fadeOut() {
    fade(FadeInternalPhase::Out);
}

float FadeInOutSwitch::getFullOpacityValue() const {
    return fullOpacityValue;
}

void FadeInOutSwitch::setFullOpacityValue( float _fullOpacityValue ) {
    FadeInOutSwitch::fullOpacityValue = _fullOpacityValue;
}

float FadeInOutSwitch::getFadeTime() const {
    return fadeTime;
}

void FadeInOutSwitch::setFadeTime( float _fadeTime ) {
    FadeInOutSwitch::fadeTime = _fadeTime;
}
