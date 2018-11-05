//
// Created by Dado on 2018-10-29.
//

#pragma once
#include <core/math/rect2f.h>
class Scene;

void ImGuiMaterials( Scene* p, const Rect2f& _r );
void callbackMaterial( const std::string& _filename, const std::vector<char>& _data );