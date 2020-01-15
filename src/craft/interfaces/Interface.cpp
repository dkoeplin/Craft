#include "Interface.h"

#include "GL/glew.h"

#include "craft/player/Player.h"
#include "craft/world/Attrib.h"
#include "craft/session/Session.h"
#include "craft/session/Window.h"
#include "craft/support/matrix.h"
#include "craft/items/item.h"

#include "craft/draw/Plant.h"
#include "craft/draw/cube.h"
#include "craft/util/util.h"

#include "craft/world/world.h"

Interface::Interface(Session *session) : session(session), window(session->window()) {}

void Interface::render_item(Attrib *attrib, World *world, Player *player) {
    float matrix[16];
    set_matrix_item(matrix, window->width(), window->height(), window->scale());
    glUseProgram(attrib->program);
    glUniformMatrix4fv(attrib->matrix, 1, GL_FALSE, matrix);
    glUniform3f(attrib->camera, 0, 0, 5);
    glUniform1i(attrib->sampler, 0);
    glUniform1f(attrib->timer, world->time_of_day());
    int w = items[player->item_index];
    if (is_plant(w)) {
        GLuint buffer = gen_plant_buffer(0, 0, 0, 0.5, w);
        draw_plant(attrib, buffer);
        del_buffer(buffer);
    }
    else {
        GLuint buffer = gen_cube_buffer(0, 0, 0, 0.5, w);
        draw_cube(attrib, buffer);
        del_buffer(buffer);
    }
}

bool Interface::is_key_pressed(const Key &key) {
    return window->is_key_pressed(key);
}

void Interface::close() {
    session->close_interface(this);
}
