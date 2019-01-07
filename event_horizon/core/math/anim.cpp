#include "anim.h"

#include <core/game_time.h>

uint64_t TimelineMapSpec::mkf = 1;

TimelineMapSpec             Timeline::timelines;
Timeline::TimelineGroupMap  Timeline::timelineGroups;
TimelineLinks               Timeline::links;

bool TimelineMapSpec::update( TimelineIndex _k, float _timeElapsed ) {
    auto ki = _k / tiNorm;
    switch (ki) {
        case tiIntIndex:
            return tmapi[_k].update( _timeElapsed );
            break;
        case tiFloatIndex:
            return tmapf[_k].update( _timeElapsed );
            break;
        case tiV2fIndex:
            return tmapV2[_k].update( _timeElapsed );
            break;
        case tiV3fIndex:
            return tmapV3[_k].update( _timeElapsed );
            break;
        case tiV4fIndex:
            return tmapV4[_k].update( _timeElapsed );
            break;
        case tiQuatIndex:
            return tmapQ[_k].update( _timeElapsed );
            break;
        default:
            break;
    }
    return false;
}

//void TimelineMapSpec::reset( TimelineIndex _k, float _startTime ) {
//    auto ki = _k / tiNorm;
//    switch (ki) {
//        case tiIntIndex:
//            tmapi[_k].reset(_startTime);
//            break;
//        case tiFloatIndex:
//            tmapf[_k].reset(_startTime);
//            break;
//        case tiV2fIndex:
//            tmapV2[_k].reset(_startTime);
//            break;
//        case tiV3fIndex:
//            tmapV3[_k].reset(_startTime);
//            break;
//        case tiV4fIndex:
//            tmapV4[_k].reset(_startTime);
//            break;
//        case tiQuatIndex:
//            tmapQ[_k].reset(_startTime);
//            break;
//        default:
//            break;
//    }
//}

bool TimelineMapSpec::isActive( TimelineIndex _k ) const {
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

void TimelineMapSpec::visit( TimelineIndex _k, AnimVisitCallback _callback ) {
    auto ki = _k / tiNorm;
    switch (ki) {
        case tiIntIndex:
            tmapi.at(_k).visit(_callback, tiIntIndex);
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
