#include "voice_command_executor_factory.h"

std::shared_ptr<VoiceCommandExecutor> VoiceCommandExecutorFactory::VoiceCommandExecutorFactory::getExecutor( const VoiceCommand& cmd ) {
	auto commandData = cmd.values;

	auto rule = commandData.find( "rule" );
	if ( rule == commandData.end() ) return nullptr;

	auto action = commandData.find( "action" );
	if ( action == commandData.end() ) return nullptr;

	auto key = rule->second + "_" + action->second;

	auto result = executors.find( key );

	if ( result == executors.end() ) return nullptr;

	return result->second;
}