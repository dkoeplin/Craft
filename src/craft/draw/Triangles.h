#ifndef CRAFT_SRC_CRAFT_DRAW_TRIANGLES_H_
#define CRAFT_SRC_CRAFT_DRAW_TRIANGLES_H_

#include "GL/glew.h"

struct Attrib;

void draw_triangles_3d_ao(Attrib *attrib, GLuint buffer, int count);

void draw_triangles_3d_text(Attrib *attrib, GLuint buffer, int count);

void draw_triangles_3d(Attrib *attrib, GLuint buffer, int count);

void draw_triangles_2d(Attrib *attrib, GLuint buffer, int count);

#endif //CRAFT_SRC_CRAFT_DRAW_TRIANGLES_H_
