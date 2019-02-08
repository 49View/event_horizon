//
// Created by Dado on 2019-01-23.
//

#pragma once

#include <cstdint>
#include <string>
#include <core/math/rect2f.h>

class SceneOrchestrator;

namespace BoxFlags {
    const static uint64_t None      = 0;
    const static uint64_t Resize    = 1 << 0;
    const static uint64_t Rearrange = 1 << 1;
    const static uint64_t Visible   = 1 << 2;
}

using BoxFlagsT = uint64_t;

class LayoutBoxRenderer {
public:
    explicit LayoutBoxRenderer( const std::string& name, bool _visible = true );
    void render( SceneOrchestrator* , JMATH::Rect2f&, BoxFlagsT );
    void toggleVisible();
    void setVisible( bool _bVis );
protected:
    bool startRender(JMATH::Rect2f& _r, BoxFlagsT _flags);
    void endRender(JMATH::Rect2f& _r);
    virtual void renderImpl( SceneOrchestrator* ,JMATH::Rect2f&  ) = 0;
protected:
    std::string name;
    bool bVisible = true;
    JMATH::Rect2f rect;
};

