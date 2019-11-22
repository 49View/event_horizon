//
// Created by Dado on 2019-03-18.
//

#pragma once

template <typename H>
class CryptoUtils {
public:
    static std::string hash( const unsigned char* _data, size_t _length ) {
        H hashStrategy = H(_data, _length);
        return hashStrategy.hexdigest();
    }
    static std::string hash( const char* _data, size_t _length ) {
        H hashStrategy = H( reinterpret_cast<const unsigned  char*>(_data), _length);
        return hashStrategy.hexdigest();
    }
};



