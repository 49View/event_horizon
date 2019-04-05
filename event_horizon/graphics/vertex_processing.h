#pragma once

#include "graphic_constants.h"
#include "core/formatting_utils.h"
#include "core/uuid.hpp"

class Renderer;

#ifdef _OPENGL
#include "opengl/vertex_processing_opengl.h"
#endif

class VPList {
public:
    VPList( std::shared_ptr<cpuVBIB> value,
            std::shared_ptr<RenderMaterial> _mat,
            std::shared_ptr<Matrix4f> _transform,
            uint64_t _tag );

    inline std::shared_ptr<RenderMaterial> getMaterial() const {
        return material;
    }

    inline void setMaterial( std::shared_ptr<RenderMaterial> mp );

    template<typename T>
    inline void setMaterialConstant( const std::string& name, const T& val ) {
        material->setConstant( name, val );
    }

    template<typename T>
    inline void setMaterialGlobalConstant( const std::string& name, const T& val ) {
        material->setGlobalConstant( name, val );
    }

    template<typename T>
    inline void setMaterialBufferConstant( const std::string& ubo_name, const std::string& name, const T& val ) {
        material->setBufferConstant( ubo_name, name, val );
    }

    template<typename T>
    inline T getMaterialConstant( const std::string& name, T& value ) {
        material->Uniforms()->Values()->get( name, value );
        return value;
    }

    inline float transparencyValue() const {
        return material->TransparencyValue();
    }

    inline void setMaterialConstantAlpha( float alpha ) {
        material->setConstant( UniformNames::alpha, alpha );
    }

    inline void setMaterialConstantOpacity( float alpha ) {
        material->setConstant( UniformNames::opacity, alpha );
    }

    void setMaterialWithTag( std::shared_ptr<RenderMaterial> mp, uint64_t _tag );
    void setMaterialColorWithTag( const Color4f& _color, uint64_t _tag );
    void setMaterialColorWithUUID( const Color4f& _color, const UUID& _uuid, Color4f& _oldColor );

    std::shared_ptr<Matrix4f> getTransform() const {
        return mTransform;
    }

    void setTransform( std::shared_ptr<Matrix4f> lTransform ) {
        if ( lTransform ) VPList::mTransform = lTransform;
    }

    bool hasTag( uint64_t _tag) const;
    uint64_t tag() const { return mTag; }
    void tag( const uint64_t tag ) { mTag = tag; }

    void draw();
    void drawWith( std::shared_ptr<RenderMaterial> _material );

private:
    GPUVData gpuData;
    std::shared_ptr<RenderMaterial> material;
    uint64_t mTag = GT_Generic;
    std::shared_ptr<Matrix4f> mTransform;
};
