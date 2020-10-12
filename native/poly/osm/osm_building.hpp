//
// Created by dado on 12/10/2020.
//

#pragma once

struct Topology;
struct OSMMesh;

class Vector3f;

[[maybe_unused]] [[maybe_unused]] void
osmCreateBuilding( Topology& mesh, const OSMMesh& group, const Vector3f& tilePosDelta, float globalOSMScale );


