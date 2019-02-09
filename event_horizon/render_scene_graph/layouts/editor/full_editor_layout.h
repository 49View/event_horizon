//
// Created by Dado on 2018-10-16.
//

#pragma once

#include <render_scene_graph/scene_state_machine.h>

class FullEditor : public SceneStateMachine {
public:
    using SceneStateMachine::SceneStateMachine;
    virtual ~FullEditor() = default;

    void activateImpl() override;
    void run() override;
};
