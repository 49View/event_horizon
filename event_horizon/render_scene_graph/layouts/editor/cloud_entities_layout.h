//
// Created by Dado on 2018-10-29.
//

#pragma once

#include <string>
#include <core/math/rect2f.h>
class SceneOrchestrator;

void ImGuiCloudEntities( SceneOrchestrator* p, Rect2f& _r, const std::string _title, const std::string& _entType, const uint64_t _version );

