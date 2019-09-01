//
// Created by Dado on 2019-09-01.
//

#pragma once

#include <string>
#include <unordered_map>

class Dirtable {
    bool mIsDirty = false;
    std::unordered_map<std::string, bool> mSubDirts;
public:
    void setDirty( bool _flag ) {
        mIsDirty = _flag;
    }

    void setDirtyCumulative( bool _flag ) {
        mIsDirty |= _flag;
    }

    [[nodiscard]] bool isDirty() const {
        return mIsDirty;
    }

    void setDirty( const std::string& _key, bool _flag ) {
        mSubDirts[_key] = _flag;
    }

    void setDirtyCumulative( const std::string& _key, bool _flag ) {
        mSubDirts[_key] |= _flag;
    }

    [[nodiscard]] bool isDirty(const std::string& _key) const {
        if ( auto it = mSubDirts.find(_key); it != mSubDirts.end() ) {
            return it->second;
        }
        return false;
    }

};