//
//  file_manager.cpp
//

#include "../../file_manager.h"

#include <fstream>
#include <stack>
#include <regex>

#include "../../util.h"
#include "../../http/webclient.h"

size_t fclength(void *cookie) {
    fseek((FILE *)cookie, 0, SEEK_END);
    size_t length = ftell((FILE *)cookie);
    fseek((FILE *)cookie, 0, SEEK_SET);
    return length;
}

size_t fcread(void *cookie, char *buf, size_t size) {
    return fread(buf, (size_t)size, 1, (FILE *)cookie);
}

size_t fcwrite(void *cookie, const char *buf, int size) {
    return fwrite(buf, (size_t)size, 1, (FILE *)cookie);
}

size_t fcseek(void *cookie, fpos_t offset, int whence) {
    fseek((FILE *)cookie, 0, SEEK_END);
    return ftell((FILE *)cookie);
}

void fcclose(void *cookie) {
    fclose((FILE*)cookie);
}

FILE* fcopen(const char *filename, const char *mode) {
    return fopen( filename, mode );
}

namespace FileManager {

    void initPersistent() {}
    bool isPersistentInitialized() { return true; }
    void persistanceSync() {}

    bool useFileSystemCachePolicy() {
        return false;
    }

    std::string systemRootDir() {
        return "";
    }

    // If we do not want to check the time stamp of the data but read it directly from the cache, then set UseCacheDirect to true
    // it will return the cached file (whatever the timestamp) if it does exist, otherwise will carry on with the cache algorithm as usual

//    void readRemoteFileInternal( const Url& uri ) {
//        Http::get( uri, cacheLoad, Http::ResponseFlags::HeaderOnly );
//    }

}