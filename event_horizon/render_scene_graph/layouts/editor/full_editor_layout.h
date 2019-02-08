//
// Created by Dado on 2018-10-16.
//

#pragma once

#include <memory>

class SceneLayout;
class SceneOrchestrator;

std::shared_ptr<SceneLayout> fullEditor();
void allConversionsdragAndDropCallback( SceneOrchestrator* p, const std::string& _path );
