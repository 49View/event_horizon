//
//  file_manager.cpp
//

#include <emscripten/em_asm.h>
#include <emscripten.h>
#include "../../file_manager.h"
#include "../../http/webclient.h"

static bool sbIsPersistentInitialized = false;

#ifdef __cplusplus
extern "C" {
#endif

EMSCRIPTEN_KEEPALIVE void setPersistanceInitialized() {
    sbIsPersistentInitialized = true;
}

EMSCRIPTEN_KEEPALIVE void setPersistanceSynced() {
    LOGR("Persistance FileSystem synced");
}

#ifdef __cplusplus
}
#endif

namespace FileManager {

    void initPersistent() {
        LOGR("Initialising IndexDB file system");
        if ( !sbIsPersistentInitialized ) {
            EM_ASM(
                //create your directory where we keep our persistent data
                FS.mkdir('/persistent_data');

                //mount persistent directory as IDBFS
                FS.mount(IDBFS,{},'/persistent_data');

                Module.print("start file sync..");
                //flag to check when data are synchronized
                Module.syncdone = 0;

                //populate persistent_data directory with existing persistent source data
                //stored with Indexed Db
                //first parameter = "true" mean synchronize from Indexed Db to
                //Emscripten file system,
                // "false" mean synchronize from Emscripten file system to Indexed Db
                //second parameter = function called when data are synchronized
                FS.syncfs(true, function(err) {
                    assert(!err);
                    Module.print("end file sync..");
                    Module.syncdone = 1;
                    ccall( 'setPersistanceInitialized', 'v' );
                });
            );
        }
    }

    void persistanceSync() {
        EM_ASM(
            FS.syncfs(function(err) {
                assert(!err);
                ccall( 'setPersistanceSynced', 'v' );
            });
        );
    }

    bool isPersistentInitialized() {
        return sbIsPersistentInitialized;
    }

    bool useFileSystemCachePolicy() {
        return false;
    }

}