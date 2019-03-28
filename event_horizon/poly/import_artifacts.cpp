//
// Created by Dado on 2019-02-20.
//

#include "import_artifacts.hpp"

const GeomAssetSP& ImportGeomArtifacts::getScene() const {
    return scene;
}

void ImportGeomArtifacts::setScene( const GeomAssetSP& scene ) {
    ImportGeomArtifacts::scene = scene;
}

const std::vector<std::shared_ptr<Material>>& ImportGeomArtifacts::getMaterials() const {
    return materials;
}

void ImportGeomArtifacts::setMaterials( const std::vector<std::shared_ptr<Material>>& materials ) {
    ImportGeomArtifacts::materials = materials;
}

void ImportGeomArtifacts::addMaterial( std::shared_ptr<Material> _mat ) {
//    std::string name;
//    std::unordered_map<std::string, RawImage> mb;
//    HeterogeneousMap values;

    materials.emplace_back(_mat);
}
