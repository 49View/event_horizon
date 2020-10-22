//
// Created by dado on 13/10/2020.
//

#pragma once

#include <vector>

struct PolyStruct;
class Vector3f;

std::vector<PolyStruct> osmCreateTree( const Vector3f& treePos, float globalOSMScale );


