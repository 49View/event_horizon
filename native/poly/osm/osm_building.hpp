//
// Created by dado on 12/10/2020.
//

#pragma once

#include <vector>

struct PolyStruct;
struct OSMMesh;

class Vector3f;

std::vector<PolyStruct> osmCreateBuilding( const OSMMesh& group, const Vector3f& tilePosDelta, float globalOSMScale );
