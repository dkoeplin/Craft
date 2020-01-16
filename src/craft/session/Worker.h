#ifndef CRAFT_SRC_CRAFT_SUPPORT_WORKER_H_
#define CRAFT_SRC_CRAFT_SUPPORT_WORKER_H_

#include "GL/glew.h"

extern "C" {
#include "tinycthread.h"
}

#include "craft/support/map.h"
#include "craft/world/Vec.h"

struct Chunk;
struct Player;
struct Session;
struct World;

struct WorkerItem {
  ChunkPos pos;
  bool load; // Chunk requires loading
  Map *block_maps[3][3];
  Map *light_maps[3][3];
  int miny;
  int maxy;
  int faces;
  GLfloat *data;

  void free_maps();
};

struct Worker {
 public:
  enum class Status {
    Idle = 0,
    Busy = 1,
    Done = 2
  };

  explicit Worker(Session *session, int index);
  void ensure_chunks(Player *player);

  /// External acknowledge done and return to idle
  void ack();
  bool is_done() { return state == Status::Done; }
  bool is_idle() { return state == Status::Idle; }

  void lock() { mtx_lock(&mtx); }
  void unlock() { mtx_unlock(&mtx); }

  Chunk *update_chunk(World *world);

 private:
  friend int worker_run(void *arg);

  int index;
  Status state = Status::Idle;
  thrd_t thrd;
  mtx_t mtx;
  cnd_t cnd;
  WorkerItem item;
  Session *session;
};

#endif //CRAFT_SRC_CRAFT_SUPPORT_WORKER_H_
