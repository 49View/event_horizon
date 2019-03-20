//
//  serializebin.hpp
//  sixthview
//
//  Created by Dado on 01/10/2015.
//
//

#pragma once

#include <map>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <string>
#include <core/serialize_types.hpp>
#include <core/hashable.hpp>
#include <poly/resources/entity_factory.hpp>

class SerializeBin : public std::enable_shared_from_this<SerializeBin> {
public:
    explicit SerializeBin( const SerializeHeader& _header ) {
        writeHeader( _header );
	}

	void writeHeader( const SerializeHeader& _header ) {
		write( _header.hash );
        write( _header.version );
        write( _header.entityGroup );
        headerSizeSeek = f.size();
        write( _header.size );
    }

	template<typename T>
	void write( const std::vector<T>& v ) {
        auto nameLength = static_cast<int32_t>( v.size() );
		write( nameLength );
		for ( const auto& d : v ) write( d );
	}

	template<typename T>
	void write( std::unique_ptr<T[]>& v, int32_t numIndices ) {
		write( numIndices );
		for ( int t = 0; t < numIndices; t++ ) write( v[t] );
	}

#define writeMapInternal \
	int32_t nameLength = static_cast<int32_t>( v.size() ); \
	write( nameLength ); \
	for ( auto& d : v ) { \
		write( d.first ); \
		write( d.second ); \
	}

	template<typename T, typename M>
	void write( const std::map<T, M>& v ) {
		writeMapInternal
	}

	template<typename T, typename M>
	void write( const std::unordered_map<T, M>& v ) {
		writeMapInternal
	}

	template<typename T>
	void write( const std::unordered_map<T, uint8_p>& v ) {
		int32_t nameLength = static_cast<int32_t>( v.size() );
		write( nameLength );
		for ( auto&& [name, uptr] : v ) {
			write( name );
			write( uptr.second );
			if ( uptr.second > 0 ) {
				for ( uint64_t q = 0; q < uptr.second; q++ ) {
					unsigned char byte = uptr.first[q];
					f.emplace_back( byte );
				}
			}
		}
	}

#define writeMapVectorInternal \
	int32_t nameLength = static_cast<int32_t>( v.size() ); \
	write( nameLength ); \
	for ( auto& d : v ) { \
		write( d.first ); \
		write( static_cast<int32_t>( d.second.size() ) ); \
		for ( auto& vv : d.second ) { \
			write( vv ); \
		} \
	}

	template<typename T, typename M>
	void write( const std::map<T, std::vector<M>>& v ) {
		writeMapVectorInternal
	}

	template<typename T, typename M>
	void write( const std::unordered_map<T, std::vector<M>>& v ) {
		writeMapVectorInternal
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

	template<typename T>
	void writeAt( size_t _pos, const T& v ) {
		size_t size = sizeof( T );
		auto b = std::make_unique<unsigned char[]>(size);
		memcpy( b.get(), reinterpret_cast<const void*>( &v ), size );
		for ( size_t q = 0; q < size; q++ ) {
			ASSERT( _pos + q < f.size() );
			f[_pos+q] = b[q];
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

	// A shared_ptr means a dependencym so it has to be hashable
	template<typename T>
	void write( std::shared_ptr<T> v ) {
		 v ? write( v->Hash() ) : write(NULL_HASH);
	}

	template<typename T>
	void writeDep( std::shared_ptr<T> v ) {
		if ( !v ) {
			static SerializableContainer nullDep{'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};
			deps.emplace( NULL_HASH, nullDep );
		} else {
//			v->calcHash();
			if ( deps.find( v->Hash() ) == deps.end() ) {
				auto writer = std::make_shared<SerializeBin>(
						SerializeHeader{v->Hash(), T::Version(), T::Prefix() } );
				v->serialize(writer);
				deps.emplace( v->Hash(), writer->buffer() );
			}
		}
	}

	void finalizeDeps() {
		write( uint64_t(deps.size()) );
		for ( const auto& [k,v] : deps ) {
			write( v );
		}
	}

	SerializableContainer buffer() {
		writeHeaderSizeInjected();
		return f;
	}

private:
	void writeHeaderSizeInjected() {
		writeAt( headerSizeSeek, f.size() - headerSizeSeek );
	}

private:
	SerializableContainer f;
	SerializationDependencyMap deps;
	size_t headerSizeSeek = 0;
};

class DeserializeBin : public std::enable_shared_from_this<DeserializeBin> {
public:
	DeserializeBin( const SerializableContainer& _data, uint64_t vf ) {
		fi = std::make_shared<std::istringstream>( std::string{ _data.begin(), _data.end() } );
		readAndCheckHeader( vf );
	}

	DeserializeBin( uint8_p&& _data, uint64_t vf ) {
		fi = std::make_shared<std::istringstream>( std::string{ reinterpret_cast<char*>(_data.first.get()),
																static_cast<size_t>(_data.second) } );
		readAndCheckHeader( vf );
	}

	void readAndCheckHeader( uint64_t vf ) {
		readHeader( header );
		if ( header.version < vf ) {
			LOGE( "Old assets loaded version %d, expecting %d", header.version, vf );
		}
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
		v = std::unique_ptr<T[]>(numIndices);
		T value;
		for ( int t = 0; t < numIndices; t++ ) {
			fi->read( reinterpret_cast<char*>( &value ), sizeof( T ) );
			v[t] = value;
		}
	}

#define readMapInternal \
	int32_t vectorSize = 0; \
	fi->read( reinterpret_cast<char*>( &vectorSize ), sizeof( int32_t ) ); \
	T valueT; \
	M valueM; \
	for ( int t = 0; t < vectorSize; t++ ) { \
		read( valueT ); \
		read( valueM ); \
		v[valueT] = valueM; \
	}

	template<typename T, typename M>
	void read( std::map<T, M>& v ) {
		readMapInternal
	}

	template<typename T, typename M>
	void read( std::unordered_map<T, M>& v ) {
		readMapInternal
	}

	template<typename T>
	void read( std::unordered_map<T, uint8_p>& v ) {
		int32_t vectorSize = 0;
		fi->read( reinterpret_cast<char*>( &vectorSize ), sizeof( int32_t ) );
		T valueT;
		for ( int t = 0; t < vectorSize; t++ ) {
			read( valueT );

			uint8_p uptr;
			fi->read( reinterpret_cast<char*>( &uptr.second ), sizeof( uint64_t ) );
			uptr.first = std::make_unique<uint8_t[]>(uptr.second);
			fi->read( reinterpret_cast<char*>(uptr.first.get()), uptr.second );

			v[valueT] = std::move(uptr);
		}
	}

#define readMapVectorInternal \
	int32_t vectorSize = 0; \
	fi->read( reinterpret_cast<char*>( &vectorSize ), sizeof( int32_t ) ); \
	T valueT; \
	M valueM; \
	std::vector<M> vectorM; \
	for ( int t = 0; t < vectorSize; t++ ) { \
		read( valueT ); \
		int32_t v2size = 0; \
		fi->read( reinterpret_cast<char*>( &v2size ), sizeof( int32_t ) ); \
		for ( auto vs = 0; vs < v2size; vs++ ) { \
			read( valueM ); \
			vectorM.push_back( valueM ); \
		} \
		v[valueT] = vectorM; \
	}

	template<typename T, typename M>
	void read( std::map<T, std::vector<M>>& v ) {
		readMapVectorInternal
	}
	template<typename T, typename M>
	void read( std::unordered_map<T, std::vector<M>>& v ) {
		readMapVectorInternal
	}

	template<typename T>
	void read( T& v ) {
		fi->read( reinterpret_cast<char*>( &v ), sizeof( T ) );
	}

	void read( std::string& str ) {
		int32_t nameLength;
		fi->read( reinterpret_cast<char*>( &nameLength ), sizeof( int32_t ) );
		auto cstr = std::make_unique<char[]>( static_cast<size_t>(nameLength + 1));
		fi->read( cstr.get(), nameLength );
		cstr[nameLength] = '\0';
		str = std::string{ cstr.get() };
	}

	template<typename T>
	void read( std::shared_ptr<T>& v ) {
		SerializeDependencyHash dh;
		read(dh);
		if ( dh != NULL_HASH ) {
//		    ### Reenable this factory
//			v = EF::create<T>(dep(dh));
		}
	}

	void readHeader( SerializeHeader& _header ) {
		read( _header.hash);
        read( _header.version );
        read( _header.entityGroup);
        read( _header.size);
    }

	void readDependency() {
		SerializableContainer bc;
		read( bc );
		static constexpr uint64_t vectorHeader = 4;
		std::string hash{ bc.begin()+vectorHeader, bc.begin()+vectorHeader+HashableDefaults::HASH_LENGTH };
		deps.emplace( hash, bc );
	}

	SerializableContainer dep( const SerializeDependencyHash& _hash ) {
		ASSERT( deps.find(_hash) != deps.end() );
		return deps[_hash];
	}

private:
	std::shared_ptr<std::istringstream> fi;
    SerializeHeader header;
	SerializationDependencyMap deps;
};
