#include "Text.h"

#include <cstring>

#include "GL/glew.h"

#include "craft/draw/Character.h"
#include "craft/draw/Shader.h"
#include "craft/draw/Triangles.h"
#include "craft/session/Window.h"
#include "craft/support/matrix.h"
#include "craft/util/Util.h"

GLuint gen_text_buffer(float x, float y, float n, char *text) {
    int length = strlen(text);
    GLfloat *data = malloc_faces(4, length);
    for (int i = 0; i < length; i++) {
        make_character(data + i * 24, x, y, n / 2, n, text[i]);
        x += n;
    }
    return gen_faces(4, length, data);
}

void draw_text(Shader *attrib, GLuint buffer, int length) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    draw_triangles_2d(attrib, buffer, length * 6);
    glDisable(GL_BLEND);
}

void render_text(Window *window, Shader *attrib, Justify justify, float x, float y, float n, char *text) {
    float matrix[16];
    set_matrix_2d(matrix, window->width(), window->height());
    glUseProgram(attrib->program);
    glUniformMatrix4fv(attrib->matrix, 1, GL_FALSE, matrix);
    glUniform1i(attrib->sampler, 1);
    glUniform1i(attrib->extra1, 0);
    int length = strlen(text);
    x -= n * static_cast<int>(justify) * (length - 1) / 2;
    GLuint buffer = gen_text_buffer(x, y, n, text);
    draw_text(attrib, buffer, length);
    del_buffer(buffer);
}
