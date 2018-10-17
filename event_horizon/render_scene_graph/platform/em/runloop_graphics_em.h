//
// Created by Dado on 01/01/2018.
//

#pragma once

extern RunLoopGraphics rl;
void main_loop_em();

template <typename T> void mainLoop( [[maybe_unused]] uint64_t _flags, [[maybe_unused]] RunLoopThreading rt ) {
    rl.initWindow( di::make_injector(APP_RSGINJECTOR).create<std::shared_ptr<T>>(), _flags );
    emscripten_set_main_loop( main_loop_em, 0, 0 );
}
