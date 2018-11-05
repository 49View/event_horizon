//
// Created by Dado on 2018-11-05.
//

#pragma once

#include <string>
class Scene;

using entityDaemonCallbackFunction = void( const std::string&, const std::vector<char>& );

void allCallbacksEntitySetup();
void allConversionsDragAndDropCallback( Scene* p, const std::string& _path );

