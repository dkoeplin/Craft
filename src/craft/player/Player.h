#ifndef CRAFT_SRC_CRAFT_PLAYER_PLAYER_H_
#define CRAFT_SRC_CRAFT_PLAYER_PLAYER_H_

#include "craft/player/KeyBindings.h"
#include "craft/util/Util.h"
#include "craft/world/State.h"

struct Shader;

#define MAX_NAME_LENGTH 32

struct Player {
  public:
    explicit Player(int id) : id(id), name{0} { name[0] = '\0'; }

    void set_draw_distance(int radius) {
        create_radius = radius;
        render_radius = radius;
        delete_radius = radius + 4;
    }

    int id;
    char name[MAX_NAME_LENGTH];
    State state;
    FVec3 velocity;
    FVec3 accel;
    int step = 1;
    // Saved states (used for interpolation)
    State state1;
    State state2;
    GLuint buffer = 0;
    KeyBindings keys;
    bool flying = false;
    int ortho = 0;
    float fov = 65.0f;
    int item_index = 0;

    /// Rendering options
    int create_radius = 10;
    int delete_radius = 14;
    int render_radius = 10;
    int sign_radius = 4;
};

void update_player(Player *player, float x, float y, float z, float rx, float ry, int interpolate);

void interpolate_player(Player *player);

GLuint gen_player_buffer(const State &state);

void make_player(float *data, float x, float y, float z, float rx, float ry);

void draw_player(Shader *attrib, Player *player);

#endif // CRAFT_SRC_CRAFT_PLAYER_PLAYER_H_
