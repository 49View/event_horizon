#include <string>
#include <thread>
#include <iostream>
#include <core/command.hpp>
#include "../../runloop_core.h"
#include "../../command.hpp"
#include "../../callback_dependency.h"

void RunLoop::consolePrompt() {
    std::string command;
    std::string inputLine;
    std::stringstream stream;
    std::cout<< "> ";
    while (std::getline(std::cin, inputLine)) {
        stream << inputLine << std::endl;
        if ( inputLine == "exit" ) {
            mbExitTriggered = true;
            break;
        }
        cq.script( inputLine );
        std::cout<< "> ";
    }
}

void RunLoop::coreLoop() {
    while  ( !mbExitTriggered ) {
        DH::update();
        cq.execute();
    }
}

void RunLoop::coreFunctions() {
    DH::update();
    cq.execute();
}

CommandQueue& RunLoop::CQ() { return cq; }

void RunLoop::runConsolePrompt() {
    std::thread promptThread( &RunLoop::consolePrompt, this );
    promptThread.detach();
}

