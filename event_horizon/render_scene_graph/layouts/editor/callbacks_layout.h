//
// Created by Dado on 2018-11-05.
//

#pragma once

#include <string>
#include <vector>
#include <core/htypes_shared.hpp>

class SceneOrchestrator;

using entityDaemonCallbackFunction = void( const std::string&, const SerializableContainer& );

void allCallbacksEntitySetup();
void allConversionsDragAndDropCallback( SceneOrchestrator* p, const std::string& _path );

