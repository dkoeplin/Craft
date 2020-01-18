#ifndef CRAFT_SRC_CRAFT_SESSION_SESSION_H_
#define CRAFT_SRC_CRAFT_SESSION_SESSION_H_

#include <vector>

#include "craft/interfaces/Interface.h"
#include "craft/session/Window.h"
#include "craft/session/Worker.h"
#include "craft/session/WorldSession.h"
#include "craft/world/State.h"

struct Shader;
struct ChatInterface;
struct Client;
struct Chunk;
struct Database;
struct Key;
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
    Session();

    void run();

    /// Suppress the next on_char call (was used by on_key already)
    void suppress_next_char() { suppress_next_char_ = true; }

    void set_draw_distance(int radius);
    void add_message(const char *text);
    void talk(const char *text);
    void show_chat(bool cmd);

    /// Window
    Window *window() { return window_.get(); }
    void close_interface(Interface *iface);

    template <typename Iface>
    Iface *get_interface() {
        auto index = std::find_if(interfaces.begin(), interfaces.end(), [&](auto &i) {
            return i->get_name() == Iface::name();
        });
        return (index != interfaces.end()) ? (Iface *)(index->get()) : nullptr;
    }
    template <typename Iface>
    Iface *get_or_open_interface() {
        if (auto *face = get_interface<Iface>()) {
            return face;
        }
        else {
            auto iface = std::make_unique<Iface>(this, world.get(), player);
            auto *result = iface.get();
            interfaces.push_back(std::move(iface));
            return result;
        }
    }

    /// Multiplayer
    bool is_online() { return online; }
    bool is_offline() { return !online; }
    void update_server(const char *server, const char *port = nullptr, bool changed = true);
    void set_identity(char *username, char *token);
    void login_user(char *username);
    void logout();

    /// World
    void update_world_file(const char *file);
    void gen_chunk_buffer(Chunk *chunk);
    void init_chunk(Chunk *chunk);
    void load_chunk(WorkerItem *item);
    void force_chunks(Player *p);
    void ensure_chunks(Player *p);
    void request_chunk(const ChunkPos &pos);
    void create_chunk(Chunk *chunk);

    /// Rendering
    void check_workers();
    void render_signs(Shader *attrib, Player *p, int w, int h);
    void render_players(Shader *attrib, Player *p, int w, int h);
    void render_chunks(Shader *attrib, Player *p, int w, int h);
    void render_sky(Shader *attrib, Player *p, int w, int h);
    void render_wireframe(Shader *attrib, Player *p, int w, int h);

  private:
    enum class Mode { Running, Changed, Exiting };

    friend void on_key(GLFWwindow *window, int key, int scancode, int action, int mods);
    friend void on_char(GLFWwindow *window, uint32_t u);
    friend void on_scroll(GLFWwindow *window, double xdelta, double ydelta);
    friend void on_mouse_button(GLFWwindow *window, int button, int action, int mods);

    void on_char(unsigned int u);
    void on_key(int key, int scancode, int action, int mods);
    void on_scroll(double dx, double dy);
    void on_mouse_button(int button, int action, int mods);
    void mouse_movement(double dt);
    void held_keys(double dt);

    void init_database();

    void reconnect();
    void load_world();
    void unload_world();

    void tick();
    Mode render();

    void terminate();

    void parse_buffer(char *buffer);

    Player *player;

    std::vector<std::unique_ptr<Interface>> interfaces;
    ChatInterface *chat = nullptr;

    std::unique_ptr<Window> window_;

    /// Rendering
    friend struct Worker;
    std::vector<std::unique_ptr<Worker>> workers;

    /// Multiplayer
    bool online = false;
    bool server_changed = false;
    char db_path[MAX_PATH_LENGTH];
    char server_addr[MAX_ADDR_LENGTH];
    int server_port;

    /// Other State
    bool suppress_next_char_ = false;
    bool time_changed = false;
    double last_commit;
    double last_update;
    double previous_time;
};

#endif // CRAFT_SRC_CRAFT_SESSION_SESSION_H_
