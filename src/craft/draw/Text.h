#ifndef CRAFT_SRC_CRAFT_DRAW_TEXT_H_
#define CRAFT_SRC_CRAFT_DRAW_TEXT_H_

struct Attrib;
struct Window;

enum class Justify {
  Left = 0,
  Center = 1,
  Right = 2
};

void render_text(Window *window, Attrib *attrib, Justify justify, float x, float y, float n, char *text);

#endif //CRAFT_SRC_CRAFT_DRAW_TEXT_H_
