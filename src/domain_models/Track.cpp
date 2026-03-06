#include "domain/Track.hpp"
#include "Logger.hpp"

Track::Track(const std::string& path)
    : path(path)
{
    LOG_INFO("Track created: %s", this->path.c_str());
}

const std::string& Track::getPath()        const { return this->path; }
const std::string& Track::getTitle()       const { return this->title; }
const std::string& Track::getArtist()      const { return this->artist; }
const std::string& Track::getAlbum()       const { return this->album; }
const std::string& Track::getGenre()       const { return this->genre; }
int                Track::getDuration()    const { return this->duration; }
int                Track::getYear()        const { return this->year; }
int                Track::getTrackNumber() const { return this->track_number; }

void Track::setTitle(const std::string& title)   { this->title = title; }
void Track::setArtist(const std::string& artist) { this->artist = artist; }
void Track::setAlbum(const std::string& album)   { this->album = album; }
void Track::setGenre(const std::string& genre)   { this->genre = genre; }
void Track::setDuration(int duration)            { this->duration = duration; }
void Track::setYear(int year)                    { this->year = year; }
void Track::setTrackNumber(int track_number)     { this->track_number = track_number; }
