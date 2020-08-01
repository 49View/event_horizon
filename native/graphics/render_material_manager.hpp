//
// Created by Dado on 2019-04-07.
//

#pragma once

#include <core/hash_shared_map.hpp>
#include <core/htypes_shared.hpp>

class Renderer;
class RenderMaterial;
class HeterogeneousMap;

class RenderMaterialManager : public HashSharedMap<RenderMaterial> {
public:
    explicit RenderMaterialManager( Renderer& rr );
    std::shared_ptr<RenderMaterial> addRenderMaterial( const std::string& _type,
                                                       std::shared_ptr<HeterogeneousMap> _values,
                                                       const StringUniqueCollection& _names );
    std::shared_ptr<RenderMaterial> upsertRenderMaterial( const std::string& _type,
                                                       std::shared_ptr<HeterogeneousMap> _values,
                                                       const StringUniqueCollection& _names );
    std::shared_ptr<RenderMaterial> addRenderMaterial( const std::string& _type );

private:
    Renderer& rr;
};



