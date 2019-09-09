//
// Created by Dado on 2019-09-09.
//
#pragma once

#include <core/names.hpp>
#include <core/serialization.hpp>

JSONDATA( UIElementContainerLogical, id, type, text, icon, tapType, func, entries )
    std::string id;
    std::string type;
    std::string text;
    std::string icon;
    std::string tapType;
    std::string font = S::DEFAULT_FONT;
    std::string size = "normal";
    std::string color = "#FFF";
    std::vector<std::string> func;
    std::vector<UIElementContainerLogical> entries;
};

JSONDATA( UIContainer, type, entries )
    std::string type;
    std::vector<UIElementContainerLogical> entries;
};

using UIContainerSP = std::shared_ptr<UIContainer>;