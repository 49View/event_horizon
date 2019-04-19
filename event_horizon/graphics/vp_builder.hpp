//
// Created by Dado on 2019-01-31.
//

#include <graphics/renderer.h>
#include <graphics/vertex_processing.h>
#include <graphics/shader_material.hpp>
#include <core/descriptors/uniform_names.h>

template <typename V>
class VPBuilder {
public:
    VPBuilder( Renderer& _rr,
               ShaderMaterial _sm ) : rr(_rr), shaderMaterial( std::move( _sm )) {
        name = UUIDGen::make();
    };

    VPBuilder( Renderer& _rr,
               const ResourceRef& _matRef,
               const ResourceRef& _vdataRef ) : rr(_rr) {
        name = UUIDGen::make();
        renderMaterialSP = rr.getMaterial( _matRef );
        gpuDataSP = rr.getGPUVData( _vdataRef );
        bStraightRef = true;
    };

    VPBuilder& p( std::shared_ptr<V> _ps ) { ps = _ps; return *this; }
    VPBuilder& n( const std::string& _name ) { name = _name; return *this; }
    VPBuilder& g( const uint64_t _tag) { tag = _tag; return *this; }

    auto build() {
        if ( bStraightRef ) {
            return std::make_shared<VPList>(
                    gpuDataSP,
                    renderMaterialSP,
                    nullptr,
                    tag );
        }
        return std::make_shared<VPList>(
               rr.addVDataResource( cpuVBIB{ ps }, name ),
               rr.addMaterialResource( shaderMaterial, name ),
               nullptr,
               tag );
    }

private:
    Renderer& rr;
    uint64_t tag = GT_Generic;
    std::shared_ptr<V> ps;
    ShaderMaterial shaderMaterial;

    std::shared_ptr<GPUVData> gpuDataSP;
    std::shared_ptr<RenderMaterial> renderMaterialSP;
    std::string name;
    bool bStraightRef = false;
};
