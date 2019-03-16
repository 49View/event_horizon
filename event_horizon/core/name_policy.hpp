//
// Created by Dado on 2019-01-29.
//

#pragma once

#include <string>

template <typename T = std::string>
class NamePolicy {
public:
    NamePolicy() = default;
    explicit NamePolicy( T name ) : mName( std::move(name) ) {}

    virtual const T& Name() const {
        return mName;
    }

    virtual T& NameRef() {
        return mName;
    }

    virtual T NameCopy() const {
        return mName;
    }

    virtual void Name( const T& _name ) {
        mName = _name;
    }

private:
    T mName;
};



