#pragma once

#include <string>
#include "restbed"
#include "core/safe_queues.h"
#include "voice/voice_command.h"
#include "voice/voice_command_executor_factory.h"

class VoiceInput {
public:
	static VoiceInput& getInstance() {
		static VoiceInput instance; // Guaranteed to be destroyed.
		return instance;// Instantiated on first use.
	}

public:
	void init();
	void destroy();
	void check();

private:
	VoiceCommand getCommand();

	VoiceInput();
	VoiceInput( VoiceInput const& ) = delete;
	void operator=( VoiceInput const& ) = delete;

	void commandMethodHandler( const std::shared_ptr< restbed::Session > session );
	void pingMethodHandler( const std::shared_ptr< restbed::Session > session );
	void runServer();

private:
	restbed::Service service;
	std::unique_ptr<std::thread> httpServerThread;
	ProducerConsumerQueue<VoiceCommand> commandQueue;
	std::shared_ptr<VoiceCommandExecutorFactory> commandFactory;
};

#define VI VoiceInput::getInstance()
