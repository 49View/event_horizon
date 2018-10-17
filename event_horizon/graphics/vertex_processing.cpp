#include "vertex_processing.h"

#include "core/app_globals.h"
#include "core/math/poly_shapes.hpp"
#include "TTF.h"
#include "program_list.h"
#include "renderer.h"

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
    if ( mVP->hasData()) {
        if ( mVP->transparencyValue() > alpha_threashold ) {
            rr.CB_U().pushVP( mVP, _mat ? _mat : mVP->getMaterial(), _transform );
        }
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

void VPList::create( std::shared_ptr<cpuVBIB> value, const uint64_t _tag ) {
    mVP = std::make_shared<VertexProcessing>( value->name, _tag, value->material );
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

