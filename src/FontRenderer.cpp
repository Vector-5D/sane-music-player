#include "FontRenderer.hpp"
#include "Logger.hpp"

FontRenderer::FontRenderer(uint32_t cellWidth, uint32_t cellHeight, uint32_t cacheSize) {
    this->cache = LoadFontCacheFT(cellWidth, cellHeight, cacheSize);
    if (this->cache.cache) {
        this->cacheReady = true;
        LOG_INFO("FontRenderer cache created (%ux%u, %u slots).", cellWidth, cellHeight, cacheSize);
    } else {
        LOG_ERROR("FontRenderer failed to create glyph cache.");
    }

    this->file   = { nullptr };
    this->active = { 0, 0, nullptr };
}

FontRenderer::~FontRenderer() {
    for (int i = 0; i < this->sizeCount; i++)
        UnloadFontFT(this->sizes[i].font);

    if (this->fileLoaded)
        UnloadFontFileFT(this->file);

    if (this->cacheReady)
        UnloadFontCacheFT(this->cache);

    LOG_INFO("FontRenderer unloaded.");
}

bool FontRenderer::loadFile(const std::string& path) {
    if (!this->cacheReady) {
        LOG_ERROR("FontRenderer: can't load file — cache not ready.");
        return false;
    }

    if (this->fileLoaded)
        UnloadFontFileFT(this->file);

    this->file       = LoadFontFileFT(path.c_str());
    this->fileLoaded = (this->file.face != nullptr);

    if (this->fileLoaded) {
        LOG_INFO("FontRenderer loaded font file: %s", path.c_str());
    } else {
        LOG_ERROR("FontRenderer failed to load font file: %s", path.c_str());
    }

    return this->fileLoaded;
}

bool FontRenderer::loadFromMemory(const unsigned char* data, int dataSize) {
    if (!this->cacheReady) {
        LOG_ERROR("FontRenderer: can't load from memory — cache not ready.");
        return false;
    }

    if (this->fileLoaded)
        UnloadFontFileFT(this->file);

    this->file       = LoadFontFileFromMemoryFT(data, dataSize);
    this->fileLoaded = (this->file.face != nullptr);

    if (this->fileLoaded) {
        LOG_INFO("FontRenderer loaded font from memory (%d bytes).", dataSize);
    } else {
        LOG_ERROR("FontRenderer failed to load font from memory.");
    }

    return this->fileLoaded;
}

bool FontRenderer::loadSize(unsigned int pixelHeight) {
    if (!this->fileLoaded) {
        LOG_ERROR("FontRenderer: can't load size %u — no font file loaded.", pixelHeight);
        return false;
    }

    // don't load the same size twice
    for (int i = 0; i < this->sizeCount; i++) {
        if (this->sizes[i].pixelHeight == pixelHeight) {
            this->active = this->sizes[i].font;
            return true;
        }
    }

    if (this->sizeCount >= MAX_SIZES) {
        LOG_ERROR("FontRenderer: reached max size limit (%d).", MAX_SIZES);
        return false;
    }

    FontFT font = LoadFontFT(this->file, pixelHeight, this->cache);
    if (!font.size) {
        LOG_ERROR("FontRenderer failed to load size %u.", pixelHeight);
        return false;
    }

    this->sizes[this->sizeCount++] = { pixelHeight, font };
    this->active = font;

    LOG_INFO("FontRenderer loaded size %u.", pixelHeight);
    return true;
}

bool FontRenderer::setSize(unsigned int pixelHeight) {
    for (int i = 0; i < this->sizeCount; i++) {
        if (this->sizes[i].pixelHeight == pixelHeight) {
            this->active = this->sizes[i].font;
            return true;
        }
    }

    LOG_WARN("FontRenderer: size %u not loaded — call loadSize() first.", pixelHeight);
    return false;
}

Vector2 FontRenderer::drawText(const std::string& text, int x, int y, Color tint) const {
    return DrawTextFT(this->active, text.c_str(), x, y, tint);
}

Vector2 FontRenderer::drawCodepoint(int codepoint, Vector2 position, Color tint) const {
    return DrawTextCodepointFT(this->active, codepoint, position, tint);
}

Vector2 FontRenderer::drawCodepoints(const int* codepoints, int count, Vector2 position, Color tint) const {
    return DrawTextCodepointsFT(this->active, codepoints, count, position, tint);
}

GlyphInfoFT FontRenderer::glyphInfo(int codepoint) const {
    return GetGlyphInfoFT(this->active, codepoint);
}

bool FontRenderer::isReady() const {
    return this->cacheReady && this->fileLoaded && (this->active.size != nullptr);
}
