//
// Created by dado on 12/10/2020.
//

#pragma once

#include <memory>

struct VData;
struct OSMMesh;

void osmCreateBarrier( const std::shared_ptr<VData>& _ret, const OSMMesh& group, float globalOSMScale );