#include "../../platform_util.h"
#include <unistd.h>
#include <sys/utsname.h>
#include <sstream>
#include <android/log.h>

void platformLogPrint( const std::string& logTag, float time, const std::string& message ) {

    #define  LOG_TAG    "[EH]"

    if ( logTag == "[INFO]" ) {
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "%s \n", message.c_str());
    }
    if ( logTag == "[WARNING]" ) {
        __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "%s \n", message.c_str());
    }
    if ( logTag == "[ERROR]" ) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s \n", message.c_str());
    }
}

void saveImageFromClipboard(const std::string& folderName) {
}

bool isTempOSFilename(const std::string& filename) {
    return false;
}

std::string cacheFolder() {
    return "";
}

//void FileManager::makeDir(const std::string& dirName) {
//	mkdir(dirName.c_str(),0775);
//}

bool isalnumCC(char c) {
    return std::isalnum(c);
}

int gethostname_em(char *name, size_t len)
{
    size_t i;
    struct utsname uts;
    if (uname(&uts)) return -1;
    if (len > sizeof uts.nodename) len = sizeof uts.nodename;
    for (i=0; i<len && (name[i] = uts.nodename[i]); i++);
    if (i==len) name[i-1] = 0;
    return 0;
}

const std::string userComputerName() {
    const char *uname = getenv( "USER" );
    char cname[512];
    char *cnameEnv = getenv( "HOSTNAME" );
    if ( cnameEnv == nullptr ) {
        if ( gethostname_em( cname, 512 ) == 0 ) { // success = 0, failure = -1
        }
    } else {
        strcpy( cname, cnameEnv );
    }
    auto userComputerName = std::string( uname ) + "@" + std::string( cname );
    return userComputerName;
}

const uint64_t cpuID() {
    return 0x37337;
}

namespace ClipBoard {
    std::string exec(const char* cmd)
    {
        FILE* pipe = popen(cmd, "r");
        if (!pipe) return "ERROR";
        char buffer[128];
        std::string result = "";
        while(!feof(pipe))
        {
            if(fgets(buffer, 128, pipe) != NULL)
            {
                result += buffer;
            }
        }
        pclose(pipe);
        return result;
    }

    std::string paste()
    {
        return exec("pbpaste");
    }

    std::string copy(const char * new_clipboard)
    {
        std::stringstream cmd;
        cmd << "echo \"" << new_clipboard << "\" | pbcopy";
        return exec(cmd.str().c_str());
    }
}

bool isTouchBased() {
    return true;
}
