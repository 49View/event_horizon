//
// Created by Dado on 2018-10-16.
//

#pragma once

#include <render_scene_graph/scene_state_machine.h>

class FullEditor : public SceneStateMachineBackEnd {
public:
    using SceneStateMachineBackEnd::SceneStateMachineBackEnd;
    virtual ~FullEditor() = default;

    void init() override;
    void run() override;
};
