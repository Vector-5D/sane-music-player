#pragma once

#include <string>
#include <vector>
#include <optional>

namespace FileDialog {
    // filter example: "mp3,flac,wav,ogg" or empty for all files
    std::optional<std::string>              openFile(const std::string& filter = "");
    std::optional<std::string>              openFolder();
    std::optional<std::vector<std::string>> openMultiple(const std::string& filter = "");
}
