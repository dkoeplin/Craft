#ifndef CRAFT_SRC_CRAFT_WORLD_CHUNK_H_
#define CRAFT_SRC_CRAFT_WORLD_CHUNK_H_

#include <cmath>

#include "craft/support/map.h"
#include "craft/support/matrix.h"
#include "craft/world/Vec3.h"
#include "craft/world/Sign.h"

#define MAX_CHUNKS 8192
#define CHUNK_SIZE 32

typedef unsigned int GLuint;
struct Shader;
struct Chunk;
struct WorkerItem;
struct World;

bool chunk_visible(Planes &planes, const ChunkPos &pos, int miny, int maxy, bool ortho);

struct Chunk {
  explicit Chunk(ChunkPos pos);
  ~Chunk();

  bool is_visible(Planes &planes, bool ortho) {
      return chunk_visible(planes, pos, miny, maxy, ortho);
  }

  Map map;
  Map lights;
  std::vector<Sign> signs;
  ChunkPos pos;
  int faces = 0;
  int sign_faces = 0;
  bool dirty = true;
  bool unloaded = true;
  int miny = 0;
  int maxy = 0;
  GLuint buffer = 0;
  GLuint sign_buffer = 0;
};

inline int chunked(float x) { return floorf(roundf(x) / CHUNK_SIZE); }
inline int chunked(int x) { return floorf(roundf(x) / CHUNK_SIZE); }

WorkerItem create_chunk_work_item(World *world, Chunk *chunk);

void compute_chunk(WorkerItem *item);

void generate_chunk(Chunk *chunk, WorkerItem *item);

void draw_chunk(Shader *attrib, Chunk *chunk);

#endif //CRAFT_SRC_CRAFT_WORLD_CHUNK_H_
