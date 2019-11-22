//
//  file_manager.cpp
//

#include "../../file_manager.h"
#include <core/util.h>
#include <stdio.h>
#include <errno.h>

#ifdef USE_GLFM
#include <graphics/opengl/mobile/glfm.h>
#define FILE_COMPAT_ANDROID_ACTIVITY glfmAndroidGetActivity()
#endif
#ifdef USE_GLFVR
#include <android/native_activity.h>
#define FILE_COMPAT_ANDROID_ACTIVITY NULL
#endif
#include <android/asset_manager.h>

#if !defined(FILE_COMPAT_ANDROID_ACTIVITY)
#error FILE_COMPAT_ANDROID_ACTIVITY must be defined as a reference to an ANativeActivity (or NULL).
#endif

static int fc_resdir(char *path, size_t path_max) {
    if (!path || path_max == 0) {
        return -1;
    }
    path[0] = 0;
    return 0;
}

//#if !defined(_BSD_SOURCE)
//    FILE* funopen(const void* __cookie,
//                  int (*__read_fn)(void*, char*, int),
//                  int (*__write_fn)(void*, const char*, int),
//                  fpos_t (*__seek_fn)(void*, fpos_t, int),
//                  int (*__close_fn)(void*));
//#endif /* _BSD_SOURCE */

size_t fclength(void *cookie) {
    return (size_t)AAsset_getLength64((AAsset *)cookie);
}

size_t fcread(void *cookie, char *buf, size_t size) {
    return AAsset_read((AAsset *)cookie, buf, (size_t)size);
//    memcpy( buf, AAsset_getBuffer( (AAsset *)cookie ), size );
//    return size;
}

size_t fcwrite(void *cookie, const char *buf, int size) {
    (void)cookie;
    (void)buf;
    (void)size;
    errno = EACCES;
    return 0;
}

size_t fcseek(void *cookie, fpos_t offset, int whence) {
    return AAsset_seek((AAsset *)cookie, offset, whence);
}

void fcclose(void *cookie) {
    AAsset_close((AAsset *)cookie);
}

FILE* fcopen(const char *filename, const char *mode) {
    ANativeActivity *activity = FILE_COMPAT_ANDROID_ACTIVITY;
    AAssetManager *assetManager = NULL;
    AAsset *asset = NULL;
    if (activity) {
        assetManager = activity->assetManager;
    }
    if (assetManager && mode && mode[0] == 'r') {
        asset = AAssetManager_open(assetManager, filename, AASSET_MODE_UNKNOWN);
//        off64_t length = AAsset_getLength64(asset);
//        LOGRS( length );
    }
    if (asset) {
        return (FILE*)asset;// funopen(asset, fcread, fcwrite, fcseek, fcclose);
    } else {
        return fopen(filename, mode);
    }
}

namespace FileManager {

    void initPersistent() {}
    bool isPersistentInitialized() { return true; }
    void persistanceSync() {}

    bool useFileSystemCachePolicy() {
        return true;
    }

    std::string systemRootDir() {
        char fullPath[PATH_MAX];
        fc_resdir(fullPath, sizeof(fullPath));
        return std::string(fullPath);
    }

    // If we do not want to check the time stamp of the data but read it directly from the cache, then set UseCacheDirect to true
    // it will return the cached file (whatever the timestamp) if it does exist, otherwise will carry on with the cache algorithm as usual
//    void readRemoteFileInternal( const Url& uri ) {
//        Http::get( uri, cacheLoad, Http::ResponseFlags::HeaderOnly );
//    }

}