#ifndef CRAFT_SRC_CRAFT_SESSION_WORLDSESSION_H_
#define CRAFT_SRC_CRAFT_SESSION_WORLDSESSION_H_

#include <memory>

#include "craft/multiplayer/client.h"
#include "craft/support/db.h"
#include "craft/world/world.h"

struct Block;
struct Client;
struct Database;

struct WorldSession {
 public:
  WorldSession();

  /// World Editing
  void unset_sign(int x, int y, int z);
  void unset_sign_face(int x, int y, int z, int face);

  void builder_block(int x, int y, int z, int w);
  void set_block(int x, int y, int z, int w);
  void set_sign(int x, int y, int z, int face, const char *text);
  void set_light(int p, int q, int x, int y, int z, int w);

  void toggle_light(int x, int y, int z);

  /// Models
  void array(Block *b1, Block *b2, int xc, int yc, int zc);
  void cube(Block *b1, Block *b2, int fill);
  void sphere(Block *center, int radius, int fill, int fx, int fy, int fz);
  void cylinder(Block *b1, Block *b2, int radius, int fill);
  void tree(Block *block);

 protected:
  void set_block(int p, int q, int x, int y, int z, int w, bool dirty);
  void set_sign(int p, int q, int x, int y, int z, int face, const char *text, bool dirty);

  std::unique_ptr<World> world;
  std::unique_ptr<Database> db;
  std::unique_ptr<Client> client;
};


#endif //CRAFT_SRC_CRAFT_SESSION_WORLDSESSION_H_
