#include <utility>

#include "vertex_processing.h"

#include <core/math/matrix4f.h>
#include <core/descriptors/uniform_names.h>

#include <core/app_globals.h>
#include <core/math/poly_shapes.hpp>
#include <graphics/render_list.h>
#include <graphics/renderer.h>

#ifdef _OPENGL
#include <graphics/opengl/gpuv_data.h>
#endif

VPList::VPList( std::shared_ptr<GPUVData> _gpuData,
                std::shared_ptr<RenderMaterial> _mat,
                const std::shared_ptr<Matrix4f>& _transform,
                const uint64_t _tag ) {
    gpuData = std::move(_gpuData);
    mTransform = _transform;
    if ( !_transform ) {
        mTransform = std::make_shared<Matrix4f>(Matrix4f::IDENTITY);
    }
    material = _mat;
    mTag = _tag;
}

void VPList::setMaterial( std::shared_ptr<RenderMaterial> mp ) {
    material = mp;
}

bool VPList::hasTag( const uint64_t _tag) const {
    return checkBitWiseFlag( tag(), _tag );
}

void VPList::setMaterialWithTag( std::shared_ptr<RenderMaterial> mp, uint64_t _tag ) {
    if ( checkBitWiseFlag( tag(), _tag ) ) {
        setMaterial( mp );
    }
}

void VPList::setMaterialColorWithTag( const Color4f& _color, uint64_t _tag ) {
    if ( checkBitWiseFlag( tag(), _tag ) ) {
        setMaterialConstant(UniformNames::diffuseColor, _color.xyz() );
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

void VPList::draw() {
    if ( gpuData->isEmpty()) return;
    setMaterialGlobalConstant( UniformNames::modelMatrix, *getTransform().get() );
    gpuData->programStart( material );
    gpuData->draw();
}

void VPList::drawWith( std::shared_ptr<RenderMaterial> _material ) {
    if ( gpuData->isEmpty()) return;
    setMaterialGlobalConstant( UniformNames::modelMatrix, *getTransform() );
    gpuData->programStart( std::move( _material ));
    gpuData->draw();
}

void VPList::setMaterialConstantAlpha( float alpha ) {
    material->setConstant( UniformNames::alpha, alpha );
}

void VPList::setMaterialConstantOpacity( float alpha ) {
    material->setConstant( UniformNames::opacity, alpha );
}

std::shared_ptr<Matrix4f> VPList::getTransform() const {
    return mTransform;
}

void VPList::setTransform( std::shared_ptr<Matrix4f> lTransform ) {
    if ( lTransform ) VPList::mTransform = lTransform;
}
