#include "Item.h"

#include "GL/glew.h"

#include "craft/draw/Shader.h"
#include "craft/draw/Triangles.h"

void draw_item(Shader *attrib, GLuint buffer, int count) { draw_triangles_3d_ao(attrib, buffer, count); }
