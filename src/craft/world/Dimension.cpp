#include "Dimension.h"

#include "craft/draw/Sphere.h"
#include "craft/util/util.h"

GLuint gen_sky_buffer() {
    float data[12288];
    make_sphere(data, 1, 3);
    return gen_buffer(sizeof(data), data);
}

Dimension::~Dimension() {
    del_buffer(sky);
}

Earth::Earth() : Dimension() {
    sky = gen_sky_buffer();
}
