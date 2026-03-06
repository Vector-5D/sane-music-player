#pragma once

#include "miniaudio.h"

class AudioPlayer {
    public:
        AudioPlayer();
        ~AudioPlayer();

        // Playback control
        bool playFile(const char* path);
        bool stop();
        bool pause();
        bool resume();

        // Volume control
        bool  setVolume(float volume);
        float getVolume();

        // Progress control
        bool  setProgress(float progress);
        float getProgress();

        // Duration and position
        float getDurationSeconds();
        float getPositionSeconds();

        // State queries
        bool isPlaying();
        bool isPaused();
        bool isFinished();

    private:
        ma_engine engine;
        ma_sound  sound;
        bool      initialized;
        bool      sound_loaded;
        bool      paused;
};
