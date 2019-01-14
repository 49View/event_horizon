#pragma once

#ifdef _OPENGL

#include "opengl/vertex_processing_opengl.h"

#endif

#include "graphic_constants.h"
#include "core/formatting_utils.h"
#include "core/uuid.hpp"

class Renderer;

class VPList {
public:
    VPList( std::shared_ptr<Matrix4f> _transform = nullptr ) {
        if ( _transform == nullptr ) {
            mTransform = std::make_shared<Matrix4f>(Matrix4f::IDENTITY);
        } else {
            mTransform = _transform;
        }
    }

    void render_im();
    void assign( const std::string& name, const Matrix4f& data );

    void create( std::shared_ptr<cpuVBIB> value, const uint64_t _tag );

    void addToCommandBuffer( Renderer& rr,
                             std::shared_ptr<Matrix4f> _transform = nullptr,
                             std::shared_ptr<RenderMaterial> _mat = nullptr,
                             float alpha_threashold = 0.0f ) const;
    //	std::map<std::string, std::shared_ptr<VertexProcessing>> withNames( const std::string& _name );

    template<typename T>
    inline void setMaterialConstant( const std::string& name, const T& val ) {
        mVP->setMaterialConstant( name, val );
    }

    template<typename T>
    inline void setMaterialGLobalConstant( const std::string& name, const T& val ) {
        mVP->setMaterialGlobalConstant( name, val );
    }

    template<typename T>
    inline void setMaterialBufferConstant( const std::string& ubo_name, const std::string& name, const T& val ) {
        mVP->setMaterialBufferConstant( ubo_name, name, val );
    }

    template<typename T>
    inline void setMaterialConstantOn( const std::string& vp_name, const std::string& name, const T& val ) {
        if ( mVP->Name() == vp_name ) {
            mVP->setMaterialConstant( name, val );
        }
    }

    void setMaterial( std::shared_ptr<RenderMaterial> mp );
    void setMaterialWithTag( std::shared_ptr<RenderMaterial> mp, uint64_t _tag );
    void setMaterialColorWithTag( const Color4f& _color, uint64_t _tag );
    void setMaterialColorWithUUID( const Color4f& _color, const UUID& _uuid, Color4f& _oldColor );

    std::shared_ptr<Matrix4f> getTransform() const {
        return mTransform;
    }

    void setTransform( std::shared_ptr<Matrix4f> lTransform ) {
        if ( lTransform ) VPList::mTransform = lTransform;
    }

    bool hasTag( const uint64_t _tag) const;

private:
    std::shared_ptr<VertexProcessing> mVP;
    std::shared_ptr<Matrix4f> mTransform;
};
