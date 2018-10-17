//
//  command.hpp
//  SixthViewer
//
//  Created by Dado on 14/12/2015.
//

#pragma once

#include <vector>
#include <sstream>
#include <string>
#include <regex>
#include <unordered_map>
#include <memory>
#include <thread>
#include <mutex>

#include "util.h"
#include "string_util.h"

CommandArgumentsT commandTokenize( const std::string& _line);

enum class CommandCase {
    Sensitive,
    Insensitive
};

struct Command {
	virtual ~Command() = default;

	template<typename...Ts>
	Command(Ts&&... args)  {

		std::ostringstream arguments;
		(void)std::initializer_list<int>{( arguments << args << " ", 0 )...};
		std::string line = arguments.str();

		mArguments = split(line);
	}

	virtual void execute() = 0;
	virtual void undo() = 0;

    void setCallback( CommandCallbackFunction f ) { Callback = f; }

	CommandArgumentsT mArguments;
	CommandCallbackFunction Callback = nullptr;
};


class CommandScript {
public:
    void executeCommand( const std::vector<std::string>& commandLineString );
    bool enqueueIfCmdBelongs( const std::string& _cmdString, CommandCase cc );
    void addCommandDefinition( const std::string& _cmdString,
                               std::function<void(const std::vector<std::string>&)> _fptr );

    void execute() {
        if ( script.empty() ) return;

        for ( const auto& cs : script ) {
            executeCommand( split(cs) );
        }
        script.clear();
    }

protected:
    std::vector<std::string> script;
    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)> > commandDefinitions;
};

class CommandQueue  {
public:
    CommandQueue() {
        static int _id = 0;
        id = _id++;
		ASSERT(id == 0);
    }

	template<typename TCmd>
	void enqueue( const TCmd cmd ) {
		std::unique_lock<std::mutex> lock( mutex );
		internalQueue.push_back( std::make_shared<TCmd>(cmd) );
	}

    void script( const std::string& cmd, CommandCase cc = CommandCase::Insensitive);

    void registerCommandScript( std::shared_ptr<CommandScript> _commandScript ) {
        scriptQueue.push_back( _commandScript );
	}

	void execute() {
        if ( scriptQueue.empty() ) return;

        for ( const auto& sq : scriptQueue ) {
            sq->execute();
        }
	}

protected:
	mutable std::mutex mutex;
	std::vector< std::shared_ptr<Command> > internalQueue;
    std::vector< std::shared_ptr<CommandScript> > scriptQueue;
	size_t top = 0;
    int id;
};

using SubCommandFunctionMapping = std::map<std::string,
                                           std::pair<std::function<void(const std::vector<std::string>&)>,
                                           std::function<void(const std::vector<std::string>&)>>>;

enum class SubCommandStep {
	Start,
	End
};

class CommandSubMapping {
public:
    void addSubCommand( const std::string& _cmd,
    					std::function<void(const std::vector<std::string>&)> _fn,
						std::function<void( const std::vector<std::string>& )> _fnEnd = nullptr );
    void subCommandRun( const std::vector<std::string>& _params, SubCommandStep _step );
private:
    SubCommandFunctionMapping mSubCommands;
};
