//
//  megawriter.hpp
//  sixthview
//
//  Created by Dado on 01/10/2015.
//
//

#pragma once

#include <array>
#include <event_horizon/core/math/matrix_anim.h>

#include "core/htypes_shared.hpp"
#include "core/math/rect2f.h"
#include "core/math/aabb.h"
#include "core/math/vector4f.h"
#include "core/serialization.hpp"

#include "imgui.h"

namespace ImGUIJsonColors {
	const static ImVec4 Key = ImVec4(0.7f, 0.95f, 0.7f, 1.0f);
	const static ImVec4 String  = ImVec4(0.55f, 0.6f, 0.7f, 1.0f);
	const static ImVec4 Int = ImVec4(0.2f, 0.7f, 0.7f, 1.0f);
	const static ImVec4 Float = ImVec4(0.3f, 0.5f, 0.5f, 1.0f);
	const static ImVec4 Float2 = ImVec4(0.9f, 0.6f, 0.2f, 1.0f);
	const static ImVec4 Float3 = ImVec4(0.9f, 0.6f, 0.3f, 1.0f);
	const static ImVec4 Float4 = ImVec4(0.9f, 0.6f, 0.4f, 1.0f);
	const static ImVec4 SquareBracket = ImVec4(0.1f, 0.7f, 0.8f, 1.0f);
	const static ImVec4 CurlyBracket  = ImVec4(0.95f, 0.7f, 0.8f, 1.0f);
	const static ImVec4 Danger  = ImVec4(0.95f, 0.0f, 0.0f, 1.0f);
}

static void drawNodeRootBox( const char* value ) {
	ImGui::Begin( value, nullptr, ImGuiWindowFlags_NoCollapse );
	ImGui::End();
}

static void drawKey( const char* _name ) {
	if ( _name != nullptr ) ImGui::TextColored( ImGUIJsonColors::Key, "%s", _name );
}

template<typename T>
static void drawPair( const char* _name, const std::pair<T,T>& value ) {
	drawKey( _name );

	auto fc = ImGUIJsonColors::Int;
	ImGui::SameLine();
	ImGui::TextColored( ImGUIJsonColors::SquareBracket, "%s", "<" );
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::TextColored( fc, "%d", value.first );
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::TextColored( ImGUIJsonColors::SquareBracket, "%s", "," );
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::TextColored( fc, "%d", value.second );
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::TextColored( ImGUIJsonColors::SquareBracket, "%s", ">" );
}

template <typename T>
static void drawVector( const T& value ) {
	size_t size = value.size();
	ImGui::SameLine();
	ImGui::TextColored( ImGUIJsonColors::SquareBracket, "%s", "[" );
	auto fc = ImGUIJsonColors::Float2;
	switch ( size ) {
		case 3:
			fc = ImGUIJsonColors::Float3;
			break;
		case 4:
			fc = ImGUIJsonColors::Float4;
			break;
		default:
			fc = ImGUIJsonColors::Float2;
	}
	for ( size_t st = 0; st < size; st++ ) {
		ImGui::SameLine(0.0f, 0.0f);
		drawFloatFormatted( value[st], fc );
		if ( st != size - 1 ) {
			ImGui::SameLine(0.0f, 0.0f);
			ImGui::TextColored( ImGUIJsonColors::SquareBracket, "%s", "," );
		}
	}
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::TextColored( ImGUIJsonColors::SquareBracket, "%s", "]" );
}

static void drawBool( const char* _name, const bool& value ) {
	drawKey( _name );
	ImGui::SameLine();
	ImGui::Text( "%s", value ? "true" : "false" );
}

template<typename T>
static void drawVector( const char* _name, const T& value ) {
	drawKey( _name );
	drawVector( value );
}

template<typename T>
static void drawInt( const T& value ) {
	ImGui::SameLine();
#if LINUX
	ImGui::Text( "%ld", static_cast<int64_t>(value) );
#else
	ImGui::Text( "%lld", static_cast<int64_t>(value) );
#endif
}

template<typename T>
static void drawFloatFormatted( const T& value, const ImVec4& col ) {
	if ( value == std::numeric_limits<float>::lowest() ) {
		ImGui::TextColored( ImGUIJsonColors::Danger, "-INF" );
		return;
	}
	if ( value == std::numeric_limits<float>::max() ) {
		ImGui::TextColored( ImGUIJsonColors::Danger, "INF" );
		return;
	}
	ImGui::TextColored( col, "%.02f", value );
}

template<typename T>
static void drawFloat( const T& value, const ImVec4& col = ImVec4{1,1,1,1} ) {
	ImGui::SameLine();
	drawFloatFormatted( value, col );
}

template<typename T>
static void drawInt( const char* _name, const T& value ) {
	ImGui::TextColored( ImGUIJsonColors::Key, "%s", _name );
	drawInt( value );
}

static void drawString( const char* value ) {
	ImGui::SameLine();
	ImGui::TextColored( ImGUIJsonColors::String, "%s", value );

//	static char buf[1024];
//    std::strcpy( buf, value );
//    if ( ImGui::InputText( "", buf, 1024, ImGuiInputTextFlags_EnterReturnsTrue ) ) {
//        LOGR( buf );
//        // Inser code here
//    };
}

static void drawString( const char* _name, const char* value ) {
	drawKey( _name );
	drawString( value );
}

template<typename T, std::size_t N>
static void drawArrayOfInt( const char* _name, const std::array<T, N>& array ) {
	drawKey( _name );
	if ( ImGui::TreeNode( _name )) {
		for ( auto& value : array ) {
			drawInt( value );
		}
		ImGui::TreePop();
	}
}

template<typename T>
static void drawArrayOfInt( const char* _name, const std::vector<T>& array ) {
	drawKey( _name );
	if ( ImGui::TreeNode( _name )) {
		for ( auto& value : array ) {
			drawInt( value );
		}
		ImGui::TreePop();
	}
}

template<typename T>
static void drawFloat( const char* _name, const T& value ) {
	ImGui::TextColored( ImGUIJsonColors::Key, "%s", _name );
	drawFloat( value );
}

template<typename T>
static void drawArrayOfFloat( const char* _name, const std::vector<T>& array ) {
	//drawKey( _name );
	if ( ImGui::TreeNode( _name )) {
		for ( auto& value : array ) {
			drawFloat( value );
		}
		ImGui::TreePop();
	}
}

template<typename T, std::size_t N>
static void drawArrayOfFloat( const char* _name, const std::array<T, N>& array ) {
	//drawKey( _name );
	if ( ImGui::TreeNode( _name )) {
		for ( auto& value : array ) {
			drawFloat( value );
		}
		ImGui::TreePop();
	}
}

static void drawArrayOfString( const char* _name, const std::vector<std::string>& array ) {
	drawKey( _name );
	if ( ImGui::TreeNode( _name )) {
		for ( auto& value : array ) {
			drawString( value.c_str() );
		}
		ImGui::TreePop();
	}
}

static void drawRect( const Rect2f& rect ) {
	drawVector( Vector2f{ rect.left(), rect.top() } );
	ImGui::SameLine( 0.0f, 0.0f );
	ImGui::TextColored( ImGUIJsonColors::SquareBracket, "%s", "," );
	drawVector( Vector2f{ rect.right(), rect.bottom() } );
}

static void drawRect( const char* _name, const Rect2f& value ) {
	drawKey( _name );
	drawRect( value );
}

static void drawArrayOfRect( const char* _name, const std::vector<Rect2f>& array ) {
	drawKey( _name );
	if ( ImGui::TreeNode( _name )) {
		for ( auto& value : array ) {
			drawRect( value );
		}
		ImGui::TreePop();
	}
}

template <typename T>
static void drawArrayOfVector( const char* _name, const std::vector<T>& array ) {
	//drawKey( _name );
	if (ImGui::TreeNode(_name)) {
		for ( auto& value : array ) {
			drawVector( value );
		}
		ImGui::TreePop();
	}
}

template <typename T>
static std::string arrayName( const char* _name, const std::vector<T>& array ) {
	std::string ret = std::string(_name);
	ret += " (";
	ret += std::to_string(array.size());
	ret += ")";
	return ret;
}

static inline void drawStringRoot( const char* _name, const char* value ) {
	if ( strcmp(_name, "Name") == 0 ) {
		drawNodeRootBox( value );
	} else {
		drawString( _name, value );
	}
}

class ImGuiStandardJson {
public:

	static void visit( const char* _name, const char* value ) {
		drawString(_name, value);
	}

	static void visit( const char* _name, const std::string& value ) {
		drawString( _name, value.c_str() );
	}

	static void visit( const char* _name, const double& value ) {
		drawFloat( _name, value );
	}

	static void visit( const char* _name, const float& value ) {
		drawFloat( _name, value );
	}

	static void visit( const char* _name, const bool& value ) {
		drawBool( _name, value );
	}

	static void visit( const char* _name, const int32_t& value ) {
		drawInt( _name, value );
	}

	static void visit( const char* _name, const uint32_t& value ) {
		drawInt( _name, value );
	}

	static void visit( const char* _name, const int64_t& value ) {
		drawInt( _name, value );
	}

	static void visit( const char* _name, const uint64_t& value ) {
		drawInt( _name, value );
	}

	static void visit( const char* _name, const Vector2f& value ) {
		drawVector( _name, value );
	}

	static void visit( const char* _name, const Vector3f& value ) {
		drawVector( _name, value );
	}

	static void visit( const char* _name, const Vector4f& value ) {
		drawVector( _name, value );
	}

	static void visit( const char* _name, const std::pair<int, int>& value ) {
		drawPair( _name, value );
	}

	static void visit( const char* _name, const JMATH::Rect2f& rect ) {
		drawRect( _name, rect );
	}

	static void visit( const char* _name, const Quaternion& q ) {
		drawVector( _name, q );
	}

	static void visit( [[maybe_unused]] const char* _name, const MatrixAnim& q ) {
		drawVector( "Pos  ", q.pos->value );
		drawVector( "Rot  ", q.rot->value );
		drawVector( "Scale", q.scale->value );
	}

	static void visit( const char* _name, const JMATH::AABB& rect ) {
		drawKey( _name );
		drawVector( rect.minPoint() );
		drawVector( rect.maxPoint() );
	}

};

#define arrayVisitStart \
    if ( array.empty()) return; \
	if (ImGui::TreeNode( arrayName( _name, array).c_str() )) { \
		for ( size_t i = 0; i < array.size(); i++ ) { \
			if (ImGui::TreeNode(std::to_string(i).c_str())) { \
				array[i]

#define arrayVisitEnd \
					template visit<C>(); \
				ImGui::TreePop(); \
			} \
		} \
		ImGui::TreePop(); \
	}

#define arrayVisitDot arrayVisitStart . arrayVisitEnd
#define arrayVisitPtr arrayVisitStart -> arrayVisitEnd

class ImGuiStandardJsonArrays {
public:
	template <typename C, typename T>
	static void visit( const char* _name, const T& value ) {
		if (ImGui::TreeNode(_name)) {
			value.template visit<C>();
			ImGui::TreePop();
		}
	}

	template <typename C, typename T>
	static void visit( const char* _name, std::shared_ptr<T> value ) {
		if ( value ) {
			if (ImGui::TreeNode(_name)) {
				value->template visit<C>();
				ImGui::TreePop();
			}
		}
	}

	template <typename C, typename T>
	static void visit( const char* _name, const std::vector<std::shared_ptr<T>>& array ) {
        arrayVisitPtr
	}

	template <typename C, typename T>
	static void visit( const char* _name, const std::vector<T>& array ) {
		arrayVisitDot
	}

	template<typename C, typename T, std::size_t N>
	static void visit( const char* _name, const std::array<T, N>& array ) {
		arrayVisitDot
	}

    template<std::size_t N>
    static void visit( const char* _name, const std::array<int32_t, N>& array ) {
		drawArrayOfInt( _name, array );
    }

    template<std::size_t N>
    static void visit( const char* _name, const std::array<Vector2f, N>& array ) {
		drawKey( _name );
		if (ImGui::TreeNode(_name)) {
			for ( auto& value : array ) {
				drawVector( value );
			}
			ImGui::TreePop();
		}
    }

	static void visit( const char* _name, const std::vector<Vector2f>& array ) {
		drawArrayOfVector( _name, array );
	}

	static void visit( const char* _name, const std::vector<Triangle2d>& array ) {
		drawKey( _name );
		for ( auto& value : array ) {
			drawVector( Vector2f{ std::get<0>( value ).x(), std::get<0>( value ).y()} );
			ImGui::SameLine(0.0f, 0.0f);
			ImGui::TextColored( ImGUIJsonColors::SquareBracket, "%s", "," );
			drawVector( Vector2f{ std::get<1>( value ).x(), std::get<1>( value ).y()} );
			ImGui::SameLine(0.0f, 0.0f);
			ImGui::TextColored( ImGUIJsonColors::SquareBracket, "%s", "," );
			drawVector( Vector2f{ std::get<2>( value ).x(), std::get<2>( value ).y()} );
		}
	}

	static void visit( const char* _name, const std::vector<Vector3f>& array ) {
		drawArrayOfVector( _name, array );
	}

	static void visit( const char* _name, const std::vector<Vector4f>& array ) {
		drawArrayOfVector( _name, array );
	}

	static void visit( const char* _name, const std::vector<uint32_t>& array ) {
		drawArrayOfInt( _name, array );
	}

	static void visit( const char* _name, const std::vector<int32_t>& array ) {
		drawArrayOfInt( _name, array );
	}

	static void visit( const char* _name, const std::vector<uint64_t>& array ) {
		drawArrayOfInt( _name, array );
	}

	static void visit( const char* _name, const std::vector<int64_t>& array ) {
		drawArrayOfInt( _name, array );
	}

	static void visit( const char* _name, const std::vector<float>& array ) {
		drawArrayOfFloat( _name, array );
	}

	static void visit( const char* _name, const std::vector<double>& array ) {
		drawArrayOfFloat( _name, array );
	}

	static void visit( const char* _name, const std::vector<std::string>& array ) {
		drawArrayOfString( _name, array );
	}

	static void visit( const char* _name, const std::vector<JMATH::Rect2f>& array ) {
		drawArrayOfRect( _name, array );
	}

	template <typename C, typename T>
	static void visit( const char* _name, const std::vector<std::vector<T>>& arrayOfArray ) {
		if ( arrayOfArray.empty() ) return;
		if (ImGui::TreeNode(_name))
		{
			for ( auto& array : arrayOfArray ) {
				arrayVisitDot
			}
			ImGui::TreePop();
		}
	}

	static void visit( const char* _name, const std::vector<std::vector<double>>& array ) {
		drawKey( _name );
		int q = 0;
		for ( auto subarray : array ) {
			drawArrayOfFloat( std::to_string(q++).c_str(), subarray );
		}
	}

	template<std::size_t N>
	static void visit( const char* _name, const std::vector<std::array<double, N>>& array ) {
		drawKey( _name );
		int q = 0;
		for ( auto subarray : array ) {
			drawArrayOfFloat( std::to_string(q++).c_str(), subarray );
		}
	}

	static void visit( const char* _name, const std::vector<std::vector<Vector2f>>& array ) {
		drawKey( _name );
		int q = 0;
		for ( auto subarray : array ) {
			drawArrayOfVector( std::to_string(q++).c_str(), subarray );
		}
	}

	static void visit( const char* _name, const std::vector<std::vector<Vector3f>>& array ) {
		drawKey( _name );
		int q = 0;
		for ( auto subarray : array ) {
			drawArrayOfVector( std::to_string(q++).c_str(), subarray );
		}
	}

};

class ImGuiStandardJsonArraysNamed {
public:
	template <typename C, typename T>
	static void visit( [[maybe_unused]] const char* _name, const T& value ) {
		if (ImGui::TreeNode(value.Name().c_str())) {
			value.template visit<C>();
			ImGui::TreePop();
		}
	}

	template <typename C, typename T>
	static void visit( [[maybe_unused]] const char* _name, std::shared_ptr<T> value ) {
		if ( value ) {
			if (ImGui::TreeNode(value->Name().c_str())) {
				value->template visit<C>();
				ImGui::TreePop();
			}
		}
	}

	template <typename C, typename T>
    static void visit( const char* _name, const std::vector<std::shared_ptr<T>>& array ) {
        if ( array.empty()) return;
        if ( array.size() == 1 ) {
			if ( ImGui::TreeNode( array[0]->Name().c_str())) {
				array[0]->template visit<C>();
				ImGui::TreePop();
			}
        } else {
			if (ImGui::TreeNode( arrayName( _name, array).c_str() )) {
				for ( size_t i = 0; i < array.size(); i++ ) {
					if ( ImGui::TreeNode( array[i]->Name().c_str())) {
						array[i]->template visit<C>();
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
        }
    }

	template <typename C, typename T>
	static void visit( const char* _name, const std::vector<T>& array ) {
		if ( array.empty()) return;
		if ( array.size() == 1 ) {
			if ( ImGui::TreeNode( array[0].Name().c_str())) {
				array[0].template visit<C>();
				ImGui::TreePop();
			}
		} else {
			if (ImGui::TreeNode( arrayName( _name, array).c_str() )) {
				for ( size_t i = 0; i < array.size(); i++ ) {
					if ( ImGui::TreeNode( array[i]->Name().c_str())) {
						array[i].template visit<C>();
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
		}
	}

    template<typename C, typename T, std::size_t N>
    static void visit( const char* _name, const std::array<T, N>& array ) {
        arrayVisitDot
    }

    template<std::size_t N>
    static void visit( const char* _name, const std::array<int32_t, N>& array ) {
        drawArrayOfInt( _name, array );
    }

    template<std::size_t N>
    static void visit( const char* _name, const std::array<Vector2f, N>& array ) {
        drawKey( _name );
        if (ImGui::TreeNode(_name)) {
            for ( auto& value : array ) {
                drawVector( value );
            }
            ImGui::TreePop();
        }
    }

	static void visit( const char* _name, const std::vector<Vector2f>& array ) {
		drawArrayOfVector( _name, array );
	}

	static void visit( const char* _name, const std::vector<Triangle2d>& array ) {
		drawKey( _name );
		for ( auto& value : array ) {
			drawVector( Vector2f{ std::get<0>( value ).x(), std::get<0>( value ).y()} );
			ImGui::SameLine(0.0f, 0.0f);
			ImGui::TextColored( ImGUIJsonColors::SquareBracket, "%s", "," );
			drawVector( Vector2f{ std::get<1>( value ).x(), std::get<1>( value ).y()} );
			ImGui::SameLine(0.0f, 0.0f);
			ImGui::TextColored( ImGUIJsonColors::SquareBracket, "%s", "," );
			drawVector( Vector2f{ std::get<2>( value ).x(), std::get<2>( value ).y()} );
		}
	}

	static void visit( const char* _name, const std::vector<Vector3f>& array ) {
		drawArrayOfVector( _name, array );
	}

	static void visit( const char* _name, const std::vector<Vector4f>& array ) {
		drawArrayOfVector( _name, array );
	}

	static void visit( const char* _name, const std::vector<uint32_t>& array ) {
		drawArrayOfInt( _name, array );
	}

	static void visit( const char* _name, const std::vector<int32_t>& array ) {
		drawArrayOfInt( _name, array );
	}

	static void visit( const char* _name, const std::vector<uint64_t>& array ) {
		drawArrayOfInt( _name, array );
	}

	static void visit( const char* _name, const std::vector<int64_t>& array ) {
		drawArrayOfInt( _name, array );
	}

	static void visit( const char* _name, const std::vector<float>& array ) {
		drawArrayOfFloat( _name, array );
	}

	static void visit( const char* _name, const std::vector<double>& array ) {
		drawArrayOfFloat( _name, array );
	}

	static void visit( const char* _name, const std::vector<std::string>& array ) {
		drawArrayOfString( _name, array );
	}

	static void visit( const char* _name, const std::vector<JMATH::Rect2f>& array ) {
		drawArrayOfRect( _name, array );
	}

	template <typename C, typename T>
	static void visit( const char* _name, const std::vector<std::vector<T>>& arrayOfArray ) {
		if ( arrayOfArray.empty() ) return;
		if (ImGui::TreeNode(_name))
		{
			for ( auto& array : arrayOfArray ) {
				arrayVisitDot
			}
			ImGui::TreePop();
		}
	}

	static void visit( const char* _name, const std::vector<std::vector<double>>& array ) {
		drawKey( _name );
		int q = 0;
		for ( auto subarray : array ) {
			drawArrayOfFloat( std::to_string(q++).c_str(), subarray );
		}
	}

	template<std::size_t N>
	static void visit( const char* _name, const std::vector<std::array<double, N>>& array ) {
		drawKey( _name );
		int q = 0;
		for ( auto subarray : array ) {
			drawArrayOfFloat( std::to_string(q++).c_str(), subarray );
		}
	}

	static void visit( const char* _name, const std::vector<std::vector<Vector2f>>& array ) {
		drawKey( _name );
		int q = 0;
		for ( auto subarray : array ) {
			drawArrayOfVector( std::to_string(q++).c_str(), subarray );
		}
	}

	static void visit( const char* _name, const std::vector<std::vector<Vector3f>>& array ) {
		drawKey( _name );
		int q = 0;
		for ( auto subarray : array ) {
			drawArrayOfVector( std::to_string(q++).c_str(), subarray );
		}
	}

};

class ImGUIJsonNamed : public JVisitor<ImGUIJsonNamed, ImGuiStandardJson, ImGuiStandardJsonArraysNamed> {};

class ImGUIJson : public JVisitor<ImGUIJson, ImGuiStandardJson, ImGuiStandardJsonArrays> {};
