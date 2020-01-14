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
  int load_state(float *x, float *y, float *z, float *rx, float *ry);
  static void insert_block(int p, int q, int x, int y, int z, int w);
  static void insert_light(int p, int q, int x, int y, int z, int w);
  static void insert_sign(int p, int q, int x, int y, int z, int face, const char *text);
  void delete_sign(int x, int y, int z, int face);
  void delete_signs(int x, int y, int z);
  void delete_all_signs();
  void load_blocks(Map *map, int p, int q);
  void load_lights(Map *map, int p, int q);
  void load_signs(std::vector<Sign> &list, int p, int q);
  int get_key(int p, int q);
  static void set_key(int p, int q, int key);
  static void worker_start(char *path = nullptr);
  void worker_stop();
  static int worker_run(void *arg);
};



#endif
