//
// Created by Dado on 2019-02-20.
//

#pragma once

#include <memory>
#include <vector>
#include <poly/poly.hpp>

class Material;
using ImportMaterialMap = std::vector<std::shared_ptr<Material>>;

class ImportGeomArtifacts {
public:
    const GeomAssetSP& getScene() const;
    void setScene( const GeomAssetSP& scene );

    const ImportMaterialMap& getMaterials() const;
    void setMaterials( const ImportMaterialMap& materials );
    void addMaterial( std::shared_ptr<Material> _mat );
private:
    GeomAssetSP scene;
    ImportMaterialMap materials;
};
