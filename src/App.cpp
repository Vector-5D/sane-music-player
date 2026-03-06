#include "App.hpp"
#include "FileDialog.hpp"
#include "Logger.hpp"
#include <raylib.h>

App::App() : w_width(800), w_height(450)
{
    InitWindow(this->w_width, this->w_height, "Sane Music Player");
    SetTargetFPS(144);
    Image icon = LoadImage("assets/icons/icon4.png");
    SetWindowIcon(icon);
    UnloadImage(icon);
    LOG_INFO("App initialized successfully.");
}

App::~App() {
    CloseWindow();
    LOG_INFO("App deinitialized successfully.");
}

void App::run() {
    while (!WindowShouldClose()) {
        this->handleInput();
        this->update();
        this->render();
    }
}

void App::handleInput() {
    // Playback controls
    if (IsKeyPressed(KEY_LEFT))
        this->playlist.playPrevious(this->player);
    if (IsKeyPressed(KEY_RIGHT))
        this->playlist.playNext(this->player);

    if (IsKeyPressed(KEY_SPACE)) {
        if (this->player.isPaused())
            this->player.resume();
        else
            this->player.pause();
    }

    // Volume controls
    if (IsKeyPressed(KEY_DOWN)) {
        float v = this->player.getVolume();
        this->player.setVolume(v - 0.05f <= 0.0f ? 0.0f : v - 0.05f);
    }
    if (IsKeyPressed(KEY_UP)) {
        float v = this->player.getVolume();
        this->player.setVolume(v + 0.05f >= 1.0f ? 1.0f : v + 0.05f);
    }

    // File IO
    if (IsKeyDown(KEY_LEFT_CONTROL) &&
        IsKeyDown(KEY_LEFT_SHIFT) &&
        IsKeyPressed(KEY_O)) {
        // TO-DO: Implement folder scan part after Library exists
    } else if (IsKeyDown(KEY_LEFT_CONTROL) &&
               IsKeyPressed(KEY_O)) {
        auto result = FileDialog::openFile("mp3,flac,wav,ogg,m4a");
        if (result) {
            // TODO: once Library exists, check track from there instead
            Track* track = new Track(*result);
            this->metadata.populateTrack(*track);
            this->playlist.clear();
            this->playlist.append(track);
            this->playlist.playCurrent(this->player);
        }
    }
}

void App::update() {
    this->w_width  = GetScreenWidth();
    this->w_height = GetScreenHeight();

    if (this->player.isFinished())
        this->playlist.playNext(this->player);
}

void App::render() {
    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();
}
