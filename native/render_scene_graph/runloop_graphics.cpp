//
//  RunLoop.cpp
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

#include "runloop_graphics.h"

[[nodiscard]] std::optional<InitializeWindowFlagsT> checkLayoutParam( const std::string& _param ) {
    if ( toLower(_param) == toLower("Normal"       ) ) return InitializeWindowFlags::Normal;
    if ( toLower(_param) == toLower("FullScreen"   ) ) return InitializeWindowFlags::FullScreen;
    if ( toLower(_param) == toLower("Minimize"     ) ) return InitializeWindowFlags::Minimize;
    if ( toLower(_param) == toLower("Maximize"     ) ) return InitializeWindowFlags::Maximize;
    if ( toLower(_param) == toLower("HalfSize"     ) ) return InitializeWindowFlags::HalfSize;
    if ( toLower(_param) == toLower("ThreeQuarter" ) ) return InitializeWindowFlags::ThreeQuarter;

    return std::nullopt;
}

InitializeWindowFlagsT checkLayoutArgvs( const CLIParamMap& params ) {
    auto wsize = params.getParam("defaultwindowsize");
    return wsize ? *checkLayoutParam(*wsize) : InitializeWindowFlags::Normal;
}
