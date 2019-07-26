//
// Created by Dado on 2018-11-05.
//

#pragma once

#include <string>
#include <vector>
#include <core/htypes_shared.hpp>

namespace SceneEvents {
    static const std::string LoadGeomAndReset      = "LoadGeomAndReset";
    static const std::string ReplaceMaterialOnCurrentObject = "ReplaceMaterialOnCurrentObject";
    static const std::string ChangeMaterialProperty = "ChangeMaterialProperty";
};

using entityDaemonCallbackFunction = void( const std::string&, const SerializableContainer& );

void allCallbacksEntitySetup();
void allConversionsDragAndDropCallback( std::vector<std::string>& _paths );

