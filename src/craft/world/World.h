#ifndef _world_h_
#define _world_h_

#include <unordered_map>

#include "craft/player/Player.h"
#include "craft/world/Chunk.h"
#include "craft/world/Dimension.h"
#include "craft/world/State.h"

struct Shader;
struct WorkerItem;

struct World {
  public:
    /// Players
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
    Chunk *find_chunk(const ChunkPos &pos);
    Chunk *force_chunk(const ChunkPos &pos);
    void delete_chunks(const std::vector<Player *> &observers);
    void delete_all_chunks();
    int chunk_has_lights(Chunk *chunk);
    void mark_chunk_dirty(Chunk *chunk);
    int chunk_count() { return chunks.size(); }

    /// Blocks
    int highest_block(int x, int z);
    int get_block_material(const ILoc3 &pos);
    Block get_block(const ILoc3 &pos);

    Player *closest_player_in_view(Player *player);

    /// Physics
    Block hit_test(const State &state, bool use_prev = false);
    BlockFace hit_test_face(const State &state);
    bool collide(int height, State &state);

    /// Rendering
    void render_players(Shader *attrib, Player *player, int width, int height);
    int render_chunks(Shader *attrib, Player *player, int width, int height);
    void render_signs(Shader *attrib, Player *player, int width, int height);
    void render_sky(Shader *attrib, Player *player, int width, int height);

    void reset();

    int day_length;

  private:
    std::vector<std::unique_ptr<Chunk>> chunks;
    std::vector<std::unique_ptr<Player>> players;
    std::unique_ptr<Dimension> dimension;
};

typedef void (*world_func)(int, int, int, int, void *);

void create_world(int p, int q, world_func func, void *arg);
inline void create_world(const ChunkPos &pos, world_func func, void *arg) { create_world(pos.x, pos.z, func, arg); }

#endif
