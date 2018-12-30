#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "htypes_shared.hpp"
#include "http/webclient.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "callback_dependency.h"
#include <core/builders.hpp>

struct FileInfo {
	std::string fullPath;
	std::string name;
	std::string nameWithoutExt;
	std::string extension;
	uint64_t	attributes;
	uint64_t    date_modified;
	uint64_t    size;
};

namespace FileManager {

    bool useFileSystemCachePolicy();
	bool fileExist( const std::string& filename );
	bool fileExistAbs( const std::string& filename );
	uint64_t fileDate( const std::string& filename );
	std::string filenameOnly( const std::string& input );
	std::string filenameOnlyNoExtension( const std::string& input );

	// Remote file access
	void readRemoteFileInternal( const Url& uri );

    void readRemoteSimpleCallback( const std::string& filename, ResponseCallbackFunc simpleCallback );

    void writeRemoteFile( const std::string& filename, const char *buff, uint64_t length,
                          HttpUrlEncode _filenameEnc = HttpUrlEncode::Yes );
    void writeRemoteFile( const std::string& _filename, const std::vector<unsigned char>& _data );
//	void renameRemoteFile( const std::string& filename, const std::string& newfilename );
//	void removeRemoteFile( const std::string& filename );
//	std::vector<FileInfo> listRemoteFolder( const std::string& _folderName, const std::string& _maskOut = "" );
//	void makeRemoteDir( const std::string& dirname );

	// Local file access
    uint8_p readLocalFile( const std::string& filename );
	std::unique_ptr<uint8_t[]> readLocalFile( const std::string& filename, uint64_t& _length,
											  bool addTrailingZero = false );
	std::string readLocalTextFile( const std::string& filename );
	std::string readLocalTextFile( const std::wstring& filename );
    std::vector<std::string> readLocalTextFileLineByLine( const std::string& filename );
	bool writeLocalFile( const std::string& filename, const char *buff, uint64_t length, bool isFullPath = false );
	bool writeLocalFile( const std::string& filename, const rapidjson::StringBuffer& s, bool isFullPath = false );
    bool writeLocalFile( const std::string& filename, const std::vector<unsigned char>& s );
    bool writeLocalFile( const std::string& filename, const std::vector<std::string>& s );
    bool writeLocalFile( const std::string& filename, const std::string& s );
    bool writeLocalFile( const std::string& filename, const std::stringstream& s );
    bool writeLocalTextFile( const std::string& filename, const std::string& s, bool isFullPath = false );
    void makeLocalDir( const std::string& dirName );
	void copyLocalFile( const std::string& src_path, const std::string& dst_path );
	void deleteLocalFile( const std::string& src_path );

	// Transfers
	void copyLocalToRemote( const std::string& source, const std::string& dest,
	                        HttpUrlEncode _filenameEnc = HttpUrlEncode::Yes );
}

namespace FM = FileManager;
