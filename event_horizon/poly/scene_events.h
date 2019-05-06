//
// Created by Dado on 2018-11-05.
//

#pragma once

#include <string>
#include <vector>
#include <core/htypes_shared.hpp>

using entityDaemonCallbackFunction = void( const std::string&, const SerializableContainer& );

void allCallbacksEntitySetup();
void allConversionsDragAndDropCallback( const std::string& _path );

