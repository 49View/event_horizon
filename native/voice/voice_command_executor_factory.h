#pragma once

#include <memory>

#include "voice/voice_command.h"
#include "voice/voice_command_executor.h"

class VoiceCommandExecutorFactory {
public:
	template <typename T, typename P>
	void registerExecutor( std::string key, P* _provider ) {
		executors[key] = std::make_shared<T>( _provider );
	}

	std::shared_ptr<VoiceCommandExecutor> getExecutor( const VoiceCommand& cmd );
private:
	std::unordered_map<std::string, std::shared_ptr<VoiceCommandExecutor>> executors;
};
