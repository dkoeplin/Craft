#ifndef CRAFT_SRC_CRAFT_SESSION_SESSION_H_
#define CRAFT_SRC_CRAFT_SESSION_SESSION_H_

#include <vector>

#include "craft/session/WorldSession.h"
#include "craft/world/State.h"

struct Attrib;
struct ChatInterface;
struct Client;
struct Chunk;
struct Database;
struct Key;
struct Interface;
struct Player;
struct Window;
struct World;
struct Worker;
struct WorkerItem;

/// Window settings
#define FULLSCREEN 0
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define VSYNC 1

/// Performance
#define WORKERS 4

/// Message settings
#define MAX_MESSAGES 4
#define MAX_TEXT_LENGTH 256

#define MAX_ADDR_LENGTH 256
#define MAX_PATH_LENGTH 256
#define DEFAULT_PORT 4080
#define DB_PATH "craft.db"

struct Session : public WorldSession {
 public:
  enum class Mode {
    Running,
    Changed,
    Exiting
  };

  Session();

  static bool init();
  bool create_window();

  static Session *current;

  /// Suppress the next on_char call (was used by on_key already)
  void suppress_char() { suppress_next_char = true; }

  void on_char(unsigned int u);
  void on_key(int key, int scancode, int action, int mods);
  void on_scroll(double dx, double dy);
  void on_mouse_button(int button, int action, int mods);
  void mouse_movement(double dt);
  void held_keys(double dt);

  void init_workers();
  void init_database();
  void update_server(const char *server, const char *port = nullptr, bool changed = true);
  void update_world_file(const char *file);
  void set_identity(char *username, char *token);
  void login_user(char *username);
  void logout();

  void reconnect();
  void load_world();

  void tick();
  Mode render();
  void shutdown();

  void set_draw_distance(int radius);
  void add_message(const char *text);
  void talk(const char *text);
  void show_chat(bool cmd);

  /// Window
  Window *window() { return window_.get(); }
  void close_interface(Interface *iface);

  /// Multiplayer
  bool is_online() { return online; }
  bool is_offline() { return !online; }

  /// World
  void gen_chunk_buffer(Chunk *chunk);
  void init_chunk(Chunk *chunk);
  void load_chunk(WorkerItem *item);
  void force_chunks(Player *p, int radius = 1);
  void ensure_chunks(Player *p);
  void request_chunk(int p, int q);
  void create_chunk(Chunk *chunk, int p, int q);

  /// Rendering
  void check_workers();
  void render_signs(Attrib *attrib, Player *p, int w, int h);
  void render_players(Attrib *attrib, Player *p, int w, int h);
  void render_chunks(Attrib *attrib, Player *p, int w, int h);
  void render_sky(Attrib *attrib, Player *p, int w, int h);
  void render_wireframe(Attrib *attrib, Player *p, int w, int h);

 private:
  friend struct Worker;
  Player *player;

  std::vector<std::unique_ptr<Interface>> interfaces;
  ChatInterface *chat = nullptr;

  /// Window
  std::unique_ptr<Window> window_;

  /// Multiplayer
  bool online = false;
  bool server_changed = false;

  void parse_buffer(char *buffer);

  char db_path[MAX_PATH_LENGTH];
  char server_addr[MAX_ADDR_LENGTH];
  int server_port;

  ///
  bool suppress_next_char = false;
  bool time_changed = false;
  double last_commit;
  double last_update;
  double previous_time;

  /// Rendering
  std::vector<std::unique_ptr<Worker>> workers;
};


#endif //CRAFT_SRC_CRAFT_SESSION_SESSION_H_
