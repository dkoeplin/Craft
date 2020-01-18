#ifndef _db_h_
#define _db_h_

#include <vector>

#include "map.h"
#include "craft/world/Sign.h"

struct Database {
 public:
  int init(char *path);
  void close();
  static void commit();
  void auth_set(char *username, char *identity_token);
  int auth_select(char *username);
  void auth_select_none();
  int auth_get(char *username, char *identity_token, int identity_token_length);
  int auth_get_selected(
          char *username, int username_length,
          char *identity_token, int identity_token_length);
  void save_state(float x, float y, float z, float rx, float ry);
  void save_state(const State &state) {
      save_state(state.x, state.y, state.z, state.rx, state.ry);
  }

  int load_state(float *x, float *y, float *z, float *rx, float *ry);
  int load_state(State &state) {
      return load_state(&state.x, &state.y, &state.z, &state.rx, &state.ry);
  }

  static void insert_block(int p, int q, int x, int y, int z, int w);
  static void insert_block(const ChunkPos &pos, const Block &block) {
      insert_block(pos.x, pos.z, block.x, block.y, block.z, block.w);
  }

  static void insert_light(int p, int q, int x, int y, int z, int w);
  static void insert_light(const ChunkPos &pos, const ILoc3 &loc, int w) {
      insert_light(pos.x, pos.z, loc.x, loc.y, loc.z, w);
  }

  static void insert_sign(int p, int q, int x, int y, int z, int face, const char *text);
  static void insert_sign(const ChunkPos &pos, const Sign &sign) {
      insert_sign(pos.x, pos.z, sign.x, sign.y, sign.z, sign.side, sign.text.c_str());
  }

  void delete_sign(int x, int y, int z, int face);
  void delete_sign(const Face &face) { delete_sign(face.x, face.y, face.z, face.side); }

  void delete_signs(int x, int y, int z);
  void delete_signs(const ILoc3 &loc) {
      delete_signs(loc.x, loc.y, loc.z);
  }

  void delete_all_signs();
  void load_blocks(Map *map, int p, int q);
  void load_blocks(Map *map, const ChunkPos &pos) {
      load_blocks(map, pos.x, pos.z);
  }

  void load_lights(Map *map, int p, int q);
  void load_lights(Map *map, const ChunkPos &pos) {
      load_lights(map, pos.x, pos.z);
  }

  void load_signs(std::vector<Sign> &list, int p, int q);
  void load_signs(std::vector<Sign> &list, const ChunkPos &pos) {
      load_signs(list, pos.x, pos.z);
  }

  int get_key(int p, int q);
  int get_key(const ChunkPos &pos) {
      return get_key(pos.x, pos.z);
  }

  static void set_key(int p, int q, int key);
  static void set_key(const ChunkPos &pos, int key) {
      set_key(pos.x, pos.z, key);
  }

  static void worker_start(char *path = nullptr);
  void worker_stop();
  static int worker_run(void *arg);
};



#endif
