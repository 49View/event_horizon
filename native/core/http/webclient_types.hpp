#pragma once

namespace HttpFilePrefix {
    const static std::string user = "/user";
    const static std::string gettoken = "/getToken";
    const static std::string refreshtoken = "/refreshToken";
    const static std::string entities = "/entities/";
    const static std::string entities_all = "/entities/metadata/byGroupTags/";
    const static std::string entities_onebinary = "/entities/content/byGroupTags/";
    const static std::string get = "/fs/";
    const static std::string fileupload = "/fs/";
    const static std::string catalog = "/catalog/";
    const static std::string getname = "/name/";
    const static std::string getnotexactname = "/get/notexact/name/";
    const static std::string broadcast = "/broadcast/";
};

namespace HttpContentType {
    const static std::string octetStream = "application/octet-stream";
    const static std::string json = "application/json";
    const static std::string text = "application/text";
}

enum class HttpQuery {
    Binary,
    JSON,
    Text
};

struct Url;

using LoginCallback = std::function<void()>;
//using HttpResouceCB = std::function<void(const std::string&)>;

using SocketCallbackDataType = rapidjson::Document;
using SocketCallbackDataTypeConstRef = const SocketCallbackDataType&;
using SocketCallbackFunc = std::function<void( const std::string& msg,  SocketCallbackDataType&& message )>;
