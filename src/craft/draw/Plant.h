#ifndef CRAFT_SRC_CRAFT_DRAW_PLANT_H_
#define CRAFT_SRC_CRAFT_DRAW_PLANT_H_

#include "GL/glew.h"

struct Shader;

void make_plant(float *data, float ao, float light, float px, float py, float pz, float n, int w, float rotation);

GLuint gen_plant_buffer(float x, float y, float z, float n, int w);

void draw_plant(Shader *attrib, GLuint buffer);

#endif // CRAFT_SRC_CRAFT_DRAW_PLANT_H_
