//
// Created by Dado on 01/05/2018.
//

#pragma once

#include <string>

#ifdef _CURL_ENABLED_

namespace CurlUtil {

    std::string read( const std::string& url );
    const char* read( const std::string& url, uint64_t& size );

};

#endif

