#ifndef CRAFT_SRC_CRAFT_DRAW_LINES_H_
#define CRAFT_SRC_CRAFT_DRAW_LINES_H_

#include "GL/glew.h"

struct Shader;

void draw_lines(Shader *attrib, GLuint buffer, int components, int count);

#endif //CRAFT_SRC_CRAFT_DRAW_LINES_H_
