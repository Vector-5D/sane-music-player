#pragma once

#include "Album.hpp"
#include <string>
#include <vector>

// Artist stores non-owning pointers to Albums held in a central collection
class Artist {
    public:
        explicit Artist(const std::string& name);

        // Getters
        const std::string& getName()       const;
        size_t             getAlbumCount() const;

        // Album pointer access
        Album*       getAlbum(size_t index);
        const Album* getAlbum(size_t index) const;

        // Album pointer management
        bool addAlbum(Album* album);
        bool removeAlbum(Album* album);
        bool hasAlbum(const Album* album) const;

    private:
        std::string         name;
        std::vector<Album*> albums; // non-owning
};
