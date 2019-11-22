//
// Created by Dado on 12/12/2017.
//

#include "../../texture.h"
#include "../gl_util.h"

void texImage2DMultisample( [[maybe_unused]] GLsizei samples, [[maybe_unused]] GLint glInternalFormat,
                            [[maybe_unused]] GLsizei w, [[maybe_unused]] GLsizei h) {
    ASSERTV(0, "Cannot use yet multisample texture on OpenGL ES");
}

