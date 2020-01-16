#include "craft/draw/Crosshairs.h"

#include "GL/glew.h"

#include "craft/draw/Lines.h"
#include "craft/session/Window.h"
#include "craft/support/matrix.h"
#include "craft/util/Util.h"
#include "craft/draw/Shader.h"

GLuint gen_crosshair_buffer(Window *window) {
    float x = window->width() / 2;
    float y = window->height() / 2;
    float p = 10 * window->scale();
    float data[] = {
            x, y - p, x, y + p,
            x - p, y, x + p, y
    };
    return gen_buffer(sizeof(data), data);
}

void render_crosshairs(Window *window, Shader *attrib) {
    float matrix[16];
    set_matrix_2d(matrix, window->width(), window->height());
    glUseProgram(attrib->program);
    glLineWidth(4 * window->scale());
    glEnable(GL_COLOR_LOGIC_OP);
    glUniformMatrix4fv(attrib->matrix, 1, GL_FALSE, matrix);
    GLuint crosshair_buffer = gen_crosshair_buffer(window);
    draw_lines(attrib, crosshair_buffer, 2, 4);
    del_buffer(crosshair_buffer);
    glDisable(GL_COLOR_LOGIC_OP);
}


