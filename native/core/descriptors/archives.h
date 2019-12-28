//
// Created by Dado on 27/12/2019.
//

#pragma once

#include <string_view>
#include <string>
#include <vector>
#include <unordered_map>

struct ArchiveDirectoryEntityElement {
    std::string name;
    uint64_t size;
};

class ArchiveDirectory {
public:
    std::vector<ArchiveDirectoryEntityElement> findFilesWithExtension(const std::string &_ext) const;
    void insert( ArchiveDirectoryEntityElement&& _elem );
private:
    std::unordered_map<std::string, ArchiveDirectoryEntityElement> admap;
};

ArchiveDirectory unzipFilesToTempFolder(const std::string &filename);