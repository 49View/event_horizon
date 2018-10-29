//
// Created by Dado on 2018-10-16.
//

#pragma once

#include <memory>

class SceneLayout;
class Scene;

std::shared_ptr<SceneLayout> fullEditor();
void allConversionsdragAndDropCallback( Scene* p, const std::string& _path );
