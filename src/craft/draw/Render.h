#ifndef CRAFT_SRC_CRAFT_DRAW_RENDER_H_
#define CRAFT_SRC_CRAFT_DRAW_RENDER_H_

#include "craft/draw/Shader.h"

struct Chunk;
struct Player;

struct Render {
    static void init();

    /// Textures
    static void load_textures();

    /// Shaders
    static void load_shaders();

    static Shader *block() { return &block_; }
    static Shader *line() { return &line_; }
    static Shader *text() { return &text_; }
    static Shader *sky() { return &sky_; }

  private:
    static Shader block_;
    static Shader line_;
    static Shader text_;
    static Shader sky_;
};

#endif // CRAFT_SRC_CRAFT_DRAW_RENDER_H_
