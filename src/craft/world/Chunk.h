#ifndef CRAFT_SRC_CRAFT_WORLD_CHUNK_H_
#define CRAFT_SRC_CRAFT_WORLD_CHUNK_H_

#include <cmath>

#include "craft/support/map.h"
#include "craft/world/Sign.h"

#define MAX_CHUNKS 8192
#define CHUNK_SIZE 32

typedef unsigned int GLuint;
struct Attrib;
struct Chunk;
struct WorkerItem;
struct World;

struct Chunk {
  ~Chunk();
  Chunk(int p, int q);

  Map map;
  Map lights;
  std::vector<Sign> signs;
  int p;
  int q;
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

void draw_chunk(Attrib *attrib, Chunk *chunk);

#endif //CRAFT_SRC_CRAFT_WORLD_CHUNK_H_
