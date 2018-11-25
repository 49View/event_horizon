//
//  megaReaderFS.hpp
//  sixthview
//
//  Created by Dado on 01/10/2015.
//
//

#pragma once

#include "megareader.hpp"
//#include "http/webclient.h"
//#include "file_manager.h"

//template <typename T>
//struct JSONCallbackData : public CallbackData {
//	JSONCallbackData( T val ) : val( val ) {}
//	T val;
//};
//
//template <typename T>
//struct JSONCallbackHandler : public FileCallbackHandler {
//
//	JSONCallbackHandler( T& value ) {
//		cbData = std::make_shared<JSONCallbackData<T>>(value);
//	}
//
//	virtual bool executeCallback() {
//		auto tData = std::dynamic_pointer_cast<JSONCallbackData<T>>(cbData);
//		rapidjson::Document document;
//		document.Parse<rapidjson::kParseStopWhenDoneFlag>( cbData->asString().c_str() );
//		MegaReader reader( document );
//		if ( !reader.isEmpty() ) {
//			tData->val.deserialize( reader );
//			return true;
//		}
//		return false;
//	}
//};
//
//template <typename T>
//struct JSONArrayCallbackData : public CallbackData {
//	JSONArrayCallbackData( std::vector<T>& val ) : val( val ) {}
//	std::vector<T> val;
//};
//
//template <typename T>
//struct JSONArrayCallbackHandler : public FileCallbackHandler {
//
//	JSONArrayCallbackHandler( std::vector<T>& value ) {
//		cbData = std::make_shared<JSONArrayCallbackData<T>>(value);
//	}
//
//	virtual bool executeCallback() {
//		auto tData = std::dynamic_pointer_cast<JSONArrayCallbackData<T>>(cbData);
//		rapidjson::Document document;
//		document.Parse<rapidjson::kParseStopWhenDoneFlag>( cbData->asString().c_str()  );
//		if ( document.Size() == 0 ) return false;
//		for (SizeType i = 0; i < document.Size(); i++) {
//			tData->val.push_back(T(MegaReader(document[i])));
//		}
//
//		return true;
//	}
//};

template < typename T >
void readFS( [[maybe_unused]] const std::string& _collection, const std::string& _name, [[maybe_unused]] const std::string& _key, [[maybe_unused]] T& val ) {
//	std::string nameEncoded = url_encode( _name );
//	FM::readRemote( Url{ "/catalog/" + _collection + "/get/" + _key + "/" + nameEncoded },
//					std::make_shared<JSONCallbackHandler<T>>( val ),
//					Http::ResponseFlags::JSON );
}

template < typename T >
void readFS( [[maybe_unused]] const std::string& _collection, const std::string& _name, [[maybe_unused]] const std::string& _key, [[maybe_unused]] std::vector<T>& val ) {
//	std::string nameEncoded = url_encode( _name );
//	FM::readRemote( Url{ "/catalog/" + _collection + "/get_many/" + _key + "/" + nameEncoded },
//					std::make_shared<JSONArrayCallbackHandler<T>>( val ),
//					Http::ResponseFlags::JSON );
}

