#include "Item.h"

#include "GL/glew.h"

#include "craft/draw/Triangles.h"
#include "craft/world/Attrib.h"

void draw_item(Attrib *attrib, GLuint buffer, int count) {
    draw_triangles_3d_ao(attrib, buffer, count);
}
