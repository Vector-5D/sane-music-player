#define MINIAUDIO_IMPLEMENTATION
#include "AudioPlayer.hpp"
#include "Logger.hpp"

// Helper function for clamping into a 0.0f to 1.0f range
static inline float clamp01(float x) {
    if (x < 0.0f) return 0.0f;
    if (x > 1.0f) return 1.0f;
    return x;
}

AudioPlayer::AudioPlayer() {
    ma_result result = ma_engine_init(nullptr, &this->engine);

    if (result != MA_SUCCESS) {
        LOG_ERROR(
            "Failed to initialize audio device: %s",
            ma_result_description(result)
        );
        std::exit(1);
    }

    this->initialized = true;
    this->sound_loaded = false;
    this->paused = false;

    LOG_INFO("Audio device initialized successfully");
}

AudioPlayer::~AudioPlayer() {
    if (this->sound_loaded) ma_sound_uninit(&this->sound);
    ma_engine_uninit(&this->engine);
    this->initialized = false;
    this->sound_loaded = false;
    this->paused = false;
    LOG_INFO("Audio device uninitialized successfully.");
}

bool AudioPlayer::playFile(const char* path) {
    if (!path) {
        LOG_ERROR("Failed to play file: path is NULL");
        return false;
    }

    float volume;

    if (this->sound_loaded) {
        volume = getVolume();
        ma_sound_uninit(&this->sound);
    } else {
        volume = 1.0f;
    }

    ma_result result = ma_sound_init_from_file(
        &this->engine,
        path,
        MA_SOUND_FLAG_STREAM,
        nullptr,
        nullptr,
        &this->sound
    );

    if (result != MA_SUCCESS) {
        LOG_ERROR("Failed to load sound: %s", ma_result_description(result));
        this->sound_loaded = false;
        this->paused = false;
        return false;
    }

    this->sound_loaded = true;

    result = ma_sound_start(&this->sound);

    if (result != MA_SUCCESS) {
        LOG_ERROR("Failed to start sound: %s", ma_result_description(result));
        ma_sound_uninit(&this->sound);
        this->sound_loaded = false;
        this->paused = false;
        return false;
    }

    setVolume(volume);

    LOG_INFO("Playing file: %s", path);
    return true;
}

bool AudioPlayer::stop() {
    if (!this->sound_loaded) {
        LOG_WARN("Couldn't stop: no sound is loaded");
        return false;
    }

    ma_sound_stop(&this->sound);
    ma_sound_uninit(&this->sound);

    this->sound_loaded = false;
    this->paused = false;

    LOG_INFO("Playback stopped");
    return true;
}

bool AudioPlayer::pause() {
    if (!this->sound_loaded) {
        LOG_WARN("Couldn't pause: no sound is loaded");
        return false;
    }

    if (this->paused) {
        LOG_WARN("Couldn't pause: already paused");
        return false;
    }

    ma_sound_stop(&this->sound);
    this->paused = true;

    LOG_INFO("Playback paused");
    return true;
}

bool AudioPlayer::resume() {
    if (!this->sound_loaded) {
        LOG_WARN("Couldn't resume: no sound is loaded");
        return false;
    }

    if (!this->paused) {
        LOG_WARN("Couldn't resume: sound isn't paused");
        return false;
    }

    ma_result result = ma_sound_start(&this->sound);

    if (result != MA_SUCCESS) {
        LOG_ERROR("Failed to resume sound: %s", ma_result_description(result));
        return false;
    }

    this->paused = false;
    LOG_INFO("Playback resumed");
    return true;
}

bool AudioPlayer::setVolume(float volume) {
    if (!this->sound_loaded) {
        LOG_WARN("Couldn't set volume: no sound is loaded");
        return false;
    }

    volume = clamp01(volume);
    ma_sound_set_volume(&this->sound, volume);

    LOG_INFO("Volume set to %.2f", volume);
    return true;
}

float AudioPlayer::getVolume() {
    if (!this->sound_loaded) return 0.0f;
    return ma_sound_get_volume(&this->sound);
}

bool AudioPlayer::setProgress(float progress) {
    if (!this->sound_loaded) {
        LOG_WARN("Couldn't set progress: no sound is loaded");
        return false;
    }

    progress = clamp01(progress);

    ma_uint64 length;
    ma_result result = ma_sound_get_length_in_pcm_frames(&this->sound, &length);

    if (result != MA_SUCCESS) {
        LOG_ERROR("Failed to get sound length: %s", ma_result_description(result));
        return false;
    }

    ma_uint64 target = (ma_uint64)(progress * length);

    result = ma_sound_seek_to_pcm_frame(&this->sound, target);

    if (result != MA_SUCCESS) {
        LOG_ERROR("Failed to seek: %s", ma_result_description(result));
        return false;
    }

    LOG_INFO("Seeked to progress %.2f%%", progress * 100.0f);
    return true;
}

float AudioPlayer::getProgress() {
    if (!this->sound_loaded) return 0.0f;

    ma_uint64 cursor, length;

    if (ma_sound_get_cursor_in_pcm_frames(&this->sound, &cursor) != MA_SUCCESS ||
        ma_sound_get_length_in_pcm_frames(&this->sound, &length) != MA_SUCCESS ||
        length == 0) {
        return 0.0f;
    }

    return (float)cursor / (float)length;
}

float AudioPlayer::getDurationSeconds() {
    if (!this->sound_loaded) return 0.0f;

    ma_uint64 frames;

    if (ma_sound_get_length_in_pcm_frames(&this->sound, &frames) != MA_SUCCESS)
        return 0.0f;

    return (float)frames / ma_engine_get_sample_rate(&this->engine);
}

float AudioPlayer::getPositionSeconds() {
    if (!this->sound_loaded) return 0.0f;

    ma_uint64 frames;

    if (ma_sound_get_cursor_in_pcm_frames(&this->sound, &frames) != MA_SUCCESS)
        return 0.0f;

    return (float)frames / ma_engine_get_sample_rate(&this->engine);
}

bool AudioPlayer::isPlaying() {
    return this->sound_loaded && ma_sound_is_playing(&this->sound);
}

bool AudioPlayer::isPaused() {
    return this->paused;
}

bool AudioPlayer::isFinished() {
    if (!this->sound_loaded) return false;
    return ma_sound_at_end(&this->sound);
}
