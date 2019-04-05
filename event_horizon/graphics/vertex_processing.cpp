#include "vertex_processing.h"

#include <core/app_globals.h>
#include <core/math/poly_shapes.hpp>
#include <graphics/render_list.h>
#include <graphics/renderer.h>

void VPList::render_im() {
    mVP->renderProgram();
}

void VPList::assign( const std::string& name, const Matrix4f& data ) {
    mVP->setMaterialConstant( name, data );
}

void VPList::addToCommandBuffer( Renderer& rr, std::shared_ptr<Matrix4f> _transform,
                                 std::shared_ptr<RenderMaterial> _mat,
                                 float alpha_threashold ) const {
//    if ( RL.isUseMultiThreadRendering() ) std::lock_guard<std::mutex> lock( RL.CBVPAddMutex());
    if ( mVP->transparencyValue() > alpha_threashold ) {
        rr.CB_U().pushVP( mVP, _mat ? _mat : mVP->getMaterial(), _transform );
    }
}

//std::map<std::string, std::shared_ptr<VertexProcessing>> VPList::withNames( const std::string& _name ) {
//	std::map<std::string, std::shared_ptr<VertexProcessing>> ret;
//
//	for ( auto& i : mVPList ) {
//		if ( i.second->Name() == _name ) ret[_name] = i.second;
//	}
//	return ret;
//}

void VPList::create( std::shared_ptr<cpuVBIB> value, std::shared_ptr<RenderMaterial> _mat, const uint64_t _tag ) {
    mVP = std::make_shared<VertexProcessing>( _tag, _mat );
    mVP->create( value );
}

void VPList::setMaterial( std::shared_ptr<RenderMaterial> mp ) {
    mVP->setMaterial( mp );
}

bool VPList::hasTag( const uint64_t _tag) const {
    return checkBitWiseFlag( mVP->tag(), _tag );
}

void VPList::setMaterialWithTag( std::shared_ptr<RenderMaterial> mp, uint64_t _tag ) {
    if ( checkBitWiseFlag( mVP->tag(), _tag ) ) {
        mVP->setMaterial( mp );
    }
}

void VPList::setMaterialColorWithTag( const Color4f& _color, uint64_t _tag ) {
    if ( checkBitWiseFlag( mVP->tag(), _tag ) ) {
        mVP->setMaterialConstant(UniformNames::diffuseColor, _color.xyz() );
    }
}


void VPList::setMaterialColorWithUUID( const Color4f& _color, const UUID& _uuid, Color4f& _oldColor ) {
//    ### REF put UUID in place for VPList
//    if ( mVP->Name() == _uuid ) {
//        Color3f oldC = Vector3f::ONE;
//        float oldAlpha = 1.0f;
//        mVP->getMaterialConstant(UniformNames::diffuseColor, oldC );
//        mVP->getMaterialConstant(UniformNames::alpha, oldAlpha );
//        mVP->setMaterialConstant(UniformNames::diffuseColor, _color.xyz() );
//        mVP->setMaterialConstant(UniformNames::alpha, _color.w() );
//        _oldColor = Vector4f{ oldC, oldAlpha};
//    }
}
