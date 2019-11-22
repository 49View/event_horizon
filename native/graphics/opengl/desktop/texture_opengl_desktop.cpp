//
// Created by Dado on 12/12/2017.
//

#include "../../texture.h"
#include "../gl_util.h"

void texImage2DMultisample( GLsizei samples, GLint glInternalFormat, GLsizei w, GLsizei h) {
    GLCALL( glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, samples, glInternalFormat, w, h, false ));
}

