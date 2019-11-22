//
// Created by Dado on 12/12/2017.
//

#include "../gl_util.h"

void setWireFrame( bool enabled ) {
    GLCALL( glPolygonMode( GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL ) );
}

void enableMultiSample( bool enabled ) {
    if ( enabled ) {
        GLCALL( glEnable( GL_MULTISAMPLE ) );
    } else {
        GLCALL( glDisable( GL_MULTISAMPLE ) );
    }
}
