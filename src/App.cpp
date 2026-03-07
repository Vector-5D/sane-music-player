#include "App.hpp"
#include "FileDialog.hpp"
#include "Logger.hpp"
#include "MetadataManager.hpp"
#include <raylib.h>

App::App() : w_width(800), w_height(450)
{
    InitWindow(this->w_width, this->w_height, "Sane Music Player");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(144);
    Image icon = LoadImage("assets/icons/icon4.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    this->fontRenderer.emplace(48, 48, 512);
    this->fontRenderer->loadFile("assets/fonts/NotoSansJP-Medium.ttf");
    this->fontRenderer->loadSize(16);
    this->fontRenderer->loadSize(24);

    getCurrentArtwork("", true);

    LOG_INFO("App initialized successfully.");
}

App::~App() {
    CloseWindow();
    UnloadTexture(this->current_artwork);
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
        // TODO: Implement folder scan part after Library exists
    } else if (IsKeyDown(KEY_LEFT_CONTROL) &&
               IsKeyPressed(KEY_O)) {
        auto result = FileDialog::openFile("mp3,flac,wav,ogg,m4a");
        if (result) {
            // TODO: once Library exists, check track from there instead
            Track* track = new Track(*result);
            getCurrentArtwork(track->getPath());
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

    this->fontRenderer->loadSize(22);
    this->fontRenderer->drawText("Genres", 20, 15, LIGHTGRAY);

    Track* track = this->playlist.getCurrentTrack();
    if (track) {
        DrawTextureEx(this->current_artwork, { 20.f, float(this->w_height - 276) }, 0.f, 0.5, WHITE);

        this->fontRenderer->setSize(24);
        this->fontRenderer->drawText(
            track->getTitle(), 317, w_height - 92, WHITE);

        this->fontRenderer->setSize(16);
        this->fontRenderer->drawText(
            track->getArtist(), 317, w_height - 63, GRAY);
        this->fontRenderer->drawText(
            track->getAlbum(), 317, w_height - 45, DARKGRAY);
    } else {
        DrawTextureEx(this->current_artwork, { 20.f, float(this->w_height - 276) }, 0.f, 0.5, WHITE);

        this->fontRenderer->setSize(24);
        this->fontRenderer->drawText(
            "No song currently playing.", 317, w_height - 74, WHITE);

        this->fontRenderer->setSize(16);
        this->fontRenderer->drawText(
            "Select a song using CTRL+O.", 317, w_height - 45, GRAY);
    }

    DrawLine(0, this->w_height - 296, 296, this->w_height - 296, DARKGRAY);
    DrawLine(296, 0, 296, this->w_height, DARKGRAY);

    EndDrawing();
}

void App::getCurrentArtwork(std::string path, bool init) {
    if (init) { // use the fallback cover on startup
        Image img = LoadImage("assets/icons/fallback.png");
        ImageResize(&img, 512, 512);
        Texture2D tex = LoadTextureFromImage(img);
        SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
        this->current_artwork = tex;
        UnloadImage(img);
        return;
    }

    std::vector<unsigned char> data = metadata.getArtwork(path);
    std::string extension = detextImageExt(data);

    Image img;
    if (extension == ".jpg" || extension == ".png") {
        img = LoadImageFromMemory(extension.c_str(), data.data(), data.size());
        if (img.data == nullptr) {
            img = LoadImage("assets/icons/fallback.png");
        }
    } else {
        img = LoadImage("assets/icons/fallback.png"); // Fallback icon
    };

    ImageResize(&img, 512, 512);
    Texture2D tex = LoadTextureFromImage(img);
    SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
    this->current_artwork = tex;
    UnloadImage(img);
}

const char* App::detextImageExt(const std::vector<unsigned char>& data) {
    if (data.size() < 4) return nullptr;
    if (data[0] == 0xFF && data[1] == 0xD8 && data[2] == 0xFF) return ".jpg";
    if (data[0] == 0x89 && data[1] == 0x50 && data[2] == 0x4E && data[3] == 0x47) return ".png";
    return ".jpg"; // Fallback extension
}
