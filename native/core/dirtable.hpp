//
// Created by Dado on 2019-09-01.
//

#pragma once

#include <string>
#include <unordered_map>

class Dirtable {
    uint64_t mIsDirty = 0;
    uint64_t mFrameDelays = 1;
    std::unordered_map<std::string, uint64_t> mSubDirts;
public:
    void setDirtDelay( uint64_t _delay ) {
        mFrameDelays = _delay;
    }

    void setDirty( bool _flag ) {
        if ( _flag ) {
            mIsDirty = mFrameDelays;
        } else {
            if ( mIsDirty > 0 ) mIsDirty--;
        }
    }

    void setDirtyCumulative( bool _flag ) {
        if ( _flag ) mIsDirty = mFrameDelays;
    }

    [[nodiscard]] bool isDirty() const {
        return mIsDirty > 0;
    }

    void setDirty( const std::string& _key, bool _flag ) {
        if ( _flag ) {
            mSubDirts[_key] = mFrameDelays;
        } else {
            if ( mSubDirts[_key] > 0 ) mSubDirts[_key] = mSubDirts[_key]-1;
        }
    }

    void setDirtyCumulative( const std::string& _key, bool _flag ) {
        if ( _flag ) {
            mSubDirts[_key] = mFrameDelays;
        }
    }

    [[nodiscard]] bool isDirty(const std::string& _key) const {
        if ( auto it = mSubDirts.find(_key); it != mSubDirts.end() ) {
            return it->second > 0;
        }
        return false;
    }

};