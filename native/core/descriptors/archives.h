//
// Created by Dado on 27/12/2019.
//

#pragma once

#include <string_view>
#include <string>
#include <vector>
#include <unordered_map>

struct ArchiveDirectoryEntityElement {
    std::string name{};
    uint64_t size = 0;
    std::string metaString{};
};

using ArchiveDirectoryContainer = std::unordered_map<std::string, ArchiveDirectoryEntityElement>;
using ArchiveDirectoryContainerIterator = std::unordered_map<std::string, ArchiveDirectoryEntityElement>::iterator;
using ArchiveDirectoryContainerConstIterator = std::unordered_map<std::string, ArchiveDirectoryEntityElement>::const_iterator;


class ArchiveDirectory {
public:
    ArchiveDirectory() = default;
    ArchiveDirectory(const std::string &name) : name(name) {}

    std::vector<ArchiveDirectoryEntityElement> findFilesWithExtension(const std::vector<std::string> &_exts) const;

    void insert(ArchiveDirectoryEntityElement &&_elem);
    std::string Name() const;

    ArchiveDirectoryContainerConstIterator begin() const;
    ArchiveDirectoryContainerConstIterator end() const;
    ArchiveDirectoryContainerIterator begin();
    ArchiveDirectoryContainerIterator end();

private:
    std::string name;
    std::unordered_map<std::string, ArchiveDirectoryEntityElement> admap;
};

void unzipFilesToTempFolder(const std::string &filename, ArchiveDirectory& ad, const std::string& forcePathName = {});