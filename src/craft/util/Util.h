#ifndef _util_h_
#define _util_h_

#include <string>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "config.h"

#define PI 3.14159265359
#define DEGREES(radians) ((radians)*180 / PI)
#define RADIANS(degrees) ((degrees)*PI / 180)
#define ABS(x) ((x) < 0 ? (-(x)) : (x))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SIGN(x) (((x) > 0) - ((x) < 0))

#define ABS_MIN(a, b) (ABS(a) < ABS(b)) ? a : b

#if DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

int rand_int(int n);
double rand_double();

GLuint gen_buffer(GLsizei size, GLfloat *data);
void del_buffer(GLuint buffer);
GLfloat *malloc_faces(int components, int faces);
GLuint gen_faces(int components, int faces, GLfloat *data);
GLuint make_shader(GLenum type, const char *source);
GLuint load_shader(GLenum type, const char *path);
GLuint make_program(GLuint shader1, GLuint shader2);
GLuint load_program(const std::string &path1, const std::string &path2);
void load_png_texture(const std::string &file_name);
char *tokenize(char *str, const char *delim, char **key);
int char_width(char input);
int string_width(const char *input);
int wrap(const char *input, int max_width, char *output, int max_length);

#endif
