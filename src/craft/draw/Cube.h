#ifndef _cube_h_
#define _cube_h_

#include "GL/glew.h"

struct Shader;
struct Player;
struct World;

void make_cube_faces(float *data, float ao[6][4], float light[6][4], int left, int right, int top, int bottom,
                     int front, int back, int wleft, int wright, int wtop, int wbottom, int wfront, int wback, float x,
                     float y, float z, float n);

void make_cube(float *data, float ao[6][4], float light[6][4], int left, int right, int top, int bottom, int front,
               int back, float x, float y, float z, float n, int w);

GLuint gen_cube_buffer(float x, float y, float z, float n, int w);

GLuint gen_wireframe_buffer(float x, float y, float z, float n);

void make_cube_wireframe(float *data, float x, float y, float z, float n);

void draw_cube(Shader *attrib, GLuint buffer);

void render_wireframe(World *world, Shader *attrib, Player *p, int w, int h);

#endif
