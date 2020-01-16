#ifndef CRAFT_SRC_CRAFT_SESSION_WORLDSESSION_H_
#define CRAFT_SRC_CRAFT_SESSION_WORLDSESSION_H_

#include <memory>

#include "craft/multiplayer/Client.h"
#include "craft/support/db.h"
#include "craft/world/World.h"

struct Block;
struct Client;
struct Database;

struct WorldSession {
 public:
  WorldSession();

  /// World Editing
  void unset_sign(const ILoc &pos);
  void unset_sign_face(const Face &face);

  void builder_block(const Block &block);
  void set_block(const Block &block);
  void set_sign(const Sign &sign);
  void set_light(const ChunkPos &pos, const ILoc &loc, int w);

  void toggle_light(const ILoc &loc);

  /// Models
  void array(Block *b1, Block *b2, int xc, int yc, int zc);
  void cube(Block *b1, Block *b2, int fill);
  void sphere(Block *center, int radius, int fill, int fx, int fy, int fz);
  void cylinder(Block *b1, Block *b2, int radius, int fill);
  void tree(Block *block);

 protected:
  void set_block(const ChunkPos &pos, const Block &block, bool dirty);
  void set_sign(const ChunkPos &pos, const Sign &sign, bool dirty);

  std::unique_ptr<World> world;
  std::unique_ptr<Database> db;
  std::unique_ptr<Client> client;
};


#endif //CRAFT_SRC_CRAFT_SESSION_WORLDSESSION_H_
