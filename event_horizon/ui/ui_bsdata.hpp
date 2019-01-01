#pragma once

#include "core/serialization.hpp"

JSONDATA_R ( StyleSheetElement, layout, style, control, text,	sizeType, size,	color, flags, margins, padding )
	std::string layout;
	std::string style;
	std::string control;
	std::string text;
	std::string sizeType;
	Vector2f    size = Vector2f{ -1.0f };
	Color4f     color = Color4f{-1.0f};;
	std::vector<std::string> flags;
	Vector4f    margins = Vector4f{-1.0f};
	Vector4f    padding = Vector4f{-1.0f};
};

JSONDATA ( LayoutElement, control, position, sizeType, size, color, backGroundColor, name, text, title, font, callback,
		   flags, margins, padding )
	std::string control;
	std::string	position;
    std::string sizeType;
	Vector2f	size = Vector2f::ZERO;
	Color4f     color = Color4f{ 255.0f, 255.0f, 255.0f, 1.0f };
	Color4f     backGroundColor = Color4f::HUGE_VALUE_NEG;
    std::string name;
	std::string text;
	std::string title;
	std::string font;
    std::string callback;
	std::vector<std::string> flags;
	Vector4f	margins = Vector4f{ -1.0f };
	Vector4f	padding = Vector4f{ -1.0f };
};

JSONDATA_R ( Layout, name, size, margins, padding, elements )
	std::string name;
	Vector2f	size = Vector2f::ZERO;
    Vector4f	margins = Vector4f{ 0.0f };
    Vector4f	padding = Vector4f{ 0.0f };
	std::vector<LayoutElement> elements;
};

JSONDATA( LayoutArrangement, layout, is_array, is_dynamic, position, margins, style )
	Layout      layout;
	bool        is_array = false;
	bool        is_dynamic = false;
	std::string position;
	Vector4f	margins = Vector4f{ -1.0f };
	std::string style;
};

JSONDATA_R( UIView, name, rootPosition, customRootPosition, margins, arrangements )
	std::string name;
	std::string rootPosition;
	Vector2f	customRootPosition = Vector2f::Y_AXIS;
	Vector4f	margins = Vector4f{ 0.0f };
	std::vector<LayoutArrangement> arrangements;

	static uint64_t Version() { return 1000; }
};
