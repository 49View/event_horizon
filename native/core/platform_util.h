#pragma once

#include <string>

void saveImageFromClipboard(const std::string& folderName);
bool isalnumCC(char c);
bool isTouchBased();
bool isTempOSFilename(const std::string& filename);