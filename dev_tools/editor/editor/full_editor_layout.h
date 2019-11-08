//
// Created by Dado on 2018-10-16.
//

#pragma once

#include <core/state_machine_helper.hpp>
#include <core/camera.h>
#include <render_scene_graph/runloop_graphics.h>
#include <render_scene_graph/scene_loader.hpp>

// Events
struct OnActivate {};

// Actions
struct Activate {
    void operator()( SceneGraph& _sg, RenderOrchestrator& rsg ) noexcept {
    }
};

// State machine Front End
struct EditorStateMachineSML { auto operator()() const noexcept { return make_transition_table(
                *state<class Initial>      + event<OnActivate>                                / Activate{}             = state<class Editor>
        ); } };

using FrontEnd = sm<EditorStateMachineSML>;

// Back End
class EditorBackEnd : public RunLoopBackEndBase, public LoginActivation<LoginFieldsPrecached>, public ScenePreLoader {
public:
    EditorBackEnd( SceneGraph& _sg, RenderOrchestrator& _rsg ) : RunLoopBackEndBase(_sg, _rsg), ScenePreLoader( _sg, _rsg ) {
        backEnd = std::make_unique<FrontEnd>( *this, _sg, _rsg );
    }
    ~EditorBackEnd() override = default;

    void updateImpl( const AggregatedInputData& _aid ) override;
    void activateImpl() override;

    const static LoginFields loginCert() {
        return LoginFields{ "guest", "guest", "eh_sandbox" };
    }

protected:
    void activatePostLoad() override;

protected:
    std::unique_ptr<FrontEnd> backEnd;
};
