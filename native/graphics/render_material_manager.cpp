//
// Created by Dado on 2019-04-07.
//

#include "render_material_manager.hpp"
#include <core/heterogeneous_map.hpp>

#include <graphics/renderer.h>
#include <graphics/shader_material.hpp>
#include <graphics/render_material.hpp>

std::shared_ptr<RenderMaterial> RenderMaterialManager::addRenderMaterial( const std::string& _type ) {
    return addRenderMaterial( _type, nullptr, { _type } );
}

std::shared_ptr<RenderMaterial> RenderMaterialManager::addRenderMaterial(
        const std::string& _type,
        const StringUniqueCollection& _names) {
    return addRenderMaterial( _type, nullptr, _names );
}

std::shared_ptr<RenderMaterial> RenderMaterialManager::addRenderMaterial(
        const std::string& _type,
        std::shared_ptr<HeterogeneousMap> _values,
        const StringUniqueCollection& _names) {

    if ( auto mat = find(_names); mat ) return mat;

    ShaderMaterial shaderMaterial{ _type, _values };
    shaderMaterial.activate(rr);
    auto rmaterial = std::make_shared<RenderMaterial>( shaderMaterial.P(), shaderMaterial.Values(), rr );

    add( _names, rmaterial );

    return rmaterial;
}

RenderMaterialManager::RenderMaterialManager( Renderer& rr ) : rr( rr ) {

}
