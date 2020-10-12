//
// Created by dado on 12/10/2020.
//

#pragma once

#include <vector>

struct PolyStruct;
struct OSMMesh;

void osmCreateBarrier( std::vector<PolyStruct>& barriers, const OSMMesh& group, float globalOSMScale );