//
// Created by Dado on 2018-10-29.
//

#pragma once

#include <core/math/rect2f.h>
class Scene;
class GLTF2;

void loadGeomInGui( Scene* p, std::shared_ptr<GLTF2> _newObject );
void ImGuiGeoms( Scene* p, const Rect2f& _r );


