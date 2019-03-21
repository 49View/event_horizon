#include "command.hpp"
#include <core/string_util.h>

void CommandQueue::script( const std::string& cmd, CommandCase cc ) {

    for ( const auto& sq : scriptQueue ) {
        if ( sq->enqueueIfCmdBelongs( cmd, cc ) ) break;
    }
}

bool CommandScript::enqueueIfCmdBelongs( const std::string& _cmdString, CommandCase cc ) {

    auto cs = split(_cmdString);

    std::string agg = "";
    for ( const auto& c : cs ) {
        agg += cc == CommandCase::Insensitive ? toLower(c) : c;
        if ( commandDefinitions.find( agg ) != commandDefinitions.end() ) {
            script.push_back( _cmdString );
            return true;
        }
        agg += " ";
    }
    return false;
}

void CommandScript::addCommandDefinition( const std::string& _cmdString,
                                          std::function<void(const std::vector<std::string>&)> _fptr) {
    std::string cmdLower = toLower(_cmdString);
    commandDefinitions[cmdLower] = _fptr;
}

void CommandScript::executeCommand( const std::vector<std::string>& commandLineString ) {
    ASSERT( !commandLineString.empty() );

    for ( size_t ct = 1; ct <= commandLineString.size(); ct++ ) {
        auto itEnd = ct == commandLineString.size() ? commandLineString.end() : commandLineString.begin() + ct;
        auto cmd = toLower(concatenate( " ", {commandLineString.begin(), itEnd} ));
        auto cmdExe = commandDefinitions.find(cmd);
        if ( cmdExe != commandDefinitions.end() ){
            LOGR(cmdExe->first.c_str());
            cmdExe->second( {itEnd, commandLineString.end()} );
        }
    }
}

void CommandScript::execute() {
    if ( script.empty() ) return;

    static const std::string regExpAggregatingDoubleQuotesPair = "(\"[^\"]+\")|\\S+";
    for ( const auto& cs : script ) {
        auto cmds = split_regexp(cs, regExpAggregatingDoubleQuotesPair);
        for ( auto& token : cmds ) token = trim( token, '"' );
        executeCommand( cmds );
    }
    script.clear();
}

void CommandSubMapping::addSubCommand( const std::string& _cmd,
                                       std::function<void( const std::vector<std::string>& )> _fn,
                                       std::function<void( const std::vector<std::string>& )> _fnEnd ) {

    mSubCommands[_cmd].first = _fn;
    mSubCommands[_cmd].second = _fnEnd;
}

void CommandSubMapping::subCommandRun( const std::vector<std::string>& _params, SubCommandStep _step ) {
    auto cmd  = concatenate( " ", {_params.begin(), _params.end()} );
    if ( auto it = mSubCommands.find(cmd); it != mSubCommands.end() ) {
        if ( _step == SubCommandStep::Start ) {
            it->second.first( _params );
        } else {
            if ( it->second.second ) it->second.second( _params );
        }
    }
}
