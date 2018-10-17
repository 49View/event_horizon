//
// Created by Dado on 19/08/2018.
//

#pragma once

#include <string>
#include <cstdint>
#include <core/callback_dependency.h>

namespace SubstanceDriver {

    void elaborateSbsarLayer( const std::string& mainFileName,
                              const std::string& layerName, const uint32_t size, DependencyMaker& _md );

    void elaborateFromTextureSet( const std::string& mainFileName, DependencyMaker& _md,
            const std::string& _forcedExt = ".png" );

    void elaborateSbsar( const std::string& mainFileName, DependencyMaker& _md );
};



