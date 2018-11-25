//
//  megawriter_fs.hpp
//  sixthview
//
//  Created by Dado on 01/10/2015.
//
//

#pragma once

#include "megawriter.hpp"

//#include "http/webclient.h"

template < typename T >
class MegaWriterFS {
public:
	MegaWriterFS( const std::string& _collection, const T& val ) {
//		StringBuffer s;
//		MegaWriter writer( s );
//		val.serialize( &writer );
//
//		Http::post( Url( "/catalog/" + _collection + "/create" ), s.GetString() );
	}
};

template < typename T >
class MegaWriterArrrayFS {
public:
	MegaWriterArrrayFS( const std::string& _collection, const std::vector<T>& val ) {

//		if ( val.empty() ) return;
//
//		StringBuffer s;
//		MegaWriter writer( s );
//		writer.serialize( nullptr, val );
//
//		Http::post( Url( "/catalog/" + _collection + "/insert_array" ), s.GetString() );
	}
};
