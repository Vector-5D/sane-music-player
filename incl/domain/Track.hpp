#pragma once

#include <string>

class Track {
    public:
        explicit Track(const std::string& path);

        // Getters
        const std::string& getPath()        const;
        const std::string& getTitle()       const;
        const std::string& getArtist()      const;
        const std::string& getAlbum()       const;
        const std::string& getGenre()       const;
        int                getDuration()    const;
        int                getYear()        const;
        int                getTrackNumber() const;

        // Setters
        void setTitle(const std::string& title);
        void setArtist(const std::string& artist);
        void setAlbum(const std::string& album);
        void setGenre(const std::string& genre);
        void setDuration(int duration);
        void setYear(int year);
        void setTrackNumber(int track_number);

    private:
        std::string path;
        std::string title        = "Unknown";
        std::string artist       = "Unknown Artist";
        std::string album        = "Unknown Album";
        std::string genre        = "Unknown";
        int         duration     = 0;
        int         year         = 0;
        int         track_number = 0;
};
