#include "Physics.h"

#include <cmath>

#include "craft/player/Player.h"
#include "craft/util/Util.h"
#include "craft/world/Chunk.h"
#include "craft/world/State.h"



void occlusion(
        char neighbors[27], char lights[27], float shades[27],
        float ao[6][4], float light[6][4])
{
    static const int lookup3[6][4][3] = {
            {{0, 1, 3}, {2, 1, 5}, {6, 3, 7}, {8, 5, 7}},
            {{18, 19, 21}, {20, 19, 23}, {24, 21, 25}, {26, 23, 25}},
            {{6, 7, 15}, {8, 7, 17}, {24, 15, 25}, {26, 17, 25}},
            {{0, 1, 9}, {2, 1, 11}, {18, 9, 19}, {20, 11, 19}},
            {{0, 3, 9}, {6, 3, 15}, {18, 9, 21}, {24, 15, 21}},
            {{2, 5, 11}, {8, 5, 17}, {20, 11, 23}, {26, 17, 23}}
    };
    static const int lookup4[6][4][4] = {
            {{0, 1, 3, 4}, {1, 2, 4, 5}, {3, 4, 6, 7}, {4, 5, 7, 8}},
            {{18, 19, 21, 22}, {19, 20, 22, 23}, {21, 22, 24, 25}, {22, 23, 25, 26}},
            {{6, 7, 15, 16}, {7, 8, 16, 17}, {15, 16, 24, 25}, {16, 17, 25, 26}},
            {{0, 1, 9, 10}, {1, 2, 10, 11}, {9, 10, 18, 19}, {10, 11, 19, 20}},
            {{0, 3, 9, 12}, {3, 6, 12, 15}, {9, 12, 18, 21}, {12, 15, 21, 24}},
            {{2, 5, 11, 14}, {5, 8, 14, 17}, {11, 14, 20, 23}, {14, 17, 23, 26}}
    };
    static const float curve[4] = {0.0, 0.25, 0.5, 0.75};
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 4; j++) {
            int corner = neighbors[lookup3[i][j][0]];
            int side1 = neighbors[lookup3[i][j][1]];
            int side2 = neighbors[lookup3[i][j][2]];
            int value = side1 && side2 ? 3 : corner + side1 + side2;
            float shade_sum = 0;
            float light_sum = 0;
            int is_light = lights[13] == 15;
            for (int k = 0; k < 4; k++) {
                shade_sum += shades[lookup4[i][j][k]];
                light_sum += lights[lookup4[i][j][k]];
            }
            if (is_light) {
                light_sum = 15 * 4 * 10;
            }
            float total = curve[value] + shade_sum / 4.0;
            ao[i][j] = MIN(total, 1.0);
            light[i][j] = light_sum / 15.0 / 4.0;
        }
    }
}

void light_fill(char *opaque, char *light, int x, int y, int z, int w, int force) {
    if (x + w < XZ_LO || z + w < XZ_LO) {
        return;
    }
    if (x - w > XZ_HI || z - w > XZ_HI) {
        return;
    }
    if (y < 0 || y >= Y_SIZE) {
        return;
    }
    if (light[XYZ(x, y, z)] >= w) {
        return;
    }
    if (!force && opaque[XYZ(x, y, z)]) {
        return;
    }
    light[XYZ(x, y, z)] = w--;
    light_fill(opaque, light, x - 1, y, z, w, 0);
    light_fill(opaque, light, x + 1, y, z, w, 0);
    light_fill(opaque, light, x, y - 1, z, w, 0);
    light_fill(opaque, light, x, y + 1, z, w, 0);
    light_fill(opaque, light, x, y, z - 1, w, 0);
    light_fill(opaque, light, x, y, z + 1, w, 0);
}

float player_player_distance(Player *p1, Player *p2) {
    State *s1 = &p1->state;
    State *s2 = &p2->state;
    float x = s2->x - s1->x;
    float y = s2->y - s1->y;
    float z = s2->z - s1->z;
    return sqrtf(x * x + y * y + z * z);
}

float player_crosshair_distance(Player *p1, Player *p2) {
    State *s1 = &p1->state;
    State *s2 = &p2->state;

    Vec3<float> vec = get_sight_vector(s1->rx, s1->ry);
    vec *= player_player_distance(p1, p2);

    return (s1->loc() - s2->loc() - vec).len();
}

Block hit_test(Map *map, float max_distance, bool use_prev,
              const State &state, const Vec3<float> &vec)
{
    int m = 32;
    Vec3<float> step = vec / m;
    Vec3<float> current = state.loc();

    Vec3<int> previous;
    for (int i = 0; i < max_distance * m; i++) {
        Vec3<int> next = current.round();
        if (next != previous) {
            int hw = map_get(map, next.x, next.y, next.z);
            if (hw > 0) {
                return {(use_prev ? previous : next), hw};
            }
            previous = next;
        }
        current += step;
    }
    return {};
}

bool player_intersects_block(int height, const State &state, const Block &block) {
    int nx = roundf(state.x);
    int ny = roundf(state.y);
    int nz = roundf(state.z);
    for (int i = 0; i < height; i++) {
        if (nx == block.x && ny - i == block.y && nz == block.z) {
            return true;
        }
    }
    return false;
}

Vec3<float> get_sight_vector(float rx, float ry) {
    float m = cosf(ry);
    Vec3<float> vec;
    vec.x = cosf(rx - RADIANS(90)) * m;
    vec.y = sinf(ry);
    vec.z = sinf(rx - RADIANS(90)) * m;
    return vec;
}

Vec3<float> get_motion_vector(bool flying, int sz, int sx, float rx, float ry) {
    Vec3<float> vec;
    if (!sz && !sx) {
        return vec;
    }
    float strafe = atan2f(sz, sx);
    if (flying) {
        float m = cosf(ry);
        float y = sinf(ry);
        if (sx) {
            if (!sz) {
                y = 0;
            }
            m = 1;
        }
        if (sz > 0) {
            y = -y;
        }
        vec.x = cosf(rx + strafe) * m;
        vec.y = y;
        vec.z = sinf(rx + strafe) * m;
    }
    else {
        vec.x = cosf(rx + strafe);
        vec.y = 0;
        vec.z = sinf(rx + strafe);
    }
    return vec;
}
