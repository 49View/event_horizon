#include "../../../platform_util.h"

#include <regex>
#include <cpuid.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include <pwd.h>

#include "../../../util.h"

void platformLogPrint( const std::string& logTag, float time, const std::string& message ) {
    std::cout << logTag << " " << time << " " << message << std::endl;
}

void saveImageFromClipboard( [[maybe_unused]] const std::string& folderName) {
}

bool isalnumCC(char c) {
    return std::isalnum(c);
}

namespace FileManager {
    void makeDir( const std::string& dirName ) {
        mkdir( dirName.c_str(), 0775 );
    }
}

std::string cacheFolder() {
    const char *homeDir = getenv( "TMPDIR" );
    return std::string( homeDir );
}

std::string getDaemonRoot() {
    return cacheFolder();
}

const std::string userComputerName() {
    const char *uname = getenv( "USER" );
    char cname[512];
    char *cnameEnv = getenv( "HOSTNAME" );
    if ( cnameEnv == nullptr ) {
        if ( gethostname( cname, 512 ) == 0 ) { // success = 0, failure = -1
        }
    } else {
        strcpy( cname, cnameEnv );
    }
    auto userComputerName = std::string( uname ) + "@" + std::string( cname );
    return userComputerName;
}

const uint64_t cpuID() {

    unsigned int cpuinfo[4] = { 0, 0, 0, 0 };
    unsigned int ax = 0;

    __get_cpuid(ax, &cpuinfo[0], &cpuinfo[1], &cpuinfo[2], &cpuinfo[3]);

    unsigned short hash = 0;
    unsigned int* ptr = (&cpuinfo[0]);
    for ( unsigned int i = 0; i < 4; i++ )
        hash += (ptr[i] & 0xFFFF) + ( ptr[i] >> 16 );

    return hash;
}

const std::string userName() {
    const char *uname = getenv( "USER" );
    auto userName = std::string( uname );
    return userName;
}

const std::string computerName() {
    char cname[512];
    char *cnameEnv = getenv( "HOSTNAME" );
    if ( cnameEnv == nullptr ) {
        if ( gethostname( cname, 512 ) == 0 ) { // success = 0, failure = -1
        }
    } else {
        strcpy( cname, cnameEnv );
    }
    return std::string( cname );
}

const std::string getUserDownloadFolder( const std::string forcedUserName ) {

    return "/Users/" + (forcedUserName.empty() ? userName() : forcedUserName) + "/Downloads";
}

bool checkFileNameNotACopy( const std::string& filename )  {
    std::vector<std::regex> fcPatterns = { std::regex("\\s\\(\\d\\)"), std::regex("\\s\\d\\.\\w") };
    for ( const auto& wb1 : fcPatterns ) {
        std::smatch base_match;
        size_t expectedResults = 1;
        std::regex_search( filename, base_match, wb1 );
        auto nM = base_match.size();
        if ( nM == expectedResults ) {
            return true;
        }
    }
    return false;
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
    return false;
}
