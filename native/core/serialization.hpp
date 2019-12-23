//
//  serialization.hpp
//  sixthview
//
//  Created by Dado on 01/10/2017.
//
//

#pragma once

#include "megawriter.hpp"
#include "megareader.hpp"
#include "string_util.h"

template <typename J, typename B, typename A>
class JVisitor {
public:
	template<typename T>
	void visit( const char* _name, const T& value ) {
		A::template visit<J,T>( _name, value );
	}

	template<typename T>
	void visit( const char* _name, std::shared_ptr<T> value ) {
		A::template visit<J,T>( _name, value );
	}

	void visit( const char* _name, const std::string& value ) {
		B::visit( _name, value );
	}

	void visit( const char* _name, const char* value ) {
		B::visit( _name, value );
	}

	void visit( const char* _name, const double& value ) {
		B::visit( _name, value );
	}

	void visit( const char* _name, const float& value ) {
		B::visit( _name, value );
	}

	void visit( const char* _name, const bool& value ) {
		B::visit( _name, value );
	}

	void visit( const char* _name, const int32_t& value ) {
		B::visit( _name, value );
	}

	void visit( const char* _name, const uint32_t& value ) {
		B::visit( _name, value );
	}

	void visit( const char* _name, const int64_t& value ) {
		B::visit( _name, value );
	}

	void visit( const char* _name, const uint64_t& value ) {
		B::visit( _name, value );
	}

	void visit( const char* _name, const Vector2f& value ) {
		B::visit( _name, value );
	}

	void visit( const char* _name, const Vector3f& value ) {
		B::visit( _name, value );
	}

	void visit( const char* _name, const Vector4f& value ) {
		B::visit( _name, value );
	}

	void visit( const char* _name, const std::pair<int, int>& value ) {
		B::visit( _name, value );
	}

	void visit( const char* _name, const JMATH::Rect2f& value ) {
		B::visit( _name, value );
	}

	void visit( const char* _name, const Quaternion& value ) {
		B::visit( _name, value );
	}

	void visit( const char* _name, const MatrixAnim& value ) {
		B::visit( _name, value );
	}

	void visit( const char* _name, const JMATH::AABB& value ) {
		B::visit( _name, value );
	}

	// std::array

	template<typename T, std::size_t N>
	void visit( const char* _name, const std::array<T, N>& array ) {
		A::visit( _name, array );
	}

	template<std::size_t N>
	void visit( const char* _name, const std::array<int32_t, N>& array ) {
		A::visit( _name, array );
	}

	template<std::size_t N>
	void visit( const char* _name, const std::array<Vector2f, N>& array ) {
		A::visit( _name, array );
	}

	// std::vector

	template<typename T>
	void visit( const char* _name, const std::vector<std::shared_ptr<T>>& array ) {
		A::template visit<J,T>( _name, array );
	}

	template<typename T>
	void visit( const char* _name, const std::vector<T>& array ) {
		A::template visit<J,T>( _name, array );
	}

	void visit( const char* _name, const std::vector<Vector2f>& array ) {
		A::visit( _name, array );
	}

	void visit( const char* _name, const std::vector<Triangle2d>& array ) {
		A::visit( _name, array );
	}

	void visit( const char* _name, const std::vector<Vector3f>& array ) {
		A::visit( _name, array );
	}

	void visit( const char* _name, const std::vector<Vector4f>& array ) {
		A::visit( _name, array );
	}

	void visit( const char* _name, const std::vector<uint32_t>& array ) {
		A::visit( _name, array );
	}

	void visit( const char* _name, const std::vector<int32_t>& array ) {
		A::visit( _name, array );
	}

	void visit( const char* _name, const std::vector<uint64_t>& array ) {
		A::visit( _name, array );
	}

	void visit( const char* _name, const std::vector<int64_t>& array ) {
		A::visit( _name, array );
	}

	void visit( const char* _name, const std::vector<float>& array ) {
		A::visit( _name, array );
	}

	void visit( const char* _name, const std::vector<double>& array ) {
		A::visit( _name, array );
	}

	void visit( const char* _name, const std::vector<std::string>& array ) {
		A::visit( _name, array );
	}

	void visit( const char* _name, const std::vector<JMATH::Rect2f>& array ) {
		A::visit( _name, array );
	}

	// std::vector of vector

	template<typename T>
	void visit( const char* _name, const std::vector<std::vector<T>>& arrayOfArray ) {
		A::template visit<J,T>( _name, arrayOfArray );
	}

	void visit( const char* _name, const std::vector<std::vector<double>>& arrayOfArray ) {
		A::visit( _name, arrayOfArray );
	}

	template<std::size_t N>
	void visit( const char* _name, const std::vector<std::array<double, N>>& arrayOfArray ) {
		A::visit( _name, arrayOfArray );
	}

	void visit( const char* _name, const std::vector<std::vector<Vector2f>>& arrayOfArray ) {
		A::visit( _name, arrayOfArray );
	}

	void visit( const char* _name, const std::vector<std::vector<Vector3f>>& arrayOfArray ) {
		A::visit( _name, arrayOfArray );
	}

};

//specialize a type for all of the STL containers.
namespace is_stl_container_impl {
template <typename T>       struct is_stl_container :std::false_type {};
template <typename... Args> struct is_stl_container<std::vector            <Args...>> :std::true_type {};
}

//type trait to utilize the implementation type traits as well as decay the type
template <typename T> struct is_stl_container {
	static constexpr bool const value = is_stl_container_impl::is_stl_container<std::decay_t<T>>::value;
};

template <typename T>
static void sseserialize( MegaWriter* visitor, const std::string& name, const T& value ) {
	visitor->serialize( name.c_str(), value );
}

template <typename V, typename T>
static void visit( const std::string& name, const T& value ) {
	V{}.visit( name.c_str(), value );
}

template <typename T>
static void sdeeserialize( const MegaReader& visitor, const std::string& name, T& value ) {
	visitor.deserialize( name.c_str(), value );
}

#define JSONSERIALONLY(CLASSNAME,...) \
	inline void serialize( MegaWriter* visitor ) const { visitor->StartObject(); serializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); visitor->EndObject(); } \
	inline SerializableContainer serialize() const { MegaWriter mw; serialize(&mw); return mw.getSerializableContainer();} \
	inline void deserialize( const MegaReader& visitor ) { deserializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); }

#define JSONSERIAL(CLASSNAME,...) \
    RESOURCE_CTORS(CLASSNAME); \
	explicit CLASSNAME( const MegaReader& reader ) { deserialize( reader ); } \
	inline void serialize( MegaWriter* visitor ) const { visitor->StartObject(); serializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); visitor->EndObject(); } \
	inline SerializableContainer serialize() const { MegaWriter mw; serialize(&mw); return mw.getSerializableContainer();} \
	inline std::string serializeString() const { MegaWriter mw; serialize(&mw); return mw.getString();} \
	inline void deserialize( const MegaReader& visitor ) { deserializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); } \
	void bufferDecode( const std::string& _json ) { \
	    bufferDecode( reinterpret_cast<const unsigned char *>( _json.data() ), _json.size() ); \
    } \
	void bufferDecode( const unsigned char *_buffer, size_t _length ) { \
        std::string strBuff{ _buffer, _buffer + _length}; \
        rapidjson::Document document; \
        document.Parse<rapidjson::kParseStopWhenDoneFlag>( strBuff.c_str() ); \
        MegaReader reader( document ); \
        deserialize( reader ); \
    }


#define JSONDATA(CLASSNAME,...) \
	struct CLASSNAME { \
	explicit CLASSNAME( const SerializableContainer& _str ) { \
		rapidjson::Document document; \
		document.Parse<rapidjson::kParseStopWhenDoneFlag>( reinterpret_cast<const char*>(_str.data()) ); \
		MegaReader reader( document ); \
		deserialize( reader ); } \
	CLASSNAME() = default; \
	explicit CLASSNAME( const std::string& _str ) { \
		rapidjson::Document document; \
		document.Parse<rapidjson::kParseStopWhenDoneFlag>( _str.c_str() ); \
		MegaReader reader( document ); \
		deserialize( reader ); } \
	explicit CLASSNAME( const MegaReader& reader ) { deserialize( reader ); } \
    template<typename TV> \
	void visit() const { traverseWithHelper<TV>( #__VA_ARGS__,__VA_ARGS__ ); } \
	std::string serialize() const { \
		MegaWriter writer; \
		serialize( &writer ); \
		return writer.getString(); \
	} \
	inline void serialize( MegaWriter* visitor ) const { visitor->StartObject(); serializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); visitor->EndObject(); } \
	inline void deserialize( const MegaReader& visitor ) { deserializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); } 

#define JSONDATA_H(CLASSNAME, PARENTCLASS,...) \
	struct CLASSNAME : public PARENTCLASS { \
	CLASSNAME() {} \
	virtual ~CLASSNAME() {} \
	CLASSNAME( const MegaReader& reader ) { deserialize( reader ); } \
	template<typename TV> \
	void visit() const { traverseWithHelper<TV>( #__VA_ARGS__,__VA_ARGS__ ); } \
	inline void serialize( MegaWriter* visitor ) const { visitor->StartObject(); serializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); visitor->EndObject(); } \
	inline void deserialize( const MegaReader& visitor ) { deserializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); } 

#define JSONDATA_R_H(CLASSNAME, PARENTCLASS,...) \
	struct CLASSNAME : public PARENTCLASS { \
	CLASSNAME() = default; \
	virtual ~CLASSNAME() {} \
	explicit CLASSNAME( const SerializableContainer& _str ) { \
		rapidjson::Document document; \
		document.Parse<rapidjson::kParseStopWhenDoneFlag>( reinterpret_cast<const char*>(_str.data()) ); \
		MegaReader reader( document ); \
		deserialize( reader ); } \
	explicit CLASSNAME( const std::string& _str ) { \
		rapidjson::Document document; \
		document.Parse<rapidjson::kParseStopWhenDoneFlag>( _str.c_str() ); \
		MegaReader reader( document ); \
		deserialize( reader ); } \
	CLASSNAME( uint8_p& _data ) { \
		rapidjson::Document document; \
		document.Parse<rapidjson::kParseStopWhenDoneFlag>( cbToString(std::move(_data)).c_str() ); \
		MegaReader reader( document ); \
		if ( !reader.isEmpty() ) { deserialize( reader );} \
	} \
	CLASSNAME( const MegaReader& reader ) { deserialize( reader ); bIsLoaded = true; } \
    inline SerializableContainer serialize() const { MegaWriter mw; serialize(&mw); return mw.getSerializableContainer();} \
	template<typename TV> \
	void visit() const { traverseWithHelper<TV>( #__VA_ARGS__,__VA_ARGS__ ); } \
	inline void serialize( MegaWriter* visitor ) const { visitor->StartObject(); serializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); visitor->EndObject(); } \
	inline void deserialize( const MegaReader& visitor ) { deserializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); } \
	bool bIsLoaded = false; \
    static bool usesNotExactQuery() { return false; }; \
    static std::string typeName() { return #CLASSNAME; }

#define JSONDATA_R(CLASSNAME,...) \
	struct CLASSNAME { \
	CLASSNAME() {} \
	CLASSNAME( const std::string& _json ) { load( _json ); } \
	CLASSNAME( uint8_p&& _data ) { load( cbToString(std::move(_data)) ); } \
	CLASSNAME( const MegaReader& reader ) { deserialize( reader ); bIsLoaded = true; } \
	template<typename TV> \
	void visit() const { traverseWithHelper<TV>( #__VA_ARGS__,__VA_ARGS__ ); } \
	void load( const std::string& _json ) { \
		rapidjson::Document document; \
		document.Parse<rapidjson::kParseStopWhenDoneFlag>( _json.c_str() ); \
		MegaReader reader( document ); \
		if ( !reader.isEmpty() ) { \
			if (reader.isArray() ) { \
				deserialize( reader.at( static_cast<rapidjson::SizeType>(0)) ); \
			} else { \
				deserialize( reader ); \
			} \
		} \
	} \
	inline void serialize( MegaWriter* visitor ) const { visitor->StartObject(); serializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); visitor->EndObject(); } \
	inline void deserialize( const MegaReader& visitor ) { deserializeWithHelper(visitor, #__VA_ARGS__, __VA_ARGS__ ); } \
	bool bIsLoaded = false; \
    static bool usesNotExactQuery() { return false; }; \
	static std::string typeName() { return #CLASSNAME; }

template<typename TVisitor,typename ...T>
inline int traverseWithHelper( std::string memberNames, T &...v ) {
	//split the names
	std::vector<std::string> names = split( memberNames, ',' );

	int i{};

	//for c++17 try just
	//((void)VisitWith(std::forward<T>(v)), ...);
	int x[] = { 0, ( (void)visit<TVisitor>( names[i++], v ), i )... };

	return x[names.size() - 1]; //return last one
}

template<typename TVisitor, typename ...T> inline int serializeWithHelper( TVisitor* visitor, std::string memberNames, T &...v ) {
	//split the names
	std::vector<std::string> names = split( memberNames, ',' );

	int i{};

	//for c++17 try just
	//((void)VisitWith(std::forward<T>(v)), ...);
	int x[] = { 0, ( (void)sseserialize( visitor, names[i++], v ), i )... };

	return x[names.size() - 1]; //return last one
}

template<typename TVisitor, typename ...T> inline int deserializeWithHelper( const TVisitor& visitor, std::string memberNames, T &...v ) {
	//split the names
	std::vector<std::string> names = split( memberNames, ',' );

	int i{};

	//for c++17 try just
	//((void)VisitWith(std::forward<T>(v)), ...);
	int x[] = { 0, ( (void)sdeeserialize( visitor, names[i++], v ), i )... };

	return x[names.size() - 1]; //return last one
}

JSONDATA(JsonError, type, msg)

    JsonError(const std::string &type, const std::string &msg) : type(type), msg(msg) {}

    std::string type;
    std::string msg;
};

inline std::string serializeLogger( const std::string type, const std::string& err ) {
    return JsonError{type, err}.serialize();
}
