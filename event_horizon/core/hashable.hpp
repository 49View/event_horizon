//
// Created by Dado on 2019-02-21.
//

#pragma once

#include <string>
#include <core/hashing/crypto_utils.hpp>

// Defaults to MD5, which is more than enough, for now!
#include <core/hashing/md5.hpp>

template <typename H = MD5>
class Hashable {
public:
    const std::string& Hash() const {
        return mHash;
    }

    std::string& HashRef() {
        return mHash;
    }

    std::string HashCopy() const {
        return mHash;
    }

    void Hash( const std::string& _name ) {
        mHash = _name;
    }

    std::string hashFn( const size_t _number ) const {
        return hashFn( std::to_string(_number) );
    }

    std::string hashFn( const std::string& _str ) const {
        return CryptoUtils<H>::hash( _str.data(), _str.size() );
    }

protected:
    virtual void calcHash( const std::string& _str ) {
        mHash = CryptoUtils<H>::hash( _str.data(), _str.size() );
    }
    virtual void calcHash( const unsigned char* _data, size_t _length ) {
        mHash = CryptoUtils<H>::hash( _data, _length );
    }
    virtual void calcHash( const ucchar_p& _data ) {
        mHash = CryptoUtils<H>::hash( _data.first, _data.second );
    }
    virtual void calcHash( const uint8_p& _data ) {
        mHash = CryptoUtils<H>::hash( _data.first.get(), _data.second );
    }
    virtual void calcHash( const SerializableContainer & _data ) {
        mHash = CryptoUtils<H>::hash( _data.data(), _data.size() );
    }
private:
    std::string mHash;

public:
    static std::string hashOf(const SerializableContainer & _data) {
        return CryptoUtils<H>::hash( _data.data(), _data.size() );
    }
    static std::string hashOf( const uint8_p& _data ) {
        return CryptoUtils<H>::hash( _data.first.get(), _data.second );
    }

};


// ### Remove and templatize!!
class HashableDefaults {
public:
    const static uint64_t HASH_LENGTH = 32;
};
#define NULL_HASH "00000000000000000000000000000000"
