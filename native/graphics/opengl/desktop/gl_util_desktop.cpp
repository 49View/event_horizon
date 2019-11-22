//
// Created by Dado on 12/12/2017.
//

#include "../gl_util.h"

GLenum primitiveToGl( Primitive primitive ) {
    switch ( primitive ) {
        case PRIMITIVE_TRIANGLES:
            return GL_TRIANGLES;
        case PRIMITIVE_TRIANGLE_STRIP:
            return GL_TRIANGLE_STRIP;
        case PRIMITIVE_TRIANGLE_FAN:
            return GL_TRIANGLE_FAN;
        case PRIMITIVE_PATCHES:
            return GL_PATCHES;
    }
    ASSERT( false );
    return 0;
}

GLenum imageTargetToGl( TextureTargetMode mode, bool multiSampled ) {

    if ( multiSampled ) return GL_TEXTURE_2D_MULTISAMPLE;

    switch ( mode ) {
        case TEXTURE_2D:
            return GL_TEXTURE_2D;
        case TEXTURE_3D:
            return GL_TEXTURE_3D;
        case TEXTURE_CUBE_MAP:
            return GL_TEXTURE_CUBE_MAP;
    }
    ASSERT( false );
    return 0;
}
