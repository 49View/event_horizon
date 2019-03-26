//
// Created by Dado on 2019-01-31.
//

#include <graphics/renderer.h>
#include <graphics/vertex_processing.h>
#include <core/descriptors/uniform_names.h>

template <typename V>
class VPBuilder {
public:
    VPBuilder( Renderer& _rr,
               std::shared_ptr<VPList> _vpl,
               const std::string& _shader,
               std::shared_ptr<HeterogeneousMap> _map = nullptr) : rr(_rr), vpl(std::move( _vpl )), shaderName(_shader) {
        ASSERT( rr.P( _shader ) != nullptr );
        name = UUIDGen::make();
//      ### MAT Double check how we inject default unassigned values, it should never be the case anyway!!
        values = _map;
        if ( !values ) {
            values = std::make_shared<HeterogeneousMap>();
            values->inject( *rr.P( _shader )->getDefaultUniforms().get() );
        }
//        material->resolveDynamicConstants();
    };

    VPBuilder& c( const Color4f& _matColor ) {
        values->assign( UniformNames::opacity, _matColor.w() );
        values->assign( UniformNames::diffuseColor, _matColor.xyz() );
        return *this;
    }
    VPBuilder& p( std::shared_ptr<V> _ps ) { ps = _ps; return *this; }
    VPBuilder& n( const std::string& _name ) { name = _name; return *this; }
    VPBuilder& g( const uint64_t _tag) { tag = _tag; return *this; }

    UUID build() {
        rr.invalidateOnAdd();
        vpl->create( VertexProcessing::create_cpuVBIB( ps, rr.addMaterial( shaderName, values ), name ), tag );
        return name;
    }

private:
    void init( const std::string& _shader, const std::string& _name ) {
    }

private:
    Renderer& rr;
    std::shared_ptr<VPList> vpl;
    uint64_t tag = GT_Generic;
    std::shared_ptr<V> ps;
    std::shared_ptr<HeterogeneousMap> values;
    std::string name;
    std::string shaderName;
};
