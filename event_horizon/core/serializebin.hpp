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

struct OneShotReadBuf : public std::streambuf {
	OneShotReadBuf( char* s, std::size_t n ) {
		setg( s, s, s + n );
	}
};

class SerializeBin : public std::enable_shared_from_this<SerializeBin> {
public:
	SerializeBin( const std::string& filename, SerializeVersionFormat vf, const std::string& _entityType  ) {
		entityType = _entityType;
		open( filename, vf );
	}

	template<typename T>
	void write( const std::vector<T>& v ) {
		int32_t nameLength = static_cast<int32_t>( v.size() );
		f.write( reinterpret_cast<const char*>( &nameLength ), sizeof( int32_t ) );
		for ( auto& d : v ) f.write( reinterpret_cast<const char*>( &d ), sizeof( T ) );
	}

	template<typename T>
	void write( const std::vector<std::shared_ptr<T>>& v ) {
		int32_t vsize = static_cast<int32_t>( v.size() );
		f.write( reinterpret_cast<const char*>( &vsize ), sizeof( int32_t ) );
		for ( auto& d : v ) d->serialize( shared_from_this() );
	}

	template<typename T>
	void write( std::unique_ptr<T[]>& v, int32_t numIndices ) {
		f.write( reinterpret_cast<const char*>( &numIndices ), sizeof( int32_t ) );
		for ( int t = 0; t < numIndices; t++ ) f.write( reinterpret_cast<const char*>( &v[t] ), sizeof( T ) );
	}

	template<typename T, typename M>
	void write( const std::map<T, M>& v ) {
		int32_t nameLength = static_cast<int32_t>( v.size() );
		f.write( reinterpret_cast<const char*>( &nameLength ), sizeof( int32_t ) );
		for ( auto& d : v ) {
			write( d.first );
			write( d.second );
		}
	}

	template<typename T, typename M>
	void write( const std::map<T, std::vector<M>>& v ) {
		int32_t nameLength = static_cast<int32_t>( v.size() );
		f.write( reinterpret_cast<const char*>( &nameLength ), sizeof( int32_t ) );
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
		f.write( reinterpret_cast<const char*>( &v ), sizeof( T ) );
	}

	template<typename T>
	void write( T* v ) {
		f.write( reinterpret_cast<const char*>( v ), sizeof( intptr_t ) );
	}

	void write( const char* str ) {
		int32_t nameLength = 0;
		if ( str == nullptr ) {
			f.write( reinterpret_cast<const char*>( &nameLength ), sizeof( int32_t ) );
		} else {
			nameLength = static_cast<int32_t>( strlen( str ) );
			f.write( reinterpret_cast<const char*>( &nameLength ), sizeof( int32_t ) );
			f.write( str, nameLength );
		}
	}

	void write( const std::string& str ) {
		write( str.c_str() );
	}

	void open( const std::string& filename, SerializeVersionFormat vf ) {
		mFilename = filename;
		std::size_t fhash = std::hash<std::string>{}( filename );
		filenameHashed = std::to_string( fhash );
		f.open( ( cacheFolder() + filenameHashed ).c_str(), std::ios_base::binary );
		if ( vf == SerializeVersionFormat::Float ) {
			write( 1.0f );
		} else {
			write( SBinVersion );
		}
	}

	void close() {
		f.close();
		auto fn = zlibUtil::deflateMemory( FM::readLocalFile(cacheFolder() + filenameHashed) );
		Http::post( Url( Http::restEntityPrefix( entityType, mFilename ) ), fn );
//		Http::post( )
//		FM::copyLocalToRemote( cacheFolder() + filenameHashed, mFilename, HttpUrlEncode::No );
	}

private:
	std::ofstream f;
	std::string mFilename;
	std::string filenameHashed;
	std::string entityType;
};

class DeserializeBin : public std::enable_shared_from_this<DeserializeBin> {
public:
	DeserializeBin( std::shared_ptr<uint8_p> _data, SerializeVersionFormat vf = SerializeVersionFormat::UInt64 ) {
		osrb = std::make_shared<OneShotReadBuf>( reinterpret_cast<char *>(_data->first.get()), _data->second );
		fi = std::make_shared<std::istream>( osrb.get() );
		readVersion( vf );
	}

	size_t tellg() const {
	    return static_cast<size_t >(fi->tellg());
	}

    void seekg( const size_t offset )  {
        fi->seekg( offset );
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
	std::shared_ptr<std::istream> fi;
	std::shared_ptr<OneShotReadBuf> osrb;
};
