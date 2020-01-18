#include "Plant.h"

#include "GL/glew.h"

#include "craft/draw/Item.h"
#include "craft/draw/Shader.h"
#include "craft/items/Item.h"
#include "craft/support/matrix.h"
#include "craft/util/Util.h"

void make_plant(float *data, float ao, float light, float px, float py, float pz, float n, int w, float rotation) {
    static const float positions[4][4][3] = {{{0, -1, -1}, {0, -1, +1}, {0, +1, -1}, {0, +1, +1}},
                                             {{0, -1, -1}, {0, -1, +1}, {0, +1, -1}, {0, +1, +1}},
                                             {{-1, -1, 0}, {-1, +1, 0}, {+1, -1, 0}, {+1, +1, 0}},
                                             {{-1, -1, 0}, {-1, +1, 0}, {+1, -1, 0}, {+1, +1, 0}}};
    static const float normals[4][3] = {{-1, 0, 0}, {+1, 0, 0}, {0, 0, -1}, {0, 0, +1}};
    static const float uvs[4][4][2] = {{{0, 0}, {1, 0}, {0, 1}, {1, 1}},
                                       {{1, 0}, {0, 0}, {1, 1}, {0, 1}},
                                       {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
                                       {{1, 0}, {1, 1}, {0, 0}, {0, 1}}};
    static const float indices[4][6] = {{0, 3, 2, 0, 1, 3}, {0, 3, 1, 0, 2, 3}, {0, 3, 2, 0, 1, 3}, {0, 3, 1, 0, 2, 3}};
    float *d = data;
    float s = 0.0625;
    float a = 0;
    float b = s;
    float du = (plants[w] % 16) * s;
    float dv = (plants[w] / 16) * s;
    for (int i = 0; i < 4; i++) {
        for (int v = 0; v < 6; v++) {
            int j = indices[i][v];
            *(d++) = n * positions[i][j][0];
            *(d++) = n * positions[i][j][1];
            *(d++) = n * positions[i][j][2];
            *(d++) = normals[i][0];
            *(d++) = normals[i][1];
            *(d++) = normals[i][2];
            *(d++) = du + (uvs[i][j][0] ? b : a);
            *(d++) = dv + (uvs[i][j][1] ? b : a);
            *(d++) = ao;
            *(d++) = light;
        }
    }
    float ma[16];
    float mb[16];
    mat_identity(ma);
    mat_rotate(mb, 0, 1, 0, RADIANS(rotation));
    mat_multiply(ma, mb, ma);
    mat_apply(data, ma, 24, 3, 10);
    mat_translate(mb, px, py, pz);
    mat_multiply(ma, mb, ma);
    mat_apply(data, ma, 24, 0, 10);
}

GLuint gen_plant_buffer(float x, float y, float z, float n, int w) {
    GLfloat *data = malloc_faces(10, 4);
    float ao = 0;
    float light = 1;
    make_plant(data, ao, light, x, y, z, n, w, 45);
    return gen_faces(10, 4, data);
}

void draw_plant(Shader *attrib, GLuint buffer) { draw_item(attrib, buffer, 24); }
