#include "Player.h"

#include "craft/draw/cube.h"
#include "craft/support/matrix.h"

void update_player(Player *player, float x, float y, float z, float rx, float ry, int interpolate) {
    if (interpolate) {
        State *s1 = &player->state1;
        State *s2 = &player->state2;
        memcpy(s1, s2, sizeof(State));
        s2->x = x; s2->y = y; s2->z = z; s2->rx = rx; s2->ry = ry;
        s2->t = glfwGetTime();
        if (s2->rx - s1->rx > PI) {
            s1->rx += 2 * PI;
        }
        if (s1->rx - s2->rx > PI) {
            s1->rx -= 2 * PI;
        }
    }
    else {
        State &s = player->state;
        s.x = x; s.y = y; s.z = z; s.rx = rx; s.ry = ry;
        del_buffer(player->buffer);
        player->buffer = gen_player_buffer(s);
    }
}

void interpolate_player(Player *player) {
    State *s1 = &player->state1;
    State *s2 = &player->state2;
    float t1 = s2->t - s1->t;
    float t2 = glfwGetTime() - s2->t;
    t1 = MIN(t1, 1);
    t1 = MAX(t1, 0.1);
    float p = MIN(t2 / t1, 1);
    update_player(
            player,
            s1->x + (s2->x - s1->x) * p,
            s1->y + (s2->y - s1->y) * p,
            s1->z + (s2->z - s1->z) * p,
            s1->rx + (s2->rx - s1->rx) * p,
            s1->ry + (s2->ry - s1->ry) * p,
            0);
}

GLuint gen_player_buffer(const State &state) {
    GLfloat *data = malloc_faces(10, 6);
    make_player(data, state.x, state.y, state.z, state.rx, state.ry);
    return gen_faces(10, 6, data);
}

void make_player(float *data, float x, float y, float z, float rx, float ry) {
    float ao[6][4] = {{0}};
    float light[6][4] = {
            {0.8, 0.8, 0.8, 0.8},
            {0.8, 0.8, 0.8, 0.8},
            {0.8, 0.8, 0.8, 0.8},
            {0.8, 0.8, 0.8, 0.8},
            {0.8, 0.8, 0.8, 0.8},
            {0.8, 0.8, 0.8, 0.8}
    };
    make_cube_faces(
            data, ao, light,
            1, 1, 1, 1, 1, 1,
            226, 224, 241, 209, 225, 227,
            0, 0, 0, 0.4);
    float ma[16];
    float mb[16];
    mat_identity(ma);
    mat_rotate(mb, 0, 1, 0, rx);
    mat_multiply(ma, mb, ma);
    mat_rotate(mb, cosf(rx), 0, sinf(rx), -ry);
    mat_multiply(ma, mb, ma);
    mat_apply(data, ma, 36, 3, 10);
    mat_translate(mb, x, y, z);
    mat_multiply(ma, mb, ma);
    mat_apply(data, ma, 36, 0, 10);
}

void draw_player(Attrib *attrib, Player *player) {
    draw_cube(attrib, player->buffer);
}
