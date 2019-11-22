#pragma once

#include <string>
#include <unordered_map>

#include "core/util.h"

class VoiceCommand {
public:
	VoiceCommand() {}

	VoiceCommand( const std::initializer_list<std::string>& slist ) {
		std::vector<std::string> larguments;
		for ( const auto& cs : slist ) {
			larguments.push_back( cs );
		}
		arguments( larguments );
	}

	VoiceCommand( const std::vector<std::string>& slist ) {
		arguments( slist );
	}

	void arguments( const std::vector<std::string>& slist ) {
		ASSERT( slist.size() % 2 == 0 );
		for ( auto q = 0; q < slist.size(); q += 2 ) {
			values[slist[q]] = slist[q + 1];
		}
	}

	bool isValid() {
		return values.find( "rule" ) != values.end();
	}

public:
	bool hasValue;
	std::string rawValue;
	std::unordered_map<std::string, std::string> values;
};

