#pragma once

#include "domain/Track.hpp"
#include "AudioPlayer.hpp"
#include <vector>
#include <string>

class Playlist {
    public:
        Playlist();

        // Track management
        void append(Track* track);
        void appendMultiple(const std::vector<Track*>& tracks);
        bool remove(size_t index);
        void clear();

        // Playback
        bool playCurrent(AudioPlayer& player);
        bool playNext(AudioPlayer& player);
        bool playPrevious(AudioPlayer& player);

        // Current track cursor
        size_t getCurrentIndex() const;
        Track* getCurrentTrack();
        bool  setCurrent(size_t index);
        void  resetCurrent();

        // State queries
        size_t count()       const;
        bool   isEmpty()     const;
        bool   hasNext()     const;
        bool   hasPrevious() const;

        // Saves the playlist to playlists/<name>.m3u
        bool saveToFile(const std::string& name) const;
        // Loads track paths from playlists/<name>.m3u — returns paths only so the
        // Caller can check them against the central library
        static std::vector<std::string> loadFromFile(const std::string& name);

    private:
        std::vector<Track*> tracks;      // tracks are non-owning
        size_t              current = 0;
};
