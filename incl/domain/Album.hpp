#pragma once

#include "Track.hpp"
#include <string>
#include <vector>

// Album stores non-owning pointers to Tracks held in a central collection
class Album {
    public:
        explicit Album(const std::string& title);

        // Getters
        const std::string& getTitle()         const;
        size_t             getTrackCount()    const;
        int                getTotalDuration() const;

        // Track pointer access
        Track*       getTrack(size_t index);
        const Track* getTrack(size_t index) const;

        // Track pointer management
        bool addTrack(Track* track);
        bool removeTrack(Track* track);
        bool hasTrack(const Track* track) const;

    private:
        std::string         title;
        std::vector<Track*> tracks;  // non-owning
};
