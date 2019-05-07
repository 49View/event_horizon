//
// Created by Dado on 01/05/2018.
//

#include "../../http/curl_utils.h"

namespace CurlUtil {

    enum class CurlRead {
        String,
        Binary
    };

    std::string read( [[maybe_unused]] const std::string& url ) {
        return "";
    }

    const char* read( [[maybe_unused]] const std::string& url, [[maybe_unused]] uint64_t& size ) {
        return "";
    }
}