//
// Created by Dado on 2018-10-16.
//

#pragma once

#include <render_scene_graph/scene_bridge.h>

struct FullEditorStateMachine : state_machine_def<FullEditorStateMachine> {
    explicit FullEditorStateMachine( SceneOrchestrator* _owner ) : so( _owner ) {}

    // Events
    struct OnWantToStartGame{};
    struct OnWantToEndGame{};

    // States
    struct InitialStateSetup : state<> {
        template <class Event, class FSM>
        void on_entry(Event const& evt, FSM& _sm)
        {
            _sm.init();
        }
    };
    struct Play : state<> {};

    // Actions
    void startGame( OnWantToStartGame const &) {
    }
    void endGame( OnWantToEndGame const &) {
    }

    struct transition_table : mpl::vector<
            a_row<InitialStateSetup, OnWantToStartGame, Play, &FullEditorStateMachine::startGame>
    > {};

    typedef InitialStateSetup initial_state;

    sm_not // Just to reduce boilderplate of no_transaction empty function

    void init();

private:
    SceneOrchestrator* so;
};
