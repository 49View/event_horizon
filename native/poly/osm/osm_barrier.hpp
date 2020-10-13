//
// Created by dado on 12/10/2020.
//

#pragma once

#include <vector>

struct PolyStruct;
struct OSMMesh;

std::vector<PolyStruct> osmCreateBarrier( const OSMMesh& group, float globalOSMScale );