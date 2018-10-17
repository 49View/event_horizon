#pragma once

#include <chrono>
#include <string>
#include <iostream>

struct Profiler {
	std::string name;
	std::chrono::high_resolution_clock::time_point p;
	std::size_t taskId;
	std::string infos;
	bool inTask;

	Profiler( std::string const &n ) :
		name( n ), p( std::chrono::high_resolution_clock::now() ) {
		inTask = false;
	}

	Profiler( std::string const &n, const size_t &  id ) :
		name( n ), p( std::chrono::high_resolution_clock::now() ), taskId( id ) {
		inTask = true;
	}

	~Profiler() {
		//if ( inTask ) return;
		using dura = std::chrono::duration<double>;
		auto d = std::chrono::high_resolution_clock::now() - p;

		auto s = name + ( inTask ? ( " ID " + std::to_string( taskId ) ) : "" ) + ": " + std::to_string( std::chrono::duration_cast<dura>( d ).count() ) + infos + "\n";
		//std::cout.imbue( std::locale( "" ) );
		std::cout << s;
	}

	template<class T>
	void addValue( std::string _name, T value ) {
		infos += "\n\t" + _name + ": " + std::to_string( value );
	}

	void addValue( std::string _name, std::string value ) {
		infos += "\n\t" + _name + ": " + value;
	}

	void addInfo( std::string info ) {
		infos += info;
	}
};

#define PROFILE_BLOCK(pbn) Profiler _pfinstance(pbn)
#define TASK_PROFILE_BLOCK(pbn, tId) Profiler _pfinstance(pbn, tId)
#define PROFILE_ADD(name, value) _pfinstance.addValue(name, value);
#define PROFILE_ADD_INFO(info) _pfinstance.addInfo(info);

//#define PROFILE_BLOCK(pbn)
//#define TASK_PROFILE_BLOCK(pbn, tId)
//#define PROFILE_ADD(name, value)
//#define PROFILE_ADD_INFO(info)
