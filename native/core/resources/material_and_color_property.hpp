//
// Created by dado on 20/06/2020.
//

#pragma once

#include <core/serialization.hpp>

JSONDATA(MaterialAndColorProperty, materialName, materialHash, colorName, colorHash, color)
    MaterialAndColorProperty( const std::string& materialHash, const C4f& color ) : materialHash(materialHash),
                                                                                    color(color) {
        materialName = materialHash;
    }
    MaterialAndColorProperty( const char *materialHash ) : materialHash(materialHash) {
        materialName = materialHash;
    }
    std::string materialName{};
    std::string materialHash{};
    std::string colorName{};
    std::string colorHash{};
    C4f color{ C4f::WHITE };
};

