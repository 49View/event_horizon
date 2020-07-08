//
//  megawriter.hpp
//  sixthview
//
//  Created by Dado on 01/10/2015.
//
//

#pragma once

#include <array>
#include <set>
#include <unordered_set>
#include <unordered_map>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <core/htypes_shared.hpp>
#include <core/math/rect2f.h>
#include <core/math/aabb.h>
#include <core/math/vector4f.h>
#include <core/math/quaternion.h>
#include <core/boxable.hpp>

using namespace rapidjson;

class MegaWriter {
public:
	MegaWriter() {
		writer = std::make_unique<Writer<StringBuffer>>( internalOS );
	}

	explicit MegaWriter( StringBuffer& os ) {
		writer = std::make_unique<Writer<StringBuffer>>( os );
	}

	std::string getString() const {
		return std::string(internalOS.GetString());
	}
    SerializableContainer getSerializableContainer() const {
	    auto s = std::string(internalOS.GetString());
        return SerializableContainer{ s.data(), s.data() + s.size() };
    }

	void StartObject() {
		writer->StartObject();
	}
	void EndObject() {
		writer->EndObject();
	}
	void StartArray() {
		writer->StartArray();
	}
	void EndArray() {
		writer->EndArray();
	}
	void String( const std::string& _string) {
		writer->String( _string.c_str() );
	}

	// Serializing

	template<typename T>
	void serialize( const char* _name, const T& value ) {
		if ( _name != nullptr ) writer->String( _name );
		value.serialize( this );
	}

    template<typename T>
    void serialize( const char* _name, const std::shared_ptr<T>& value ) {
        if ( _name != nullptr ) writer->String( _name );
        value->serialize( this );
    }

    template<typename T, std::size_t N>
	void serialize( const char* _name, const std::array<T, N>& array ) {
		if ( _name != nullptr ) writer->String( _name );
		writer->StartArray();
		for ( auto& value : array ) {
			value.serialize( this );
		}
		writer->EndArray();
	}

	template<typename T>
	void serialize( const char* _name, const std::vector<T>& array ) {
		if ( _name != nullptr ) writer->String( _name );
		writer->StartArray();
		for ( auto& value : array ) {
			value.serialize( this );
		}
		writer->EndArray();
	}

    template<typename T, std::size_t N>
    void serialize( const char* _name, const std::vector<std::array<T, N>>& array ) {
        if ( _name != nullptr ) writer->String( _name );
        writer->StartArray();
        for ( auto& varr : array ) {
            writer->StartArray();
            for ( auto& value : varr ) {
                writer->StartArray();
                writer->Double( value[0] );
                writer->Double( value[1] );
                writer->Double( value[2] );
                writer->EndArray();
            }
            writer->EndArray();
        }
        writer->EndArray();
    }

    template<typename T>
    void serialize( const char* _name, const VectorWrap<T>& array ) {
        if ( _name != nullptr ) writer->String( _name );
        writer->StartObject();
        writer->String( "v" );
        writer->StartArray();
        for ( auto& value : array.v ) {
            for ( int t = 0; t < value.size(); t++ )
                serialize( nullptr, value[t] );
        }
        writer->EndArray();
        writer->String( "wrap" );
        writer->Bool( array.wrap );
        writer->EndObject();
    }

    void serialize( const char* _name, const std::set<std::string>& array ) {
		if ( _name != nullptr ) writer->String( _name );
		writer->StartArray();
		for ( auto& value : array ) {
			writer->String( value.c_str() );
		}
		writer->EndArray();
	}

    void serialize( const char* _name, const std::unordered_set<std::string>& array ) {
        if ( _name != nullptr ) writer->String( _name );
        writer->StartArray();
        for ( auto& value : array ) {
            writer->String( value.c_str() );
        }
        writer->EndArray();
    }

    template<typename T>
    void serialize( const char* _name, const std::unordered_map<std::string, T>& _map ) {
        if ( _name != nullptr ) writer->String( _name );
        writer->StartArray();
        for ( const auto& [k,v] : _map ) {
            serialize( nullptr, std::make_pair( k, v) );
        }
        writer->EndArray();
    }

    template<typename T>
	void serialize( const char* _name, const std::vector<std::shared_ptr<T>>& array ) {
		if ( _name != nullptr ) writer->String( _name );
		writer->StartArray();
		for ( auto& value : array ) {
			value->serialize( this );
		}
		writer->EndArray();
	}

	template<typename T>
	void serialize( const char* _name, const std::vector<std::vector<T>>& array ) {
		if ( _name != nullptr ) writer->String( _name );
		writer->StartArray();
		for ( auto& subarray : array ) {
			writer->StartArray();
			for ( auto& value : subarray ) {
				value.serialize( this );
			}
			writer->EndArray();
        }
		writer->EndArray();
	}

    template<typename T>
    void serialize( const char* _name, const VectorOfVectorWrap<T>& array ) {
        if ( _name != nullptr ) writer->String( _name );
        writer->StartArray();
        for ( auto& subarray : array ) {
            serialize( nullptr, subarray );
        }
        writer->EndArray();
    }

    void serialize( const char* name, const char* value ) {
        if ( name != nullptr ) writer->String( name );
		writer->String( value );
	}

	void serialize( const char* name, const double& value ) {
        if ( name != nullptr )writer->String( name );
		writer->Double( value );
	}

	void serialize( const char* name, const float& value ) {
        if ( name != nullptr )writer->String( name );
		writer->Double( value );
	}

	void serialize( const char* name, const bool& value ) {
        if ( name != nullptr )writer->String( name );
		writer->Bool( value );
	}

	void serialize( const char* name, const int32_t& value ) {
        if ( name != nullptr )writer->String( name );
		writer->Int( value );
	}

	void serialize( const char* name, const uint32_t& value ) {
        if ( name != nullptr )writer->String( name );
		writer->Uint( value );
	}

	void serialize( const char* name, const int64_t& value ) {
        if ( name != nullptr )writer->String( name );
		writer->Int64( value );
	}

	void serialize( const char* name, const uint64_t& value ) {
        if ( name != nullptr )writer->String( name );
		writer->Uint64( value );
	}

	void serialize( const char* name, const std::string& value ) {
		if ( name ) writer->String( name );
		writer->String( value.c_str() );
	}

	void serialize( const char* _name, const Vector2f& value ) {
		if ( _name != nullptr ) writer->String( _name );
		writer->StartArray();
		writer->Double( value[0] );
		writer->Double( value[1] );
		writer->EndArray();
	}

	void serialize( const char* _name, const Vector3f& value ) {
		if ( _name != nullptr ) writer->String( _name );
		writer->StartArray();
		writer->Double( value[0] );
		writer->Double( value[1] );
		writer->Double( value[2] );
		writer->EndArray();
	}

	void serialize( const char* _name, const Vector4f& value ) {
		if ( _name != nullptr ) writer->String( _name );
		writer->StartArray();
		writer->Double( value[0] );
		writer->Double( value[1] );
		writer->Double( value[2] );
		writer->Double( value[3] );
		writer->EndArray();
	}

    void serialize( const char* _name, const Quaternion& value ) {
        if ( _name != nullptr ) writer->String( _name );
        writer->StartArray();
        writer->Double( value[0] );
        writer->Double( value[1] );
        writer->Double( value[2] );
        writer->Double( value[3] );
        writer->EndArray();
    }

    void serialize( const char* name, const std::pair<int, int>& value ) {
        if ( name != nullptr )writer->String( name );
		writer->StartArray();
		writer->Int( static_cast<int32_t>( value.first ) );
		writer->Int( static_cast<int32_t>( value.second ) );
		writer->EndArray();
	}

	template <typename  T>
    void serialize( const char* name, const std::pair<std::string, T>& value ) {
        if ( name ) writer->String( name );
        writer->StartObject();
        writer->String( "key" );
        writer->String( value.first.c_str() );
        writer->String( "value" );
        serialize( nullptr, value.second );
        writer->EndObject();
    }

    void serialize( const char* name, const JMATH::Rect2f& rect ) {
        if ( name != nullptr ) writer->String( name );
		writer->StartArray();
		JMATH::Rect2f finalRect = rect;
		writer->Double( finalRect.left() );
		writer->Double( finalRect.top() );
		writer->Double( finalRect.right() );
		writer->Double( finalRect.bottom() );
		writer->EndArray();
	}

	void serialize( const char* name, const EmptyBox& rect ) {
	}

	void serialize( const char* name, const JMATH::AABB& rect ) {
        if ( name != nullptr ) writer->String( name );
		writer->StartArray();
		writer->Double( rect.minPoint().x() );
		writer->Double( rect.minPoint().y() );
		writer->Double( rect.minPoint().z() );
		writer->Double( rect.maxPoint().x() );
		writer->Double( rect.maxPoint().y() );
		writer->Double( rect.maxPoint().z() );
		writer->EndArray();
	}

	void serialize( const char* _name, const std::vector<Vector2f>& array ) {
		if ( _name != nullptr ) writer->String( _name );
		writer->StartArray();
		for ( auto& value : array ) {
			writer->StartArray();
			writer->Double( value[0] );
			writer->Double( value[1] );
			writer->EndArray();
		}
		writer->EndArray();
	}

	void serialize( const char* _name, const std::vector<Triangle2d>& array ) {
		if ( _name != nullptr ) writer->String( _name );
		writer->StartArray();
		for ( auto& value : array ) {
			writer->StartArray();
			writer->Double( std::get<0>( value ).x() );
			writer->Double( std::get<0>( value ).y() );
			writer->EndArray();
			writer->StartArray();
			writer->Double( std::get<1>( value ).x() );
			writer->Double( std::get<1>( value ).y() );
			writer->EndArray();
			writer->StartArray();
			writer->Double( std::get<2>( value ).x() );
			writer->Double( std::get<2>( value ).y() );
			writer->EndArray();
		}
		writer->EndArray();
	}

	template<std::size_t N>
	void serialize( const char* _name, const std::array<Vector2f, N>& array ) {
		if ( _name != nullptr ) writer->String( _name );
		writer->StartArray();
		for ( auto& value : array ) {
			writer->StartArray();
			writer->Double( value[0] );
			writer->Double( value[1] );
			writer->EndArray();
		}
		writer->EndArray();
	}

	void serialize( const char* _name, const std::vector<Vector3f>& array ) {
		if ( _name != nullptr ) writer->String( _name );
		writer->StartArray();
		for ( auto& value : array ) {
			writer->StartArray();
			writer->Double( value[0] );
			writer->Double( value[1] );
			writer->Double( value[2] );
			writer->EndArray();
		}
		writer->EndArray();
	}

	void serialize( const char* _name, const std::vector<Vector4f>& array ) {
		if ( _name != nullptr ) writer->String( _name );
		writer->StartArray();
		for ( auto& value : array ) {
			writer->StartArray();
			writer->Double( value[0] );
			writer->Double( value[1] );
			writer->Double( value[2] );
			writer->Double( value[3] );
			writer->EndArray();
		}
		writer->EndArray();
	}

    void serialize( const char* _name, const Matrix3f& mat ) {
        if ( _name != nullptr ) writer->String( _name );
        writer->StartArray();
        serialize( "Row0", mat.getRow(0) );
        serialize( "Row1", mat.getRow(1) );
        serialize( "Row2", mat.getRow(2) );
        writer->EndArray();
    }

    void serialize( const char* _name, const Matrix4f& mat ) {
        if ( _name != nullptr ) writer->String( _name );
        writer->StartArray();
        for ( int t = 0; t < 16; t++ ) {
            writer->Double( mat[t] );
        }
        writer->EndArray();
    }

    template<std::size_t N>
	void serialize( const char* _name, const std::array<int32_t, N>& array ) {
		if ( _name != nullptr ) writer->String( _name );
		writer->StartArray();
		for ( auto& value : array ) {
			writer->Int( value );
		}
		writer->EndArray();
	}

	void serialize( const char* name, const std::vector<uint32_t>& value ) {
        if ( name != nullptr ) writer->String( name );
		writer->StartArray();
		for ( uint64_t t = 0; t < value.size(); t++ ) {
			writer->Uint( value[t] );
		}
		writer->EndArray();
	}


	void serialize( const char* name, const std::vector<int32_t>& value ) {
        if ( name != nullptr ) writer->String( name );
		writer->StartArray();
		for ( uint64_t t = 0; t < value.size(); t++ ) {
			writer->Int( value[t] );
		}
		writer->EndArray();
	}


	void serialize( const char* name, const std::vector<uint64_t>& value ) {
        if ( name != nullptr ) writer->String( name );
		writer->StartArray();
		for ( uint64_t t = 0; t < value.size(); t++ ) {
			writer->Uint64( value[t] );
		}
		writer->EndArray();
	}


	void serialize( const char* name, const std::vector<int64_t>& value ) {
        if ( name != nullptr ) writer->String( name );
		writer->StartArray();
		for ( uint64_t t = 0; t < value.size(); t++ ) {
			writer->Int64( value[t] );
		}
		writer->EndArray();
	}


	void serialize( const char* name, const std::vector<float>& value ) {
        if ( name != nullptr ) writer->String( name );
		writer->StartArray();
		for ( uint64_t t = 0; t < value.size(); t++ ) {
			writer->Double( value[t] );
		}
		writer->EndArray();
	}

	void serialize( const char* name, const std::vector<double>& value ) {
        if ( name != nullptr ) writer->String( name );
		writer->StartArray();
		for ( uint64_t t = 0; t < value.size(); t++ ) {
			writer->Double( value[t] );
		}
		writer->EndArray();
	}

	void serialize( const char* name, const std::vector<std::string>& value ) {
        if ( name != nullptr ) writer->String( name );
		writer->StartArray();
		for ( uint64_t t = 0; t < value.size(); t++ ) {
			writer->String( value[t].c_str() );
		}
		writer->EndArray();
	}

	void serialize( const char* name, const std::vector<std::pair<std::string, std::string>>& value ) {
        if ( name != nullptr ) writer->String( name );
		writer->StartArray();
		for ( const auto& t : value ) {
			writer->StartArray();
			writer->String( t.first.c_str() );
			writer->String( t.second.c_str() );
			writer->EndArray();
		}
		writer->EndArray();
	}

	void serialize( const char* name, const std::vector<JMATH::Rect2f>& value ) {
        if ( name != nullptr ) writer->String( name );
		writer->StartArray();
		for ( uint64_t t = 0; t < value.size(); t++ ) {
			serialize( name, value[t] );
		}
		writer->EndArray();
	}

	void serialize( const char* _name, const std::vector<std::vector<double>>& array ) {
		if ( array.size() > 0 ) {
			if ( _name != nullptr ) writer->String( _name );
			writer->StartArray();
			for ( auto subarray : array ) {
				writer->StartArray();
				for ( auto value : subarray ) {
					writer->Double( value );
				}
				writer->EndArray();
			}
			writer->EndArray();
		}
	}

	template<std::size_t N>
	void serialize( const char* _name, const std::vector<std::array<double, N>>& array ) {
		if ( array.size() > 0 ) {
			if ( _name != nullptr ) writer->String( _name );
			writer->StartArray();
			for ( auto subarray : array ) {
				writer->StartArray();
				for ( auto value : subarray ) {
					writer->Double( value );
				}
				writer->EndArray();
			}
			writer->EndArray();
		}
	}

	void serialize( const char* _name, const std::vector<std::vector<Vector2f>>& array ) {
		if ( array.size() > 0 ) {
			if ( _name != nullptr ) writer->String( _name );
			writer->StartArray();
			for ( auto subarray : array ) {
				writer->StartArray();
				for ( auto value : subarray ) {
					writer->StartArray();
					for ( int t = 0; t < value.size(); t++ ) {
						writer->Double( value[t] );
					}
					writer->EndArray();
				}
				writer->EndArray();
			}
			writer->EndArray();
		}
	}


	void serialize( const char* _name, const std::vector<std::vector<Vector3f>>& array ) {
		if ( array.size() > 0 ) {
			if ( _name != nullptr ) writer->String( _name );
			writer->StartArray();
			for ( auto subarray : array ) {
				writer->StartArray();
				for ( auto value : subarray ) {
					writer->StartArray();
					for ( int t = 0; t < value.size(); t++ ) {
						writer->Double( value[t] );
					}
					writer->EndArray();
				}
				writer->EndArray();
			}
			writer->EndArray();
		}
	}

//	void serialize( const CoreMetaData& _md ) {
//		serialize( MetaData::Name.c_str(), _md.getName() );
//		serialize( MetaData::Type.c_str(), _md.getType() );
//		serialize( MetaData::Hash.c_str(), _md.getHash() );
//		serialize( MetaData::Thumb.c_str(), _md.getThumb() );
//		serialize( MetaData::Raw.c_str(), _md.getRaw() );
//		serialize( MetaData::Tags.c_str(), _md.getTags() );
//	}

private:
	std::unique_ptr<Writer<StringBuffer>> writer;
	StringBuffer internalOS;
};

template <typename T>
inline std::string serializeArray( const std::vector<T>& _array ) {
    MegaWriter writer;
    writer.serialize( nullptr, _array);
    return writer.getString();
}
