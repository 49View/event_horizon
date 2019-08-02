//
// Created by Dado on 2019-08-02.
//

#pragma once

class AppData {
public:
    AppData() = default;
JSONSERIAL( AppData, geomres, colorres, materialres, profileres,
            imageres, fontres );

    [[nodiscard]] size_t firstTierResourceCount() const {
        return colorres.size() +
               materialres.size() +
               profileres.size() +
               imageres.size() +
               fontres.size();
    };

    [[nodiscard]] size_t secondTierResourceCount() const {
        return geomres.size();
    }

    [[nodiscard]] const std::vector<std::string>& Geoms()          { return geomres; }
    [[nodiscard]] const std::vector<std::string>& MaterialColors() { return colorres; }
    [[nodiscard]] const std::vector<std::string>& Materials()      { return materialres; }
    [[nodiscard]] const std::vector<std::string>& Profiles()       { return profileres; }
    [[nodiscard]] const std::vector<std::string>& RawImages()      { return imageres; }
    [[nodiscard]] const std::vector<std::string>& Fonts()          { return fontres; }

    void addGeom         ( CResourceRef _value ) { geomres.emplace_back(_value);  }
    void addMaterialColor( CResourceRef _value ) { colorres.emplace_back(_value);  }
    void addMaterial     ( CResourceRef _value ) { materialres.emplace_back(_value);  }
    void addProfile      ( CResourceRef _value ) { profileres.emplace_back(_value);  }
    void addRawImage     ( CResourceRef _value ) { imageres.emplace_back(_value);  }
    void addFont         ( CResourceRef _value ) { fontres.emplace_back(_value);  }

protected:
    std::vector<std::string> geomres;
    std::vector<std::string> colorres;
    std::vector<std::string> materialres;
    std::vector<std::string> profileres;
    std::vector<std::string> imageres;
    std::vector<std::string> fontres;
};

