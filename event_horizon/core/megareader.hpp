//
//  megaReader.hpp
//  sixthview
//
//  Created by Dado on 01/10/2015.
//
//

#pragma once

#include <vector>
#include <array>
#include <set>
#include "rapidjson/document.h"
#include "math/rect2f.h"
#include "math/aabb.h"
#include "metadata.h"

using namespace rapidjson;

class MegaReader {
private:
	const rapidjson::Value* value;
public:
	MegaReader( const rapidjson::Value& v ) {
		value = &v;
	}

	MegaReader( const rapidjson::Value* v ) {
		value = v;
	}

	bool isEmpty() const {
		if ( value->IsObject() && value->MemberCount() == 0 ) return true;
		if ( value->IsArray() && value->Size() == 0 ) return true;
		return false;
	}

	bool isArray() const {
		return value->IsArray();
	}

	std::vector<std::string> allMembers() const {
		std::vector<std::string> ret;
		for (auto itr = value->MemberBegin(); itr != value->MemberEnd(); ++itr) ret.push_back( itr->name.GetString() );
		return ret;
	}

	inline rapidjson::SizeType Size() const {
		return value->Size();
	}

	const rapidjson::Value* at( rapidjson::SizeType idx ) const {
		return &( ( *( value ) )[idx] );
	}

	const rapidjson::Value* at( const char* nameIndex ) const {
		if ( value->HasMember( nameIndex ) )
			return &( ( *( value ) )[nameIndex] );
		return nullptr;
	}

	const rapidjson::Value* at( const std::string& nameIndex ) const {
		if ( value->HasMember( nameIndex.c_str() ) )
			return &( ( *( value ) )[nameIndex.c_str()] );
		return nullptr;
	}

	bool hasMember( const char* name ) const {
		return value->HasMember( name );
	}

	// Serializing

	template<typename T>
	void deserialize( const char* name, T& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			ret = T{ ( *( value ) )[name] };
		}
	}


	void deserialize( const char* name, float& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			ret = ( *( value ) )[name].GetFloat();
		}
	}


	void deserialize( const char* name, bool& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			ret = static_cast<bool>( ( *( value ) )[name].GetBool() );
		}
	}


	void deserialize( const char* name, int32_t& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			ret = ( *( value ) )[name].GetInt();
		}
	}


	void deserialize( const char* name, uint32_t& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			ret = ( *( value ) )[name].GetInt();
		}
	}


	void deserialize( const char* name, uint64_t& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			ret = ( *( value ) )[name].GetInt64();
		}
	}


	void deserialize( const char* name, int64_t& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			ret = ( *( value ) )[name].GetInt64();
		}
	}


	void deserialize( const char* name, std::pair<int32_t, int32_t>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			int32_t i1 = ( *( value ) )[name][0].GetInt();
			int32_t i2 = ( *( value ) )[name][1].GetInt();
			ret = { i1, i2 };
		}
	}


	void deserialize( const char* name, std::string& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			ret = std::string( ( *( value ) )[name].GetString() );
		}
	}


	void deserialize( const char* name, JMATH::Rect2f& rect ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			rect = JMATH::Rect2f( ( *( value ) )[name][0].GetFloat(), ( *( value ) )[name][1].GetFloat(),
								  ( *( value ) )[name][2].GetFloat(),
								  ( *( value ) )[name][3].GetFloat() );
		}
	}


	void deserialize( const char* name, JMATH::AABB& rect ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			rect.setMinPoint( { ( *( value ) )[name][0].GetFloat(), ( *( value ) )[name][1].GetFloat(), ( *( value ) )[name][2].GetFloat() } );
			rect.setMaxPoint( { ( *( value ) )[name][3].GetFloat(), ( *( value ) )[name][4].GetFloat(), ( *( value ) )[name][5].GetFloat() } );
		}
	}


	void deserialize( const char* name, Vector2f& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				ret[t] = ( *( value ) )[name][t].GetFloat();
			}
		}
	}


	void deserialize( const char* name, Vector3f& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				ret[t] = ( *( value ) )[name][t].GetFloat();
			}
		}
	}

	void deserialize( const char* name, Vector4f& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				ret[t] = ( *( value ) )[name][t].GetFloat();
			}
		}
	}

	template<typename T>
	void deserialize( const char* name, std::vector<T>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				ret.push_back( T( ( *( value ) )[name][t] ) );
			}
		}
	}

	template<typename T>
	void deserialize( const char* name, std::set<T>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				ret.emplace( T( ( *( value ) )[name][t] ) );
			}
		}
	}

	template<typename T>
	void deserialize( const char* name, std::vector<std::shared_ptr<T>>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				ret.push_back( std::make_shared<T>( ( *( value ) )[name][t] ) );
			}
		}
	}

	void deserialize( const char* name, std::vector<Vector2f>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				Vector2f v1 = Vector2f::ZERO;
				for ( SizeType m = 0; m < ( *( value ) )[name][t].Size(); m++ ) {
					v1[m] = ( ( *( value ) )[name][t][m].GetFloat() );
				}
				ret.push_back( v1 );
			}
		}
	}

	void deserialize( const char* name, std::vector<Triangle2d>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t+=3 ) {
				Triangle2d sv;
				for ( SizeType m = 0; m < ( *( value ) )[name][t].Size(); m++ ) {
					std::get<0>(sv)[m] = ( *( value ) )[name][t][m].GetFloat();
				}
				for ( SizeType m = 0; m < ( *( value ) )[name][t+1].Size(); m++ ) {
					std::get<1>( sv )[m] = ( *( value ) )[name][t+1][m].GetFloat();
				}
				for ( SizeType m = 0; m < ( *( value ) )[name][t+2].Size(); m++ ) {
					std::get<2>( sv )[m] = ( *( value ) )[name][t+2][m].GetFloat();
				}
				ret.push_back( sv );
			}
		}
	}

	template<std::size_t N>
	void deserialize( const char* name, std::array<Vector2f, N>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				Vector2f v1;
				for ( SizeType m = 0; m < ( *( value ) )[name][t].Size(); m++ ) {
					v1[m] = ( ( *( value ) )[name][t][m].GetFloat() );
				}
				ret[t] = v1;
			}
		}
	}


	void deserialize( const char* name, std::vector<Vector3f>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				Vector3f v1;
				for ( SizeType m = 0; m < ( *( value ) )[name][t].Size(); m++ ) {
					v1[m] = ( ( *( value ) )[name][t][m].GetFloat() );
				}
				ret.push_back( v1 );
			}
		}
	}

	void deserialize( const char* name, std::vector<Vector4f>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				Vector4f v1;
				for ( SizeType m = 0; m < ( *( value ) )[name][t].Size(); m++ ) {
					v1[m] = ( ( *( value ) )[name][t][m].GetFloat() );
				}
				ret.push_back( v1 );
			}
		}
	}


	void deserialize( const char* name, std::vector<std::string>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				ret.push_back( ( *( value ) )[name][t].GetString() );
			}
		}
	}


	void deserialize( const char* name, std::vector<int32_t>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				ret.push_back( ( *( value ) )[name][t].GetInt() );
			}
		}
	}

	template<std::size_t N>
	void deserialize( const char* name, std::array<int32_t, N>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				ret[t] = ( ( *( value ) )[name][t].GetInt() );
			}
		}
	}

	void deserialize( const char* name, std::vector<uint32_t>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				ret.push_back( ( *( value ) )[name][t].GetUint() );
			}
		}
	}


	void deserialize( const char* name, std::vector<int64_t>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				ret.push_back( ( *( value ) )[name][t].GetInt64() );
			}
		}
	}


	void deserialize( const char* name, std::vector<uint64_t>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				ret.push_back( ( *( value ) )[name][t].GetUint64() );
			}
		}
	}


	void deserialize( const char* name, std::vector<float>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				ret.push_back( static_cast<float>( ( *( value ) )[name][t].GetDouble() ) );
			}
		}
	}


	void deserialize( const char* name, std::vector<double>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				ret.push_back( ( *( value ) )[name][t].GetDouble() );
			}
		}
	}

	template<typename T>
	void deserialize( const char* name, std::vector<std::vector<T>>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				std::vector<T> sv;
				for ( SizeType q = 0; q < ( *( value ) )[name][t].Size(); q++ ) {
					sv.push_back( T( ( *( value ) )[name][t][q] ) );
				}
				ret.push_back( sv );
			}
		}
	}

	void deserialize( const char* name, std::vector<std::vector<double>>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				std::vector<double> sv;
				for ( SizeType q = 0; q < ( *( value ) )[name][t].Size(); q++ ) {
					sv.push_back( ( *( value ) )[name][t][q].GetDouble() );
				}
				ret.push_back( sv );
			}
		}
	}

	template<std::size_t N>
	void deserialize( const char* name, std::vector<std::array<double, N>>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				std::array<double, N> sv;
				for ( SizeType q = 0; q < N; q++ ) {
					sv[q] = ( *( value ) )[name][t][q].GetDouble();
				}
				ret.push_back( sv );
			}
		}
	}

	void deserialize( const char* name, std::vector<std::vector<Vector2f>>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				std::vector<Vector2f> sv;
				for ( SizeType q = 0; q < ( *( value ) )[name][t].Size(); q++ ) {
					Vector2f v1;
					for ( SizeType m = 0; m < ( *( value ) )[name][t][q].Size(); m++ ) {
						v1[m] = ( *( value ) )[name][t][q][m].GetFloat();
					}
					sv.push_back( v1 );
				}
				ret.push_back( sv );
			}
		}
	}


	void deserialize( const char* name, std::vector<std::vector<Vector3f>>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				std::vector<Vector3f> sv;
				for ( SizeType q = 0; q < ( *( value ) )[name][t].Size(); q++ ) {
					Vector3f v1;
					for ( SizeType m = 0; m < ( *( value ) )[name][t][q].Size(); m++ ) {
						v1[m] = ( *( value ) )[name][t][q][m].GetFloat();
					}
					sv.push_back( v1 );
				}
				ret.push_back( sv );
			}
		}
	}


	void deserialize( const char* name, std::vector<std::vector<Vector4f>>& ret ) const {
		if ( value->FindMember( name ) != value->MemberEnd() ) {
			for ( SizeType t = 0; t < ( *( value ) )[name].Size(); t++ ) {
				std::vector<Vector4f> sv;
				for ( SizeType q = 0; q < ( *( value ) )[name][t].Size(); q++ ) {
					Vector4f v1;
					for ( SizeType m = 0; m < ( *( value ) )[name][t][q].Size(); m++ ) {
						v1[m] = ( *( value ) )[name][t][q][m].GetFloat();
					}
					sv.push_back( v1 );
				}
				ret.push_back( sv );
			}
		}
	}

	void deserialize( std::vector<CoreMetaData>& _vec ) {
		for ( SizeType t = 0; t < (*value).Size(); t++ ) {
			CoreMetaData elem;
			elem.setName( (*value)[t][MetaData::Name.c_str()].GetString() );
			elem.setType( (*value)[t][MetaData::Type.c_str()].GetString() );
			elem.setThumb( (*value)[t][MetaData::Thumb.c_str()].GetString() );
//			deserialize( MetaData::Name.c_str(),  elem.Name() );
//			deserialize( MetaData::Type.c_str(),  elem.Type() );
//			deserialize( MetaData::Thumb.c_str(), elem.Thumb() );
			_vec.push_back( elem );
		}
	}

};
