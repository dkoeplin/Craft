#ifndef CRAFT_SRC_CRAFT_WORLD_ATTRIB_H_
#define CRAFT_SRC_CRAFT_WORLD_ATTRIB_H_

#include "GL/glew.h"

struct Shader {
    GLuint program;
    GLuint position;
    GLuint normal;
    GLuint uv;
    GLuint matrix;
    GLuint sampler;
    GLuint camera;
    GLuint timer;
    GLuint extra1;
    GLuint extra2;
    GLuint extra3;
    GLuint extra4;
};

#endif // CRAFT_SRC_CRAFT_WORLD_ATTRIB_H_
