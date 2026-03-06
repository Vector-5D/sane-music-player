#include "domain/Album.hpp"
#include "Logger.hpp"

#include <algorithm>

Album::Album(const std::string& title)
    : title(title)
{
    LOG_INFO("Album created: %s", this->title.c_str());
}

const std::string& Album::getTitle()      const { return this->title; }
size_t             Album::getTrackCount() const { return this->tracks.size(); }

int Album::getTotalDuration() const {
    int total = 0;
    for (const Track* track : this->tracks)
        total += track->getDuration();

    LOG_INFO("Album '%s' total duration: %d seconds", this->title.c_str(), total);
    return total;
}

Track* Album::getTrack(size_t index) {
    if (index >= this->tracks.size()) {
        LOG_ERROR("Couldn't get track from album; index out of bounds.");
        return nullptr;
    }
    return this->tracks[index];
}

const Track* Album::getTrack(size_t index) const {
    if (index >= this->tracks.size()) {
        LOG_ERROR("Couldn't get track from album; index out of bounds.");
        return nullptr;
    }
    return this->tracks[index];
}

bool Album::addTrack(Track* track) {
    if (!track) {
        LOG_ERROR("Couldn't add track to album; track is null.");
        return false;
    }

    if (this->hasTrack(track)) {
        LOG_WARN("Track already exists in album '%s': %s", this->title.c_str(), track->getPath().c_str());
        return false;
    }

    this->tracks.push_back(track);
    LOG_INFO("Track added to album '%s': %s", this->title.c_str(), track->getPath().c_str());
    return true;
}

bool Album::removeTrack(Track* track) {
    if (!track) {
        LOG_ERROR("Couldn't remove track from album; track is null.");
        return false;
    }

    auto it = std::find(this->tracks.begin(), this->tracks.end(), track);
    if (it == this->tracks.end()) {
        LOG_WARN("Track not found in album '%s': %s", this->title.c_str(), track->getPath().c_str());
        return false;
    }

    this->tracks.erase(it);
    LOG_INFO("Track removed from album '%s': %s", this->title.c_str(), track->getPath().c_str());
    return true;
}

bool Album::hasTrack(const Track* track) const {
    if (!track) {
        LOG_ERROR("Couldn't check if album has track; track is null.");
        return false;
    }

    return std::find(this->tracks.begin(), this->tracks.end(), track) != this->tracks.end();
}
