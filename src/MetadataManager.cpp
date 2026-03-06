#include "MetadataManager.hpp"
#include "Logger.hpp"
#include <filesystem>
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/mpegfile.h>
#include <taglib/flacfile.h>
#include <taglib/mp4file.h>
#include <taglib/mp4tag.h>
#include <taglib/vorbisfile.h>
#include <taglib/xiphcomment.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/attachedpictureframe.h>

namespace fs = std::filesystem;

MetadataManager::MetadataManager() {
#ifdef _WIN32
    const char* appData = std::getenv("LOCALAPPDATA");
    cacheDir = appData ? std::string(appData) + "/SaneMusicPlayer/artwork_cache"
                       : "./artwork_cache";
#else
    const char* home = std::getenv("HOME");
    cacheDir = home ? std::string(home) + "/.cache/SaneMusicPlayer/artwork_cache"
                    : "./artwork_cache";
#endif

    try {
        if (!fs::exists(this->cacheDir)) {
            fs::create_directories(this->cacheDir);
            LOG_INFO("Created artwork cache directory: %s", this->cacheDir.c_str());
        }
    } catch (const std::exception& e) {
        LOG_WARN("Failed to create cache directory: %s", e.what());
        this->cacheDir = "./artwork_cache";
        fs::create_directories(this->cacheDir);
    }
}

std::string MetadataManager::hashString(const std::string& str) {
    std::hash<std::string> hasher;
    size_t hash = hasher(str);
    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::setw(16) << hash;
    return oss.str();
}

std::string MetadataManager::getCacheFilePath(const std::string& filePath) {
    return this->cacheDir + "/" + this->hashString(filePath) + ".jpg";
}

TrackMetadata MetadataManager::extractMetadata(const std::string& filePath) {
    TrackMetadata meta;
    meta.path = filePath;

    TagLib::FileRef f(filePath.c_str());
    if (f.isNull() || !f.tag()) return meta;

    auto* t = f.tag();
    meta.title       = t->title().to8Bit(true);
    meta.artist      = t->artist().to8Bit(true);
    meta.album       = t->album().to8Bit(true);
    meta.genre       = t->genre().to8Bit(true);
    meta.duration    = f.audioProperties() ? f.audioProperties()->lengthInSeconds() : 0;
    meta.year        = t->year();
    meta.trackNumber = t->track();

    try {
        meta.lastModified = std::chrono::duration_cast<std::chrono::seconds>(
            fs::last_write_time(filePath).time_since_epoch()
        ).count();
    } catch (...) {
        meta.lastModified = 0;
    }

    return meta;
}

void MetadataManager::populateTrack(Track& track) {
    TrackMetadata meta = this->extractMetadata(track.getPath());

    track.setTitle(meta.title.empty()   ? "Unknown"        : meta.title);
    track.setArtist(meta.artist.empty() ? "Unknown Artist" : meta.artist);
    track.setAlbum(meta.album.empty()   ? "Unknown Album"  : meta.album);
    track.setGenre(meta.genre.empty()   ? "Unknown"        : meta.genre);
    track.setDuration(meta.duration);
    track.setYear(meta.year);
    track.setTrackNumber(meta.trackNumber);

    LOG_INFO("Track populated from metadata: %s", track.getPath().c_str());
}

std::vector<unsigned char> MetadataManager::getArtwork(const std::string& filePath) {
    // check memory cache
    auto it = this->artworkCache.find(filePath);
    if (it != this->artworkCache.end()) {
        if (fs::exists(it->second)) {
            LOG_INFO("Artwork memory cache hit for: %s", filePath.c_str());
            return this->readAllBytes(it->second);
        }
        this->artworkCache.erase(it);
    }

    // check disk cache
    std::string cachedPath = this->getCacheFilePath(filePath);
    if (fs::exists(cachedPath)) {
        LOG_INFO("Artwork disk cache hit for: %s", filePath.c_str());
        this->artworkCache[filePath] = cachedPath;
        return this->readAllBytes(cachedPath);
    }

    LOG_INFO("Artwork cache miss, extracting for: %s", filePath.c_str());

    // try embedded artwork
    std::vector<unsigned char> imageData = this->extractArtwork(filePath);
    if (!imageData.empty()) {
        if (this->writeAllBytes(cachedPath, imageData)) {
            this->artworkCache[filePath] = cachedPath;
            LOG_INFO("Cached embedded artwork to disk: %s", cachedPath.c_str());
        }
        return imageData;
    }

    // try loose artwork
    imageData = this->findLooseArtwork(filePath);
    if (!imageData.empty()) {
        if (this->writeAllBytes(cachedPath, imageData)) {
            this->artworkCache[filePath] = cachedPath;
            LOG_INFO("Cached loose artwork to disk: %s", cachedPath.c_str());
        }
        return imageData;
    }

    LOG_WARN("No artwork found for: %s", filePath.c_str());
    return imageData;
}

std::vector<unsigned char> MetadataManager::extractArtwork(const std::string& filePath) {
    std::vector<unsigned char> imageData;

    TagLib::FileRef fileRef(filePath.c_str());
    if (fileRef.isNull()) return imageData;

    auto dot = filePath.find_last_of('.');
    if (dot == std::string::npos || dot == filePath.size() - 1) return imageData;

    std::string ext = filePath.substr(dot + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(),
        [](unsigned char c) { return std::tolower(c); });

    if (ext == "mp3") {
        auto* f = dynamic_cast<TagLib::MPEG::File*>(fileRef.file());
        if (f && f->ID3v2Tag()) {
            auto frames = f->ID3v2Tag()->frameListMap()["APIC"];
            if (!frames.isEmpty()) {
                auto* frame = static_cast<TagLib::ID3v2::AttachedPictureFrame*>(frames.front());
                auto data = frame->picture();
                imageData.assign(data.begin(), data.end());
                LOG_INFO("Extracted MP3 artwork (%zu bytes)", imageData.size());
            }
        }
    } else if (ext == "flac") {
        auto* f = dynamic_cast<TagLib::FLAC::File*>(fileRef.file());
        if (f && !f->pictureList().isEmpty()) {
            auto data = f->pictureList().front()->data();
            imageData.assign(data.begin(), data.end());
            LOG_INFO("Extracted FLAC artwork (%zu bytes)", imageData.size());
        }
    } else if (ext == "mp4" || ext == "m4a") {
        auto* f = dynamic_cast<TagLib::MP4::File*>(fileRef.file());
        if (f && f->tag()) {
            auto items = f->tag()->itemMap();
            if (items.contains("covr")) {
                auto coverList = items["covr"].toCoverArtList();
                if (!coverList.isEmpty()) {
                    auto data = coverList.front().data();
                    imageData.assign(data.begin(), data.end());
                    LOG_INFO("Extracted M4A artwork (%zu bytes)", imageData.size());
                }
            }
        }
    } else if (ext == "ogg") {
        auto* f = dynamic_cast<TagLib::Ogg::Vorbis::File*>(fileRef.file());
        if (f && f->tag() && !f->tag()->pictureList().isEmpty()) {
            auto data = f->tag()->pictureList().front()->data();
            imageData.assign(data.begin(), data.end());
            LOG_INFO("Extracted OGG artwork (%zu bytes)", imageData.size());
        }
    }

    return imageData;
}

std::vector<unsigned char> MetadataManager::findLooseArtwork(const std::string& filePath) {
    static const std::vector<std::string> names = {
        "cover.jpg",  "cover.jpeg",  "cover.png",
        "folder.jpg", "folder.jpeg", "folder.png",
        "front.jpg",  "front.jpeg",  "front.png",
        "album.jpg",  "album.jpeg",  "album.png",
        "Cover.jpg",  "Cover.jpeg",  "Cover.png",
        "Folder.jpg", "Folder.jpeg", "Folder.png"
    };

    fs::path dir = fs::path(filePath).parent_path();

    for (const std::string& name : names) {
        fs::path candidate = dir / name;
        if (fs::exists(candidate) && fs::is_regular_file(candidate)) {
            auto data = this->readAllBytes(candidate);
            if (!data.empty()) {
                LOG_INFO("Found loose artwork: %s", candidate.generic_string().c_str());
                return data;
            }
        }
    }

    return {};
}

std::vector<unsigned char> MetadataManager::readAllBytes(const fs::path& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return {};

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) return {};

    return buffer;
}

bool MetadataManager::writeAllBytes(const fs::path& path, const std::vector<unsigned char>& data) {
    std::ofstream file(path, std::ios::binary);
    if (!file) return false;
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return file.good();
}

void MetadataManager::clearArtworkCache() {
    this->artworkCache.clear();

    try {
        if (fs::exists(this->cacheDir)) {
            for (const auto& entry : fs::directory_iterator(this->cacheDir))
                fs::remove(entry.path());
            LOG_INFO("Artwork cache cleared from disk.");
        }
    } catch (const std::exception& e) {
        LOG_WARN("Failed to clear disk cache: %s", e.what());
    }
}
