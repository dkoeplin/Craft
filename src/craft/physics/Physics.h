
#ifndef CRAFT_SRC_CRAFT_PHYSICS_PHYSICS_H_
#define CRAFT_SRC_CRAFT_PHYSICS_PHYSICS_H_

#include "craft/world/Chunk.h"
#include "craft/world/State.h"

struct Block;
struct Player;
struct State;
struct Map;

#define XZ_SIZE (CHUNK_SIZE * 3 + 2)
#define XZ_LO (CHUNK_SIZE)
#define XZ_HI (CHUNK_SIZE * 2 + 1)
#define Y_SIZE 258
#define XYZ(x, y, z) ((y) * XZ_SIZE * XZ_SIZE + (x) * XZ_SIZE + (z))
#define XZ(x, z) ((x) * XZ_SIZE + (z))

void occlusion(char neighbors[27], char lights[27], float shades[27],
               float ao[6][4], float light[6][4]);

void light_fill(char *opaque, char *light, int x, int y, int z, int w, int force);

float player_player_distance(Player *p1, Player *p2);
float player_crosshair_distance(Player *p1, Player *p2);

Block hit_test(Map *map, float max_distance, bool use_prev,
               const State &state, const Vec3<float> &vec);

bool player_intersects_block(int height, const State &state, const Block &block);

bool collide(int height, State &state);

Vec3<float> get_sight_vector(float rx, float ry);

Vec3<float> get_motion_vector(bool flying, int sz, int sx, float rx, float ry);

#endif //CRAFT_SRC_CRAFT_PHYSICS_PHYSICS_H_
