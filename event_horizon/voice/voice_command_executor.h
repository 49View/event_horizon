#pragma once

#include <unordered_map>
#include <string>

struct Command;

class CommandExecutor {
public:

	virtual void execute( std::shared_ptr<Command> cmd ) = 0;
};

class VoiceCommandExecutor {
public:
	virtual void execute( const std::unordered_map<std::string, std::string>& commandData ) = 0;
	virtual bool requireSync() = 0;

	inline std::string getString( const std::unordered_map<std::string, std::string>& commandData, const std::string& commandName ) {
		auto val = commandData.find( commandName );
		if ( val == commandData.end() )return "";
		return val->second;
	}

	inline int getInt( const std::unordered_map<std::string, std::string>& commandData, const std::string& commandName, int deafult ) {
		auto val = commandData.find( commandName );
		if ( val == commandData.end() )return deafult;

		return  std::stoi( val->second, NULL );
	}
	
};
