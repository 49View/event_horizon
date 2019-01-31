//
// Created by Dado on 2019-01-31.
//

#include <graphics/renderer.h>

template <typename V>
class VPBuilder {
public:
    explicit VPBuilder( Renderer& _rr, std::shared_ptr<VPList> _vpl, std::string _shader ) : rr(_rr), vpl(
            std::move( _vpl )) {
        name = UUIDGen::make();
        ASSERT( rr.P( _shader ) != nullptr );
        material = std::make_shared<Material>( rr.P( _shader )->getDefaultUniforms() );
    };

    VPBuilder& c( const Color4f& _matColor ) {
        material->assign( UniformNames::opacity, _matColor.w() );
        material->assign( UniformNames::diffuseColor, _matColor.xyz() );
        return *this;
    }
    VPBuilder& p( std::shared_ptr<V> _ps ) { ps = _ps; return *this; }
    VPBuilder& m( std::shared_ptr<Material> _mat ) {
        material->inject( *_mat.get() );
        return *this;
    }
    VPBuilder& n( const std::string& _name ) { name = _name; return *this; }
    VPBuilder& t( const std::string& _tex ) {
        material->assign( UniformNames::colorTexture, _tex );
        return *this;
    }
    VPBuilder& g( const uint64_t _tag) { tag = _tag; return *this; }

    UUID build() {
        rr.invalidateOnAdd();

        vpl->create( VertexProcessing::create_cpuVBIB( ps, rr.addMaterial( material ), name ), tag );

        return name;
    }

private:
    Renderer& rr;
    std::shared_ptr<VPList> vpl;
    uint64_t tag = GT_Generic;
    std::shared_ptr<V> ps;
    std::shared_ptr<Material> material;
    std::string name;
};
