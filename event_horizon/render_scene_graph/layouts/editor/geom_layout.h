//
// Created by Dado on 2018-10-29.
//

#pragma once

#include <core/math/rect2f.h>
class Scene;

void callbackGeom( const std::string& _filename, const std::vector<char>& _data );
void callbackGeomGLTF( const std::string& _filename );

void ImGuiGeoms( Scene* p, const Rect2f& _r );


