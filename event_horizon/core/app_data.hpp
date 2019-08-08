//
// Created by Dado on 2019-08-02.
//

#pragma once

#include <core/name_policy.hpp>

JSONDATA( AppRenderSettings, shadowOverBurnCofficient, indoorSceneCoeff, shadowZFightCofficient )
    float shadowOverBurnCofficient = 2.0f;
    float indoorSceneCoeff = 1.0f;
    float shadowZFightCofficient = 0.002f;
};

class AppData : public Keyable<> {
public:
    AppData() = default;
JSONSERIAL( AppData, renderSettings, mKey, geoms, colors, materials, profiles,
            images, fonts );

    [[nodiscard]] size_t firstTierResourceCount() const {
        return colors.size() +
               materials.size() +
               profiles.size() +
               images.size() +
               fonts.size();
    };

    [[nodiscard]] size_t secondTierResourceCount() const {
        return geoms.size();
    }

    [[nodiscard]] size_t totalResourceCount() const {
        return firstTierResourceCount() + secondTierResourceCount();
    }

    [[nodiscard]] const std::vector<std::string>& Geoms()          { return geoms; }
    [[nodiscard]] const std::vector<std::string>& MaterialColors() { return colors; }
    [[nodiscard]] const std::vector<std::string>& Materials()      { return materials; }
    [[nodiscard]] const std::vector<std::string>& Profiles()       { return profiles; }
    [[nodiscard]] const std::vector<std::string>& RawImages()      { return images; }
    [[nodiscard]] const std::vector<std::string>& Fonts()          { return fonts; }

    void addGeom         ( CResourceRef _value ) { geoms.emplace_back( _value);  }
    void addMaterialColor( CResourceRef _value ) { colors.emplace_back( _value);  }
    void addMaterial     ( CResourceRef _value ) { materials.emplace_back( _value);  }
    void addProfile      ( CResourceRef _value ) { profiles.emplace_back( _value);  }
    void addRawImage     ( CResourceRef _value ) { images.emplace_back( _value);  }
    void addFont         ( CResourceRef _value ) { fonts.emplace_back( _value);  }

protected:
    AppRenderSettings        renderSettings;
    std::vector<std::string> geoms;
    std::vector<std::string> colors;
    std::vector<std::string> materials;
    std::vector<std::string> profiles;
    std::vector<std::string> images;
    std::vector<std::string> fonts;
};

