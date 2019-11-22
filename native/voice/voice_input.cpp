#include "voice_input.h"
#include <memory>
#include <cstdlib>
#include <iostream>
#include <thread>
#include "restbed"
#include "core/util.h"
#include "core/rapidjson/document.h"
#include "core/service_factory.h"

VoiceInput::VoiceInput() {
	commandFactory = ServiceFactory::get<VoiceCommandExecutorFactory>();
}

void VoiceInput::init() {
	httpServerThread = std::make_unique< std::thread>( std::bind( &VoiceInput::runServer, this ) );
}

void VoiceInput::destroy() {
	service.stop();
	if ( httpServerThread->joinable() )
		httpServerThread->join();
}

void VoiceInput::check() {
	auto cmd = getCommand();
	if ( cmd.hasValue ) {
		auto executor = commandFactory->getExecutor( cmd );

		if ( executor )
			executor->execute( cmd.values );
	}
}

VoiceCommand VoiceInput::getCommand() {
	VoiceCommand cmd;
	cmd.hasValue = false;

	if ( !commandQueue.empty() )
		cmd = commandQueue.dequeue();

	return cmd;
}

void VoiceInput::commandMethodHandler( const std::shared_ptr< restbed::Session > session ) {
	const std::shared_ptr< const restbed::Request > request = session->get_request();

#ifndef OSX
	//auto headers = request->get_headers();
	auto content_length = request->get_header( "Content-Length", 0 );

	session->fetch( content_length, [request, this]( const std::shared_ptr< restbed::Session > session, const restbed::Bytes & body ) {
		const char* bytes = reinterpret_cast<const char*>( body.data() );

		rapidjson::Document document;
		document.Parse( bytes, body.size() );

		LOGI( "Command received:  %.*s", (int)body.size(), body.data() );

		VoiceCommand cmd;

		if ( document.IsObject() ) {
			for ( rapidjson::Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr ) {
				if ( itr->value.IsString() ) {
					cmd.values[itr->name.GetString()] = itr->value.GetString();
				}
			}
		}

		session->close( restbed::OK, "Done", { { "Content-Length", "4" },{ "Connection", "close" } } );

		if ( cmd.isValid() ) {
			cmd.hasValue = true;
			cmd.rawValue.assign( bytes, bytes + (int)body.size() );

			auto executor = commandFactory->getExecutor( cmd );

			if ( executor ) {
				if ( executor->requireSync() )
					commandQueue.enqueue( cmd );
				else
					executor->execute( cmd.values );
			}
		}
	} );
#endif
}

void VoiceInput::pingMethodHandler( const std::shared_ptr< restbed::Session > session ) {
	LOGI( "Ping request received" );
	session->close( restbed::OK, "OK", { { "Content-Length", "2" },{ "Connection", "close" } } );
}

void VoiceInput::runServer() {
#ifndef OSX
	const int port = 7777;

	auto cmdResource = std::make_shared<restbed::Resource >();
	cmdResource->set_path( "/queuecommand" );
	cmdResource->set_method_handler( "POST", std::bind( &VoiceInput::commandMethodHandler, this, std::placeholders::_1 ) );

	service.publish( cmdResource );

	auto pingResource = std::make_shared<restbed::Resource >();
	pingResource->set_path( "/ping" );
	pingResource->set_method_handler( "GET", std::bind( &VoiceInput::pingMethodHandler, this, std::placeholders::_1 ) );

	service.publish( pingResource );

	auto settings = std::make_shared<restbed::Settings>();
	settings->set_port( port );

	LOGI( "Listening on port %d", port );

	service.start( settings );
#endif
}