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

class SerializeBin : public std::enable_shared_from_this<SerializeBin> {
public:
	SerializeBin( uint64_t vf ) {
        write( vf );
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
	}

	void write( const char* str ) {
		int32_t nameLength = 0;
		if ( str == nullptr ) {
			write( nameLength );
		} else {
			nameLength = static_cast<int32_t>( strlen( str ) );
			write( nameLength );
			for ( int32_t q = 0; q < nameLength; q++ ) {
				f.emplace_back( str[q] );
			}
		}
	}

	void write( const std::string& str ) {
		write( str.c_str() );
	}

	std::vector<unsigned char> buffer() const {
		return f;
	}

private:
	std::vector<unsigned char> f;
};

class DeserializeBin : public std::enable_shared_from_this<DeserializeBin> {
public:
	DeserializeBin( const std::vector<char>& _data, uint64_t vf ) {
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

	void readVersion( uint64_t vf ) {
        uint64_t savedVersion = 0;
        read( savedVersion );
        if ( savedVersion < vf ) {
            LOGE( "Old assets loaded version %d, expecting %d", savedVersion, vf );
        }
	}

private:
	std::shared_ptr<std::istringstream> fi;
};
