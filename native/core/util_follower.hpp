//
// Created by dado on 06/06/2020.
//

#pragma once

#include <map>
#include <cassert>
#include <core/htypes_shared.hpp>

typedef std::map<uint64_t, FollowerGapData> gapMap;
typedef gapMap::iterator gapMapIt;

class FollowerGap {
public:
    FollowerGap() {
        mGaps.clear();
    }

    FollowerGap( const uint64_t size ) {
        for ( uint64_t t = 0; t < size; t++ ) mGaps.push_back(FollowerGapData(FollowerGapSide::NoGap));
    }

    void pushGap( const FollowerGapData& val ) {
        mGaps.push_back(val);
    }

    void createGap( uint64_t start_index, uint64_t end_index, float insetStart, float insetEnd = -1.0f ) {
        assert(start_index < mGaps.size());
        assert(end_index < mGaps.size());

        if ( insetEnd == -1.0f ) insetEnd = insetStart;
        mGaps[start_index] = FollowerGapData(FollowerGapSide::Start, insetStart);
        mGaps[end_index] = FollowerGapData(FollowerGapSide::End, insetEnd);
    }

    bool isGap( FollowerGapSide side, uint64_t index, float& inset ) const;
    bool isGapAt( uint64_t index ) const;
    bool isStartGapAt( uint64_t index ) const;
    bool isEndGapAt( uint64_t index ) const;
    bool isVisibleAt( uint64_t index ) const;
    void tagAllNotVisible();
public:
    static const FollowerGap Empty;
private:
    std::vector<FollowerGapData> mGaps;
};



