#include <string>
#include <thread>
#include <iostream>
#include <sstream>
#include "../../runloop_core.h"
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
//        cq.script( inputLine );
        std::cout<< "> ";
    }
}

void RunLoop::coreLoop() {
    while  ( !mbExitTriggered ) {
        DH::update();
    }
}

void RunLoop::coreFunctions() {
    DH::update();
}

void RunLoop::runConsolePrompt() {
    std::thread promptThread( &RunLoop::consolePrompt, this );
    promptThread.detach();
}

