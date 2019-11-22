//
// Created by Dado on 2019-04-09.
//

#pragma once

#include <core/serializebin.hpp>

template<typename TVisitor, typename ...T> inline void serializeBinVariadic( TVisitor& visitor, T &...v ) {
    ((void)visitor.write(std::forward<T>(v)), ...);
}

template<typename TVisitor, typename ...T> inline void deserializeBinVariadic( TVisitor& visitor, T &...v ) {
    ((void)visitor.read(static_cast<T&>(v)), ...);
}

#define JSONSERIALBIN(...) \
	inline void serialize( SerializeBin& visitor ) const { serializeBinVariadic(visitor, __VA_ARGS__ ); } \
	inline SerializableContainer serialize() const { SerializeBin mw; serialize(mw); return mw.serialize();} \

#define JSONRESOURCECLASSSERIALBIN(CLASSNAME,...) \
    RESOURCE_CTORS(CLASSNAME); \
    JSONSERIALBIN(__VA_ARGS__) \
	explicit CLASSNAME( DeserializeBin& reader ) { deserialize( reader ); } \
	inline void deserialize( DeserializeBin& visitor ) { deserializeBinVariadic(visitor, __VA_ARGS__ ); } \
	void bufferDecode( const unsigned char *_buffer, size_t _length ) { \
        DeserializeBin reader( _buffer, _length ); \
        deserialize( reader ); \
    }

