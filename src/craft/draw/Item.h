#ifndef CRAFT_SRC_CRAFT_DRAW_ITEM_H_
#define CRAFT_SRC_CRAFT_DRAW_ITEM_H_

#include "GL/glew.h"

struct Shader;

void draw_item(Shader *attrib, GLuint buffer, int count);

#endif //CRAFT_SRC_CRAFT_DRAW_ITEM_H_
