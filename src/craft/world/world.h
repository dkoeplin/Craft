#ifndef _world_h_
#define _world_h_

#include <unordered_map>

#include "craft/player/Player.h"
#include "craft/world/Dimension.h"
#include "craft/world/State.h"
#include "craft/world/Chunk.h"

struct Attrib;
struct WorkerItem;

struct World {
 public:
  Player *find_player(int64_t id);
  Player *add_player(int id);
  Player *next_player(Player *p);
  void delete_player(int64_t id);
  void delete_all_players();
  size_t player_count() { return players.size(); }
  void interpolate_players();

  void move_entities(double dt);

  // TODO: Dimension

  /// Time
  float time_of_day();
  float get_daylight();

  /// Chunks
  Chunk *find_chunk(int p, int q);
  Chunk *force_chunk(int p, int q);
  void delete_chunks(const std::vector<Player *> &observers);
  void delete_all_chunks();
  int has_lights(Chunk *chunk);
  void mark_chunk_dirty(Chunk *chunk);
  int chunk_count() { return chunks.size(); }

  /// Blocks
  int highest_block(float x, float z);
  int get_block(int x, int y, int z);

  Player *closest_player_in_view(Player *player);

  /// Physics
  Block hit_test(const State &state, bool use_prev = false);
  BlockFace hit_test_face(const State &state);
  bool collide(int height, State &state);

  /// Rendering
  void render_players(Attrib *attrib, Player *player, int width, int height);
  int render_chunks(Attrib *attrib, Player *player, int width, int height);
  void render_signs(Attrib *attrib, Player *player, int width, int height);
  void render_sky(Attrib *attrib, Player *player, int width, int height);

  void reset();

  int day_length;

 private:
  std::vector<std::unique_ptr<Chunk>> chunks;
  std::vector<std::unique_ptr<Player>> players;
  std::unique_ptr<Dimension> dimension;
};

typedef void (*world_func)(int, int, int, int, void *);

void create_world(int p, int q, world_func func, void *arg);

#endif
