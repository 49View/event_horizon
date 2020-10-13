//
// Created by dado on 13/10/2020.
//

#pragma once

#include <memory>

struct VData;
struct Vector3f;

void osmCreateTree( const std::shared_ptr<VData>& _ret, const Vector3f& treePos, float globalOSMScale );


