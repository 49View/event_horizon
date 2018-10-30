//
//  serializebin.hpp
//  sixthview
//
//  Created by Dado on 01/10/2015.
//
//

#pragma once

#include <map>
#include <vector>
#include <fstream>
#include <string>
#include <core/zlib_util.h>
#include "file_manager.h"

enum class SerializeVersionFormat {
	Float,
	UInt64
};

static const uint64_t SBinVersion = 2030;

class SerializeBin : public std::enable_shared_from_this<SerializeBin> {
public:
	SerializeBin( SerializeVersionFormat vf, const std::string& _entityType  ) {
		entityType = _entityType;
		open( vf );
	}

	template<typename T>
	void write( const std::vector<T>& v ) {
		int32_t nameLength = static_cast<int32_t>( v.size() );
		write( nameLength );
		for ( const auto& d : v ) write( d );
	}

	template<typename T>
	void write( std::unique_ptr<T[]>& v, int32_t numIndices ) {
		write( numIndices );
		for ( int t = 0; t < numIndices; t++ ) write( v[t] );
	}

	template<typename T, typename M>
	void write( const std::map<T, M>& v ) {
		int32_t nameLength = static_cast<int32_t>( v.size() );
		write( nameLength );
		for ( auto& d : v ) {
			write( d.first );
			write( d.second );
		}
	}

	template<typename T, typename M>
	void write( const std::map<T, std::vector<M>>& v ) {
		int32_t nameLength = static_cast<int32_t>( v.size() );
		write( nameLength );
		for ( auto& d : v ) {
			write( d.first );
			write( static_cast<int32_t>( d.second.size() ) );
			for ( auto& vv : d.second ) {
				write( vv );
			}
		}
	}

	template<typename T>
	void write( const T& v ) {
		size_t size = sizeof( T );
		auto b = std::make_unique<unsigned char[]>(size);
		memcpy( b.get(), reinterpret_cast<const void*>( &v ), size );
		for ( size_t q = 0; q < size; q++ ) {
			f.emplace_back( b[q] );
		}
//		f.write( reinterpret_cast<const char*>( &v ), sizeof( T ) );
	}

//	template<typename T>
//	void write( T* v ) {
////		f.write( reinterpret_cast<const char*>( v ), sizeof( intptr_t ) );
//	}

	void write( const char* str ) {
		int32_t nameLength = 0;
		if ( str == nullptr ) {
			write( nameLength );
		} else {
			nameLength = static_cast<int32_t>( strlen( str ) );
			write( nameLength );
			for ( size_t q = 0; q < nameLength; q++ ) {
				f.emplace_back( str[q] );
			}
//			f.write( str, nameLength );
		}
	}

	void write( const std::string& str ) {
		write( str.c_str() );
	}

	void open( SerializeVersionFormat vf ) {
//		f.open( ( cacheFolder() + filenameHashed ).c_str(), std::ios_base::binary );
		if ( vf == SerializeVersionFormat::Float ) {
			write( 1.0f );
		} else {
			write( SBinVersion );
		}
	}

	std::vector<unsigned char> buffer() const {
		return f;
	}

private:
	std::vector<unsigned char> f;
	std::string entityType;
};

class DeserializeBin : public std::enable_shared_from_this<DeserializeBin> {
public:
	explicit DeserializeBin( const std::vector<char>& _data,
							 SerializeVersionFormat vf = SerializeVersionFormat::UInt64 ) {
		fi = std::make_shared<std::istringstream>( std::string{ _data.begin(), _data.end() } );
		readVersion( vf );
	}

    template<typename T>
	void read( std::vector<T>& v ) {

		int32_t vectorSize = 0;
		fi->read( reinterpret_cast<char*>( &vectorSize ), sizeof( int32_t ) );
		v.clear();
		T value;
		for ( int t = 0; t < vectorSize; t++ ) {
			fi->read( reinterpret_cast<char*>( &value ), sizeof( T ) );
			v.push_back( value );
		}
	}

	template<typename T>
	void read( std::vector<std::shared_ptr<T>>& v ) {

		int32_t vectorSize = 0;
		v.clear();
		fi->read( reinterpret_cast<char*>( &vectorSize ), sizeof( int32_t ) );
		for ( int t = 0; t < vectorSize; t++ ) {
			std::shared_ptr<T> value = std::make_shared<T>();
			value->deserialize( shared_from_this() );
			v.push_back( value );
		}
	}

	template<typename T>
	void read( std::unique_ptr<T[]>& v, int32_t& numIndices ) {

		fi->read( reinterpret_cast<char*>( &numIndices ), sizeof( int32_t ) );
		v = std::unique_ptr<T[]>( new T[numIndices] );
		T value;
		for ( int t = 0; t < numIndices; t++ ) {
			fi->read( reinterpret_cast<char*>( &value ), sizeof( T ) );
			v[t] = value;
		}
	}

	template<typename T, typename M>
	void read( std::map<T, M>& v ) {

		int32_t vectorSize = 0;
		fi->read( reinterpret_cast<char*>( &vectorSize ), sizeof( int32_t ) );
		T valueT;
		M valueM;
		for ( int t = 0; t < vectorSize; t++ ) {
			read( valueT );
			read( valueM );
			v[valueT] = valueM;
		}
	}

	template<typename T, typename M>
	void read( std::map<T, std::vector<M>>& /*v*/ ) {

		int32_t vectorSize = 0;
		fi->read( reinterpret_cast<char*>( &vectorSize ), sizeof( int32_t ) );
		T valueT;
		M valueM;
		std::vector<M> vectorM;

		for ( int t = 0; t < vectorSize; t++ ) {
			read( valueT );
			int32_t v2size = 0;
			fi->read( reinterpret_cast<char*>( &v2size ), sizeof( int32_t ) );
			for ( auto vs = 0; vs < v2size; vs++ ) {
				read( valueM );
				vectorM.push_back( valueM );
			}
		}
	}

	template<typename T>
	void read( T& v ) {
		fi->read( reinterpret_cast<char*>( &v ), sizeof( T ) );
	}

	void read( char** str ) {

		int32_t nameLength;
		fi->read( reinterpret_cast<char*>( &nameLength ), sizeof( int32_t ) );
		*str = new char[nameLength + 1];
		fi->read( *str, nameLength );
		( *str )[nameLength] = '\0';
	}

	void read( std::string& str ) {

		int32_t nameLength;
		fi->read( reinterpret_cast<char*>( &nameLength ), sizeof( int32_t ) );
		char* cstr = new char[nameLength + 1];
		fi->read( cstr, nameLength );
		( cstr )[nameLength] = '\0';
		str = cstr;
		delete[] cstr;
	}

	template<typename T>
	void read( T* v ) {
		fi->read( reinterpret_cast<char*>( &v ), sizeof( intptr_t ) );
	}

	void readVersion( SerializeVersionFormat vf ) {
		if ( vf == SerializeVersionFormat::Float ) {
			float savedVersion;
			read( savedVersion );
		} else {
			uint64_t savedVersion;
			read( savedVersion );
			if ( savedVersion < SBinVersion ) {
				LOGE( "Old assets loaded version %d, expecting %d", savedVersion, SBinVersion );
			}
		}
	}

private:
	std::shared_ptr<std::istringstream> fi;
};
