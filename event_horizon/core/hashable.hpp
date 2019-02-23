//
// Created by Dado on 2019-02-21.
//

#pragma once

#include <string>
#include <core/hashing/md5.hpp>

class Hashable {
public:
    virtual const std::string& Hash() const {
        return mHash;
    }

    virtual std::string& HashRef() {
        return mHash;
    }

    virtual std::string HashCopy() const {
        return mHash;
    }

    virtual void Hash( const std::string& _name ) {
        mHash = _name;
    }

protected:
    void calcHash() {
        mHash = calcHashImpl();
    }
    virtual std::string calcHashImpl() = 0;
private:
    std::string mHash;

public:
    const static uint64_t HASH_LENGTH = 32;
};