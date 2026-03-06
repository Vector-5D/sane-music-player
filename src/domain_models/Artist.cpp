#include "domain/Artist.hpp"
#include "Logger.hpp"

#include <algorithm>

Artist::Artist(const std::string& name)
    : name(name)
{
    LOG_INFO("Artist created: %s", this->name.c_str());
}

const std::string& Artist::getName()       const { return this->name; }
size_t             Artist::getAlbumCount() const { return this->albums.size(); }

Album* Artist::getAlbum(size_t index) {
    if (index >= this->albums.size()) {
        LOG_ERROR("Couldn't get album from artist; index out of bounds.");
        return nullptr;
    }
    return this->albums[index];
}

const Album* Artist::getAlbum(size_t index) const {
    if (index >= this->albums.size()) {
        LOG_ERROR("Couldn't get album from artist; index out of bounds.");
        return nullptr;
    }
    return this->albums[index];
}

bool Artist::addAlbum(Album* album) {
    if (!album) {
        LOG_ERROR("Couldn't add album to artist; album is null.");
        return false;
    }

    if (this->hasAlbum(album)) {
        LOG_WARN("Album already exists for artist '%s': %s", this->name.c_str(), album->getTitle().c_str());
        return false;
    }

    this->albums.push_back(album);
    LOG_INFO("Album added to artist '%s': %s", this->name.c_str(), album->getTitle().c_str());
    return true;
}

bool Artist::removeAlbum(Album* album) {
    if (!album) {
        LOG_ERROR("Couldn't remove album from artist; album is null.");
        return false;
    }

    auto it = std::find(this->albums.begin(), this->albums.end(), album);
    if (it == this->albums.end()) {
        LOG_WARN("Album not found for artist '%s': %s", this->name.c_str(), album->getTitle().c_str());
        return false;
    }

    this->albums.erase(it);
    LOG_INFO("Album removed from artist '%s': %s", this->name.c_str(), album->getTitle().c_str());
    return true;
}

bool Artist::hasAlbum(const Album* album) const {
    if (!album) {
        LOG_ERROR("Couldn't check if artist has album; album is null.");
        return false;
    }

    return std::find(this->albums.begin(), this->albums.end(), album) != this->albums.end();
}
