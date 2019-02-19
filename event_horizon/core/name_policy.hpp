//
// Created by Dado on 2019-01-29.
//

#pragma once

#include <string>

template <typename T = std::string>
class NamePolicy {
public:
    inline const T& Name() const {
        return mName;
    }

    inline T& NameRef() {
        return mName;
    }

    inline T Name() {
        return mName;
    }

    inline void Name( const T& _name ){
        mName = _name;
    }

private:
    T mName;
};



