#include "anim.h"

#include <core/game_time.h>

std::unordered_map<std::string, std::shared_ptr<TimelineGroup<int>        >> Timeline::timelinei;
std::unordered_map<std::string, std::shared_ptr<TimelineGroup<float>      >> Timeline::timelinef;
std::unordered_map<std::string, std::shared_ptr<TimelineGroup<Vector2f>   >> Timeline::timelineV2;
std::unordered_map<std::string, std::shared_ptr<TimelineGroup<Vector3f>   >> Timeline::timelineV3;
std::unordered_map<std::string, std::shared_ptr<TimelineGroup<Vector4f>   >> Timeline::timelineV4;
std::unordered_map<std::string, std::shared_ptr<TimelineGroup<Quaternion> >> Timeline::timelineQ;

//void TimelineMapSpec::addDelay( TimelineIndex _k, float _delay ) {
//    auto ki = _k / tiNorm;
//    switch (ki) {
//        case tiIntIndex:
//            tmapi[_k].addDelay( _delay );
//            break;
//        case tiFloatIndex:
//            tmapf[_k].addDelay( _delay );
//            break;
//        case tiV2fIndex:
//            tmapV2[_k].addDelay( _delay );
//            break;
//        case tiV3fIndex:
//            tmapV3[_k].addDelay( _delay );
//            break;
//        case tiV4fIndex:
//            tmapV4[_k].addDelay( _delay );
//            break;
//        case tiQuatIndex:
//            tmapQ[_k].addDelay( _delay );
//        default:
//            break;
//    }
//}
//
//bool TimelineMapSpec::update( TimelineIndex _k, float _timeElapsed ) {
//    auto ki = _k / tiNorm;
//    switch (ki) {
//        case tiIntIndex:
//            return tmapi[_k].update( _timeElapsed );
//            break;
//        case tiFloatIndex:
//            return tmapf[_k].update( _timeElapsed );
//            break;
//        case tiV2fIndex:
//            return tmapV2[_k].update( _timeElapsed );
//            break;
//        case tiV3fIndex:
//            return tmapV3[_k].update( _timeElapsed );
//            break;
//        case tiV4fIndex:
//            return tmapV4[_k].update( _timeElapsed );
//            break;
//        case tiQuatIndex:
//            return tmapQ[_k].update( _timeElapsed );
//            break;
//        default:
//            break;
//    }
//    return false;
//}
//
//bool TimelineMapSpec::deleteKey( TimelineIndex _k, uint64_t _index ) {
//    auto ki = _k / tiNorm;
//    switch (ki) {
//        case tiIntIndex:
//            return tmapi[_k].deleteKey(_index);
//        case tiFloatIndex:
//            return tmapf[_k].deleteKey(_index);
//        case tiV2fIndex:
//            return tmapV2[_k].deleteKey(_index);
//        case tiV3fIndex:
//            return tmapV3[_k].deleteKey(_index);
//        case tiV4fIndex:
//            return tmapV4[_k].deleteKey(_index);
//        case tiQuatIndex:
//            return tmapQ[_k].deleteKey(_index);
//        default:
//            break;
//    }
//    return false;
//}
//
//void TimelineMapSpec::updateKeyTime( TimelineIndex _k, uint64_t _index, float _time ) {
//    auto ki = _k / tiNorm;
//    switch (ki) {
//        case tiIntIndex:
//            tmapi[_k].updateKeyTime( _index, _time);
//            break;
//        case tiFloatIndex:
//            tmapf[_k].updateKeyTime( _index, _time);
//            break;
//        case tiV2fIndex:
//            tmapV2[_k].updateKeyTime( _index, _time);
//            break;
//        case tiV3fIndex:
//            tmapV3[_k].updateKeyTime( _index, _time);
//            break;
//        case tiV4fIndex:
//            tmapV4[_k].updateKeyTime( _index, _time);
//            break;
//        case tiQuatIndex:
//            tmapQ[_k].updateKeyTime( _index, _time);
//            break;
//        default:
//            break;
//    }
//}
//
//bool TimelineMapSpec::isActive( TimelineIndex _k ) const {
//    auto ki = _k / tiNorm;
//    switch (ki) {
//        case tiIntIndex:
//            return tmapi.at(_k).isActive();
//        case tiFloatIndex:
//            return tmapf.at(_k).isActive();
//        case tiV2fIndex:
//            return tmapV2.at(_k).isActive();
//        case tiV3fIndex:
//            return tmapV3.at(_k).isActive();
//        case tiV4fIndex:
//            return tmapV4.at(_k).isActive();
//        case tiQuatIndex:
//            return tmapQ.at(_k).isActive();
//        default:
//            break;
//    }
//    return false;
//}
//
//void TimelineMapSpec::visit( TimelineIndex _k, AnimVisitCallback _callback ) {
//    auto ki = _k / tiNorm;
//    switch (ki) {
//        case tiIntIndex:
//            tmapi.at(_k).visit(_callback, _k, tiIntIndex);
//            break;
//        case tiFloatIndex:
//            tmapf.at(_k).visit(_callback, _k, tiFloatIndex);
//            break;
//        case tiV2fIndex:
//            tmapV2.at(_k).visit(_callback, _k, tiV2fIndex);
//            break;
//        case tiV3fIndex:
//            tmapV3.at(_k).visit(_callback, _k, tiV3fIndex);
//            break;
//        case tiV4fIndex:
//            tmapV4.at(_k).visit(_callback, _k, tiV4fIndex);
//            break;
//        case tiQuatIndex:
//            tmapQ.at(_k).visit(_callback, _k, tiQuatIndex);
//            break;
//        default:
//            break;
//    }
//}
