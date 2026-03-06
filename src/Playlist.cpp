#include "Playlist.hpp"
#include "Logger.hpp"

#include <fstream>
#include <sstream>
#include <filesystem>

static const std::string PLAYLIST_DIR = "playlists/";

static std::string playlistPath(const std::string& name) {
    return PLAYLIST_DIR + name + ".m3u";
}

Playlist::Playlist()
    : current(0)
{
    LOG_INFO("Playlist initialized successfully.");
}

void Playlist::append(Track* track) {
    this->tracks.push_back(track);
}

void Playlist::appendMultiple(const std::vector<Track*>& tracks) {
    for (Track* track : tracks)
        this->tracks.push_back(track);
}

bool Playlist::remove(size_t index) {
    if (index >= this->tracks.size()) {
        LOG_ERROR("Couldn't remove track; index out of bounds.");
        return false;
    }

    this->tracks.erase(this->tracks.begin() + index);

    // keep current in bounds after removal
    if (!this->tracks.empty() && this->current >= this->tracks.size())
        this->current = this->tracks.size() - 1;

    return true;
}

void Playlist::clear() {
    this->tracks.clear();
    this->current = 0;
}

bool Playlist::playCurrent(AudioPlayer& player) {
    if (this->tracks.empty()) {
        LOG_WARN("Couldn't play current track; no tracks loaded in playlist.");
        return false;
    }
    return player.playFile(this->tracks[this->current]->getPath().c_str());
}

bool Playlist::playNext(AudioPlayer& player) {
    if (this->tracks.empty()) {
        LOG_WARN("Couldn't play next track; no tracks loaded in playlist.");
        return false;
    }

    size_t next = this->hasNext() ? this->current + 1 : 0;
    if (!player.playFile(this->tracks[next]->getPath().c_str()))
        return false;

    this->current = next;
    return true;
}

bool Playlist::playPrevious(AudioPlayer& player) {
    if (this->tracks.empty()) {
        LOG_WARN("Couldn't play previous track; no tracks loaded in playlist.");
        return false;
    }

    size_t prev = this->hasPrevious() ? this->current - 1 : this->tracks.size() - 1;
    if (!player.playFile(this->tracks[prev]->getPath().c_str()))
        return false;

    this->current = prev;
    return true;
}

size_t Playlist::getCurrentIndex() const { return this->current; }

Track* Playlist::getCurrentTrack() {
    if (this->tracks.empty()) {
        LOG_ERROR("Couldn't get current track; no tracks loaded in playlist.");
        return nullptr;
    }
    return this->tracks[this->current];
}

bool Playlist::setCurrent(size_t index) {
    if (index >= this->tracks.size()) {
        LOG_ERROR("Couldn't set current track; index out of bounds.");
        return false;
    }
    this->current = index;
    return true;
}

void Playlist::resetCurrent() {
    this->current = 0;
}

size_t Playlist::count()       const { return this->tracks.size(); }
bool   Playlist::isEmpty()     const { return this->tracks.empty(); }
bool   Playlist::hasNext()     const { return this->current + 1 < this->tracks.size(); }
bool   Playlist::hasPrevious() const { return this->current > 0; }

// -----------------------------------------------------------------------------
// Persistence
// -----------------------------------------------------------------------------

bool Playlist::saveToFile(const std::string& name) const {
    std::filesystem::create_directories(PLAYLIST_DIR);

    const std::string path = playlistPath(name);
    std::ofstream file(path);
    if (!file) {
        LOG_ERROR("Couldn't open playlist file for writing: %s", path.c_str());
        return false;
    }

    file << "#EXTM3U\n";

    for (const Track* track : this->tracks) {
        // #EXTINF:<duration in seconds>,<artist> - <title>
        file << "#EXTINF:" << track->getDuration()
             << "," << track->getArtist()
             << " - " << track->getTitle()
             << "\n";
        file << track->getPath() << "\n";
    }

    LOG_INFO("Playlist '%s' saved to %s (%zu tracks)", name.c_str(), path.c_str(), this->tracks.size());
    return true;
}

std::vector<std::string> Playlist::loadFromFile(const std::string& name) {
    std::vector<std::string> paths;

    const std::string path = playlistPath(name);
    std::ifstream file(path);
    if (!file) {
        LOG_ERROR("Couldn't open playlist file for reading: %s", path.c_str());
        return paths;
    }

    std::string line;
    while (std::getline(file, line)) {
        // skip the header and EXTINF metadata lines
        if (line.empty() || line[0] == '#')
            continue;
        paths.push_back(line);
    }

    LOG_INFO("Playlist '%s' loaded from %s (%zu tracks)", name.c_str(), path.c_str(), paths.size());
    return paths;
}
