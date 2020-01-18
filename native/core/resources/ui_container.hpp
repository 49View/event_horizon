//
// Created by Dado on 2019-09-09.
//
#pragma once

#include <core/names.hpp>
#include <core/serialization.hpp>

JSONDATA( UIElementContainerLogical, id, type, text, icon, tapType, font, size, color, func, entries )

    std::string id;
    std::string type;
    std::string text;
    std::string icon;
    std::string tapType;
    std::string font;
    std::string size;
    std::string color;
    std::vector<std::string> func;
    std::vector<UIElementContainerLogical> entries;

    void fixUpDefaults() {
        if ( font.empty()) font = S::DEFAULT_FONT;
        if ( size.empty()) size = "normal";
        if ( color.empty()) color = "#FFF";
    };
};

JSONDATA( UIContainer, type, entries )

    std::string type;
    std::vector<UIElementContainerLogical> entries;

    static UIContainer placeHolder() {
        UIContainer ret;
        ret.type = "UIContainer2d";
        UIElementContainerLogical le;
        le.type = "Title";
        le.text = "New Title";
        ret.entries.emplace_back( le );
        return ret;
    }
};

using UIContainerSP = std::shared_ptr<UIContainer>;