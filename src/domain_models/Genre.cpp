#include "domain/Genre.hpp"
#include "Logger.hpp"

#include <algorithm>

Genre::Genre(const std::string& name)
    : name(name)
{
    LOG_INFO("Genre created: %s", this->name.c_str());
}

const std::string& Genre::getName()       const { return this->name; }
size_t             Genre::getAlbumCount() const { return this->albums.size(); }

Album* Genre::getAlbum(size_t index) {
    if (index >= this->albums.size()) {
        LOG_ERROR("Couldn't get album from genre; index out of bounds.");
        return nullptr;
    }
    return this->albums[index];
}

const Album* Genre::getAlbum(size_t index) const {
    if (index >= this->albums.size()) {
        LOG_ERROR("Couldn't get album from genre; index out of bounds.");
        return nullptr;
    }
    return this->albums[index];
}

bool Genre::addAlbum(Album* album) {
    if (!album) {
        LOG_ERROR("Couldn't add album to genre; album is null.");
        return false;
    }

    if (this->hasAlbum(album)) {
        LOG_WARN("Album already exists in genre '%s': %s", this->name.c_str(), album->getTitle().c_str());
        return false;
    }

    this->albums.push_back(album);
    LOG_INFO("Album added to genre '%s': %s", this->name.c_str(), album->getTitle().c_str());
    return true;
}

bool Genre::removeAlbum(Album* album) {
    if (!album) {
        LOG_ERROR("Couldn't remove album from genre; album is null.");
        return false;
    }

    auto it = std::find(this->albums.begin(), this->albums.end(), album);
    if (it == this->albums.end()) {
        LOG_WARN("Album not found in genre '%s': %s", this->name.c_str(), album->getTitle().c_str());
        return false;
    }

    this->albums.erase(it);
    LOG_INFO("Album removed from genre '%s': %s", this->name.c_str(), album->getTitle().c_str());
    return true;
}

bool Genre::hasAlbum(const Album* album) const {
    if (!album) {
        LOG_ERROR("Couldn't check if genre has album; album is null.");
        return false;
    }

    return std::find(this->albums.begin(), this->albums.end(), album) != this->albums.end();
}
