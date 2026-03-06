#pragma once

#include <raylib.h>
#include <string>

extern "C" {
#include "rfreetype.h"
}

// Wraps rfreetype's FontCacheFT / FontFileFT / FontFT into a single RAII
// object. One FontRenderer per font file; call loadSize() for each pixel
// size you need (e.g. 16, 24, 48). All sizes share the same glyph cache.
class FontRenderer {
    public:
        // cacheSize: how many glyphs to keep in the atlas simultaneously
        // cellWidth / cellHeight: atlas cell dimensions in pixels — set these to
        // at least the largest glyph size you expect
        FontRenderer(uint32_t cellWidth, uint32_t cellHeight, uint32_t cacheSize = 512);
        ~FontRenderer();

        // Non-copyable, movable
        FontRenderer(const FontRenderer&)            = delete;
        FontRenderer& operator=(const FontRenderer&) = delete;
        FontRenderer(FontRenderer&&)                 = default;
        FontRenderer& operator=(FontRenderer&&)      = default;

        // Load a font file from disk, call before loadSize()
        bool loadFile(const std::string& path);
        // Load a font file from memory, alternative to loadFile()
        bool loadFromMemory(const unsigned char* data, int dataSize);

        // Create a sized font at the given pixel height
        // Returns false if the file hasn't been loaded yet
        bool loadSize(unsigned int pixelHeight);
        // Switch the active size for subsequent draw calls
        bool setSize(unsigned int pixelHeight);

        // Drawing methods
        // Draws a UTF-8 string, returns the bounding box size
        Vector2 drawText(const std::string& text, int x, int y, Color tint) const;
        // Draws a single codepoint, returns advance + height
        Vector2 drawCodepoint(int codepoint, Vector2 position, Color tint) const;
        // Draws a pre-decoded codepoint array
        Vector2 drawCodepoints(const int* codepoints, int count, Vector2 position, Color tint) const;

        // Metrics
        GlyphInfoFT glyphInfo(int codepoint) const;

        bool isReady() const;

    private:
        FontCacheFT cache;
        FontFileFT  file;
        FontFT      active;         // Currently selected size
        bool        fileLoaded  = false;
        bool        cacheReady  = false;

        // Store one FontFT per loaded size so callers can switch freely
        struct SizedFont {
            unsigned int pixelHeight;
            FontFT       font;
        };

        static constexpr int MAX_SIZES = 16;
        SizedFont sizes[MAX_SIZES];
        int       sizeCount = 0;
};
