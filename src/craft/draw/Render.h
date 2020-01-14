#ifndef CRAFT_SRC_CRAFT_DRAW_RENDER_H_
#define CRAFT_SRC_CRAFT_DRAW_RENDER_H_

#include "craft/world/Attrib.h"

struct Chunk;
struct Player;

struct Render {
  static void init();

  /// Textures
  static void load_textures();

  /// Shaders
  static void load_shaders();

  static Attrib *block() { return &block_; }
  static Attrib *line() { return &line_; }
  static Attrib *text() { return &text_; }
  static Attrib *sky() { return &sky_; }

 private:
  static Attrib block_;
  static Attrib line_;
  static Attrib text_;
  static Attrib sky_;
};

#endif //CRAFT_SRC_CRAFT_DRAW_RENDER_H_
