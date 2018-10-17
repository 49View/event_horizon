#include "command.hpp"

void CommandQueue::script( const std::string& cmd, CommandCase cc ) {

    for ( const auto& sq : scriptQueue ) {
        if ( sq->enqueueIfCmdBelongs( cmd, cc ) ) break;
    }
}

bool CommandScript::enqueueIfCmdBelongs( const std::string& _cmdString, CommandCase cc ) {

    std::string cmdLower = cc == CommandCase::Insensitive ? toLower(_cmdString) : _cmdString;
    auto cs = split(cmdLower);

    std::string agg = "";
    for ( size_t q = 0; q < cs.size(); q++ ) {
        agg += cs[q];
        if ( commandDefinitions.find( agg ) != commandDefinitions.end() ) {
            script.push_back( cmdLower );
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
        auto cmd = concatenate( " ", {commandLineString.begin(), itEnd} );
        auto cmdExe = commandDefinitions.find(cmd);
        if ( cmdExe != commandDefinitions.end() ){
            LOGR(cmdExe->first.c_str());
            cmdExe->second( {itEnd, commandLineString.end()} );
        }
    }
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
