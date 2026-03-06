#pragma once

#include "domain/Track.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

struct TrackMetadata {
    std::string path;
    std::string title;
    std::string artist;
    std::string album;
    std::string genre;
    int         duration     = 0;
    std::time_t lastModified = 0;
    int         year         = 0;
    int         trackNumber  = 0;
};

class MetadataManager {
    public:
        MetadataManager();

        TrackMetadata              extractMetadata(const std::string& filePath);
        void                       populateTrack(Track& track);
        std::vector<unsigned char> getArtwork(const std::string& filePath);
        void                       clearArtworkCache();

    private:
        std::string                getCacheFilePath(const std::string& filePath);
        std::vector<unsigned char> extractArtwork(const std::string& filePath);
        std::vector<unsigned char> findLooseArtwork(const std::string& filePath);
        std::vector<unsigned char> readAllBytes(const fs::path& path);
        bool                       writeAllBytes(const fs::path& path, const std::vector<unsigned char>& data);
        std::string                hashString(const std::string& str);

        std::string                                  cacheDir;
        std::unordered_map<std::string, std::string> artworkCache;
};
