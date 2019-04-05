#include <utility>

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
               std::shared_ptr<VPList> _vpl,
               ShaderMaterial _sm ) : rr(_rr), vpl(std::move( _vpl )), shaderMaterial( std::move( _sm )) {
        name = UUIDGen::make();
        shaderMaterial.activate(rr);
    };

    VPBuilder& p( std::shared_ptr<V> _ps ) { ps = _ps; return *this; }
    VPBuilder& n( const std::string& _name ) { name = _name; return *this; }
    VPBuilder& g( const uint64_t _tag) { tag = _tag; return *this; }

    UUID build() {
        rr.invalidateOnAdd();
        vpl->create( VertexProcessing::create_cpuVBIB( ps ), rr.addMaterial( shaderMaterial ), tag );
        return name;
    }

private:
    Renderer& rr;
    std::shared_ptr<VPList> vpl;
    uint64_t tag = GT_Generic;
    std::shared_ptr<V> ps;
    ShaderMaterial shaderMaterial;
    std::string name;
};
