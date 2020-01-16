#ifndef CRAFT_SRC_CRAFT_DRAW_TRIANGLES_H_
#define CRAFT_SRC_CRAFT_DRAW_TRIANGLES_H_

#include "GL/glew.h"

struct Shader;

void draw_triangles_3d_ao(Shader *attrib, GLuint buffer, int count);

void draw_triangles_3d_text(Shader *attrib, GLuint buffer, int count);

void draw_triangles_3d(Shader *attrib, GLuint buffer, int count);

void draw_triangles_2d(Shader *attrib, GLuint buffer, int count);

#endif //CRAFT_SRC_CRAFT_DRAW_TRIANGLES_H_
