//
//  file_manager.cpp
//

#include "../../file_manager.h"

#include <fstream>
#include <stack>
#include <regex>

#include "../../util.h"
#include "../../http/webclient.h"

namespace FileManager {

    void initPersistent() {}
    bool isPersistentInitialized() { return true; }
    void persistanceSync() {}

    bool useFileSystemCachePolicy() {
        return true;
    }
    // If we do not want to check the time stamp of the data but read it directly from the cache, then set UseCacheDirect to true
    // it will return the cached file (whatever the timestamp) if it does exist, otherwise will carry on with the cache algorithm as usual

//    void readRemoteFileInternal( const Url& uri ) {
//        Http::get( uri, cacheLoad, Http::ResponseFlags::HeaderOnly );
//    }

}