//
//  megawriter.hpp
//  sixthview
//
//  Created by Dado on 01/10/2015.
//
//

#pragma once

#include <array>

#include "core/htypes_shared.hpp"
#include "core/math/rect2f.h"
#include "core/math/aabb.h"
#include "core/math/vector4f.h"

#include "imgui.h"

namespace ImGUIJsonColors {
	const static ImVec4 Key = ImVec4(0.7f, 0.95f, 0.7f, 1.0f);
	const static ImVec4 Int = ImVec4(0.2f, 0.7f, 0.7f, 1.0f);
	const static ImVec4 Float = ImVec4(0.3f, 0.5f, 0.5f, 1.0f);
	const static ImVec4 Float2 = ImVec4(0.9f, 0.6f, 0.2f, 1.0f);
	const static ImVec4 Float3 = ImVec4(0.9f, 0.6f, 0.3f, 1.0f);
	const static ImVec4 Float4 = ImVec4(0.9f, 0.6f, 0.4f, 1.0f);
	const static ImVec4 SquareBracket = ImVec4(0.1f, 0.7f, 0.8f, 1.0f);
	const static ImVec4 CurlyBracket  = ImVec4(0.95f, 0.7f, 0.8f, 1.0f);
}

void drawKey( const char* _name ) {
	if ( _name != nullptr ) ImGui::TextColored( ImGUIJsonColors::Key, "%s", _name );
}

template<typename T>
void drawPair( const char* _name, const std::pair<T,T>& value ) {
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
void drawVector( const T& value ) {
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
	}
	for ( auto st = 0; st < size; st++ ) {
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::TextColored( fc, "%.02f", value[st] );
		if ( st != size - 1 ) {
			ImGui::SameLine(0.0f, 0.0f);
			ImGui::TextColored( ImGUIJsonColors::SquareBracket, "%s", "," );
		}
	}
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::TextColored( ImGUIJsonColors::SquareBracket, "%s", "]" );

}

void drawBool( const char* _name, const bool& value ) {
	drawKey( _name );
	ImGui::SameLine();
	ImGui::Text( "%s", value ? "true" : "false" );
}

template<typename T>
void drawVector( const char* _name, const T& value ) {
	drawKey( _name );
	drawVector( value );
}

template<typename T>
void drawInt( const T& value ) {
	ImGui::SameLine();
	ImGui::Text( "%lld", static_cast<int64_t>(value) );
}

template<typename T>
void drawFloat( const T& value ) {
	ImGui::SameLine();
	ImGui::Text( "%.02f", value );
}

template<typename T>
void drawInt( const char* _name, const T& value ) {
	ImGui::TextColored( ImGUIJsonColors::Key, "%s", _name );
	drawInt( value );
}

void drawString( const char* value ) {
	ImGui::SameLine();
	ImGui::Text( "%s", value );
}

void drawString( const char* _name, const char* value ) {
	drawKey( _name );
	drawString( value );
}

template<typename T, std::size_t N>
void drawArrayOfInt( const char* _name, const std::array<T, N>& array ) {
	drawKey( _name );
	if ( ImGui::TreeNode( _name )) {
		for ( auto& value : array ) {
			drawInt( value );
		}
		ImGui::TreePop();
	}
}

template<typename T>
void drawArrayOfInt( const char* _name, const std::vector<T>& array ) {
	drawKey( _name );
	if ( ImGui::TreeNode( _name )) {
		for ( auto& value : array ) {
			drawInt( value );
		}
		ImGui::TreePop();
	}
}

template<typename T>
void drawFloat( const char* _name, const T& value ) {
	ImGui::TextColored( ImGUIJsonColors::Key, "%s", _name );
	drawFloat( value );
}

template<typename T>
void drawArrayOfFloat( const char* _name, const std::vector<T>& array ) {
	//drawKey( _name );
	if ( ImGui::TreeNode( _name )) {
		for ( auto& value : array ) {
			drawFloat( value );
		}
		ImGui::TreePop();
	}
}

template<typename T, std::size_t N>
void drawArrayOfFloat( const char* _name, const std::array<T, N>& array ) {
	//drawKey( _name );
	if ( ImGui::TreeNode( _name )) {
		for ( auto& value : array ) {
			drawFloat( value );
		}
		ImGui::TreePop();
	}
}

void drawArrayOfString( const char* _name, const std::vector<std::string>& array ) {
	drawKey( _name );
	if ( ImGui::TreeNode( _name )) {
		for ( auto& value : array ) {
			drawString( value.c_str() );
		}
		ImGui::TreePop();
	}
}

void drawRect( const Rect2f& rect ) {
	drawVector( Vector2f{ rect.left(), rect.top() } );
	ImGui::SameLine( 0.0f, 0.0f );
	ImGui::TextColored( ImGUIJsonColors::SquareBracket, "%s", "," );
	drawVector( Vector2f{ rect.right(), rect.bottom() } );
}

void drawRect( const char* _name, const Rect2f& value ) {
	drawKey( _name );
	drawRect( value );
}

void drawArrayOfRect( const char* _name, const std::vector<Rect2f>& array ) {
	drawKey( _name );
	if ( ImGui::TreeNode( _name )) {
		for ( auto& value : array ) {
			drawRect( value );
		}
		ImGui::TreePop();
	}
}

template <typename T>
void drawArrayOfVector( const char* _name, const std::vector<T>& array ) {
	//drawKey( _name );
	if (ImGui::TreeNode(_name)) {
		for ( auto& value : array ) {
			drawVector( value );
		}
		ImGui::TreePop();
	}
}

template <typename T>
std::string arrayName( const char* _name, const std::vector<T>& array ) {
	std::string ret = std::string(_name);
	ret += " (";
	ret += std::to_string(array.size());
	ret += ")";
	return ret;
}

#define arrayVisitStart \
	if (ImGui::TreeNode( arrayName( _name, array).c_str() )) { \
		for ( size_t i = 0; i < array.size(); i++ ) { \
			if (ImGui::TreeNode(std::to_string(i).c_str())) { \
				array[i]

#define arrayVisitEnd \
					template visit<ImGUIJson>(); \
				ImGui::TreePop(); \
			} \
		} \
		ImGui::TreePop(); \
	}

#define arrayVisitDot arrayVisitStart . arrayVisitEnd
#define arrayVisitPtr arrayVisitStart -> arrayVisitEnd

class ImGUIJson {
public:

	template<typename T>
	void visit( const char* _name, const T& value ) {
		drawKey( _name );
		if (ImGui::TreeNode(_name)) {
			value.template visit<ImGUIJson>();
			ImGui::TreePop();
		}
	}

	template<typename T, std::size_t N>
	void visit( const char* _name, const std::array<T, N>& array ) {
		arrayVisitDot
	}

	template<typename T>
	void visit( const char* _name, const std::vector<T>& array ) {
		arrayVisitDot
	}

	template<typename T>
	void visit( const char* _name, const std::vector<std::shared_ptr<T>>& array ) {
		arrayVisitPtr
	}

	template<typename T>
	void visit( const char* _name, const std::vector<std::vector<T>>& arrayOfArray ) {
		if (ImGui::TreeNode(_name))
		{
			for ( auto& array : arrayOfArray ) {
				arrayVisitDot
			}
			ImGui::TreePop();
		}
	}

	void visit( const char* _name, const char* value ) {
		drawString( _name, value );
	}

	void visit( const char* _name, const double& value ) {
		drawFloat( _name, value );
	}

	void visit( const char* _name, const float& value ) {
		drawFloat( _name, value );
	}

	void visit( const char* _name, const bool& value ) {
		drawBool( _name, value );
	}

	void visit( const char* _name, const int32_t& value ) {
		drawInt( _name, value );
	}

	void visit( const char* _name, const uint32_t& value ) {
		drawInt( _name, value );
	}

	void visit( const char* _name, const int64_t& value ) {
		drawInt( _name, value );
	}

	void visit( const char* _name, const uint64_t& value ) {
		drawInt( _name, value );
	}

	void visit( const char* _name, const std::string& value ) {
		drawString( _name, value.c_str() );
	}

	void visit( const char* _name, const Vector2f& value ) {
		drawVector( _name, value );
	}

	void visit( const char* _name, const Vector3f& value ) {
		drawVector( _name, value );
	}

	void visit( const char* _name, const Vector4f& value ) {
		drawVector( _name, value );
	}

	void visit( const char* _name, const std::pair<int, int>& value ) {
		drawPair( _name, value );
	}

	void visit( const char* _name, const JMATH::Rect2f& rect ) {
		drawRect( _name, rect );
	}

	void visit( const char* _name, const JMATH::AABB& rect ) {
		drawKey( _name );
		drawVector( rect.minPoint() );
		drawVector( rect.maxPoint() );
	}

	void visit( const char* _name, const std::vector<Vector2f>& array ) {
		drawArrayOfVector( _name, array );
	}

	void visit( const char* _name, const std::vector<Triangle2d>& array ) {
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

	template<std::size_t N>
	void visit( const char* _name, const std::array<Vector2f, N>& array ) {
		drawKey( _name );
		if (ImGui::TreeNode(_name)) {
			for ( auto& value : array ) {
				drawVector( value );
			}
			ImGui::TreePop();
		}
	}

	void visit( const char* _name, const std::vector<Vector3f>& array ) {
		drawArrayOfVector( _name, array );
	}

	void visit( const char* _name, const std::vector<Vector4f>& array ) {
		drawArrayOfVector( _name, array );
	}

	template<std::size_t N>
	void visit( const char* _name, const std::array<int32_t, N>& array ) {
		drawArrayOfInt( _name, array );
	}

	void visit( const char* _name, const std::vector<uint32_t>& array ) {
		drawArrayOfInt( _name, array );
	}

	void visit( const char* _name, const std::vector<int32_t>& array ) {
		drawArrayOfInt( _name, array );
	}

	void visit( const char* _name, const std::vector<uint64_t>& array ) {
		drawArrayOfInt( _name, array );
	}

	void visit( const char* _name, const std::vector<int64_t>& array ) {
		drawArrayOfInt( _name, array );
	}

	void visit( const char* _name, const std::vector<float>& array ) {
		drawArrayOfFloat( _name, array );
	}

	void visit( const char* _name, const std::vector<double>& array ) {
		drawArrayOfFloat( _name, array );
	}

	void visit( const char* _name, const std::vector<std::string>& value ) {
		drawArrayOfString( _name, value );
	}

	void visit( const char* _name, const std::vector<JMATH::Rect2f>& value ) {
		drawArrayOfRect( _name, value );
	}

	void visit( const char* _name, const std::vector<std::vector<double>>& array ) {
		drawKey( _name );
		int q = 0;
		for ( auto subarray : array ) {
			drawArrayOfFloat( std::to_string(q++).c_str(), subarray );
		}
	}

	template<std::size_t N>
	void visit( const char* _name, const std::vector<std::array<double, N>>& array ) {
		drawKey( _name );
		int q = 0;
		for ( auto subarray : array ) {
			drawArrayOfFloat( std::to_string(q++).c_str(), subarray );
		}
	}

	void visit( const char* _name, const std::vector<std::vector<Vector2f>>& array ) {
		drawKey( _name );
		int q = 0;
		for ( auto subarray : array ) {
			drawArrayOfVector( std::to_string(q++).c_str(), subarray );
		}
	}

	void visit( const char* _name, const std::vector<std::vector<Vector3f>>& array ) {
		drawKey( _name );
		int q = 0;
		for ( auto subarray : array ) {
			drawArrayOfVector( std::to_string(q++).c_str(), subarray );
		}
	}

};
