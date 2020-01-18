#include "Session.h"

#include <string>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "curl/curl.h"

#include "craft/draw/Cube.h"
#include "craft/draw/Render.h"
#include "craft/draw/Shader.h"
#include "craft/interfaces/ChatInterface.h"
#include "craft/interfaces/DebugInterface.h"
#include "craft/interfaces/Interface.h"
#include "craft/interfaces/WorldInterface.h"
#include "craft/multiplayer/Auth.h"
#include "craft/multiplayer/Client.h"
#include "craft/physics/Physics.h"
#include "craft/player/Player.h"
#include "craft/session/Window.h"
#include "craft/session/Worker.h"
#include "craft/support/db.h"
#include "craft/util/Logging.h"
#include "craft/util/Util.h"
#include "craft/world/Chunk.h"
#include "craft/world/State.h"
#include "craft/world/World.h"

Session::Session() : WorldSession() {
    window_ = std::make_unique<Window>();

    for (int i = 0; i < WORKERS; i++) {
        auto worker = std::make_unique<Worker>(this, i);
        workers.push_back(std::move(worker));
    }
}

void Session::run() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    srand(time(nullptr));
    rand();

    REQUIRE(glfwInit(), "Unable to initialize GLFW.");

    REQUIRE(window_->init(this), "Unable to initialize window.");
    Render::load_textures();
    Render::load_shaders();

    Session::Mode mode = Session::Mode::Running;
    while (mode != Session::Mode::Exiting) {
        reconnect();
        init_database();
        load_world();

        while (mode == Session::Mode::Running) {
            tick();
            mode = render();
        }

        unload_world();
    }

    terminate();
}

void Session::terminate() {
    glfwTerminate();
    curl_global_cleanup();
}

void Session::on_char(unsigned int u) {
    if (suppress_next_char) {
        suppress_next_char = false;
    } else if (window_->in_focus()) {
        bool consumed = false;
        for (auto interface = interfaces.rbegin(); !consumed && interface != interfaces.rend(); ++interface) {
            consumed |= (*interface)->on_char(u);
        }
    }
}

void Session::on_key(int key, int scancode, int action, int mods) {
    if (window_->in_focus()) {
        auto kkey = Key(static_cast<Key::Value>(key));
        auto kmods = ButtonMods(mods);
        bool consumed = false;
        if (action == GLFW_PRESS) {
            for (auto interface = interfaces.rbegin(); !consumed && interface != interfaces.rend(); ++interface) {
                consumed |= (*interface)->on_key_press(kkey, scancode, kmods);
            }
        } else if (action == GLFW_REPEAT) {
            for (auto interface = interfaces.rbegin(); !consumed && interface != interfaces.rend(); ++interface) {
                consumed |= (*interface)->on_key_repeat(kkey, scancode, kmods);
            }
        } else if (action == GLFW_RELEASE) {
            for (auto interface = interfaces.rbegin(); !consumed && interface != interfaces.rend(); ++interface) {
                consumed |= (*interface)->on_key_release(kkey, scancode, kmods);
            }
        }
    }
}

void Session::on_scroll(double dx, double dy) {
    if (window_->in_focus()) {
        bool consumed = false;
        for (auto interface = interfaces.rbegin(); !consumed && interface != interfaces.rend(); ++interface) {
            consumed |= (*interface)->on_scroll(dx, dy);
        }
    }
}

void Session::on_mouse_button(int button, int action, int mods) {
    if (!window_->in_focus()) {
        window_->focus();
    } else {
        auto mbutton = static_cast<MouseButton>(button);
        auto maction = static_cast<MouseAction>(action);
        auto mmods = ButtonMods(mods);
        bool consumed = false;
        for (auto interface = interfaces.rbegin(); !consumed && interface != interfaces.rend(); ++interface) {
            consumed |= (*interface)->on_mouse_button(mbutton, maction, mmods);
        }
    }
}

void Session::mouse_movement(double dt) {
    static bool first_update = true;
    static double px = 0;
    static double py = 0;

    if (window_->in_focus() && !first_update) {
        double x, y;
        window_->get_cursor_pos(x, y);
        double dx = x - px;
        double dy = y - py;
        bool consumed = false;
        for (auto interface = interfaces.rbegin(); !consumed && interface != interfaces.rend(); ++interface) {
            consumed |= (*interface)->mouse_movement(x, y, dx, dy, dt);
        }
        px = x;
        py = y;
    } else {
        first_update = false;
        window_->get_cursor_pos(px, py);
    }
}

void Session::held_keys(double dt) {
    bool consumed = false;
    for (auto interface = interfaces.rbegin(); !consumed && interface != interfaces.rend(); ++interface) {
        consumed |= (*interface)->held_keys(dt);
    }
}

void Session::init_database() {
#define USE_CACHE 1
    if (is_offline() || USE_CACHE) {
        REQUIRE(!db->init(db_path), "Unable to load database at " << std::string(db_path));
        if (online) {
            // TODO: support proper caching of signs (handle deletions)
            db->delete_all_signs();
        }
    }
}

void Session::update_world_file(const char *file) { snprintf(db_path, MAX_PATH_LENGTH, "%s", DB_PATH); }

void Session::update_server(const char *server, const char *port, bool changed) {
    server_changed = changed;
    if (server) {
        online = true;
        strncpy(server_addr, server, MAX_ADDR_LENGTH);
        server_port = port ? atoi(port) : DEFAULT_PORT;
        snprintf(db_path, MAX_PATH_LENGTH, "cache.%s.%d.db", server_addr, server_port);
    } else {
        online = false;
        snprintf(db_path, MAX_PATH_LENGTH, "%s", DB_PATH);
    }
}

void Session::reconnect() {
    if (online) {
        client->connect(server_addr, server_port);
        client->start();
        client->version(1);

        char username[128] = {0};
        char identity_token[128] = {0};
        char access_token[128] = {0};
        if (db->auth_get_selected(username, 128, identity_token, 128)) {
            printf("Contacting login server for username: %s\n", username);
            if (get_access_token(access_token, 128, username, identity_token)) {
                printf("Successfully authenticated with the login server\n");
                client->login(username, access_token);
            } else {
                printf("Failed to authenticate with the login server\n");
                client->login("", "");
            }
        } else {
            printf("Logging in anonymously\n");
            client->login("", "");
        }
    }
}

void Session::tick() {
    double now = glfwGetTime();
    double dt = now - previous_time;
    dt = MIN(dt, 0.2);
    dt = MAX(dt, 0.0);
    previous_time = now;

    if (time_changed) {
        time_changed = false;
        last_commit = glfwGetTime();
        last_update = glfwGetTime();
    }

    window_->update();
    mouse_movement(dt);
    held_keys(dt);
    world->move_entities(dt);

    bool consumed = false;
    for (auto interface = interfaces.rbegin(); !consumed && interface != interfaces.rend(); ++interface) {
        consumed |= (*interface)->tick(dt);
    }

    if (online) {
        /// Handle data from server
        char *buffer = client->recv();
        if (buffer) {
            parse_buffer(buffer);
            free(buffer);
        }
        /// Send position to server
        if (now - last_update > 0.1) {
            last_update = now;
            client->position(player->state);
        }

        /// Interpolate player positions
        world->interpolate_players();
    }

    // FLUSH DATABASE //
    if (now - last_commit > COMMIT_INTERVAL) {
        last_commit = now;
        db->commit();
    }
}

Session::Mode Session::render() {
    // TODO: Also account for other observers
    world->delete_chunks({player});

    del_buffer(player->buffer);
    player->buffer = gen_player_buffer(player->state);

    bool top = true;
    bool consumed = false;
    // Render proceeds from back to front
    for (auto interface = interfaces.begin(); !consumed && interface != interfaces.end(); ++interface) {
        consumed |= (*interface)->render(top);
        top = false;
    }

    window_->swap();
    if (window_->should_close()) {
        return Mode::Exiting;
    }
    if (server_changed) {
        server_changed = false;
        return Mode::Changed;
    }
    return Mode::Running;
}

void Session::unload_world() {
    db->save_state(player->state);
    db->close();
    client->stop();
    world->delete_all_chunks();
    world->delete_all_players();
}

void Session::load_world() {
    world->reset();
    last_commit = glfwGetTime();
    last_update = glfwGetTime();
    previous_time = glfwGetTime();

    player = world->add_player(0);
    auto &state = player->state;
    REQUIRE(player, "Failed to create player");

    // Load state from database
    bool loaded = db->load_state(player->state);
    force_chunks(player);
    if (!loaded) {
        player->state.y = world->highest_block(state.x, state.z) + 2;
    }

    auto world_iface = std::make_unique<WorldInterface>(this, world.get(), player);
    auto chat_iface = std::make_unique<ChatInterface>(this, world.get());
    auto debug_iface = std::make_unique<DebugInterface>(this, world.get(), player);
    chat = chat_iface.get();
    interfaces.push_back(std::move(world_iface));
    interfaces.push_back(std::move(chat_iface));
    interfaces.push_back(std::move(debug_iface));
}

void Session::check_workers() {
    for (auto &worker : workers) {
        worker->lock();
        if (worker->is_done()) {
            if (auto *chunk = worker->update_chunk(world.get())) {
                request_chunk(chunk->pos);
            }
        }
        worker->unlock();
    }
}

void Session::gen_chunk_buffer(Chunk *chunk) {
    WorkerItem item = create_chunk_work_item(world.get(), chunk);
    compute_chunk(&item);
    generate_chunk(chunk, &item);
    chunk->dirty = false;
}

void Session::load_chunk(WorkerItem *item) {
    Map *block_map = item->block_maps[1][1];
    Map *light_map = item->light_maps[1][1];
    create_world(item->pos, map_set_func, block_map);
    db->load_blocks(block_map, item->pos);
    db->load_lights(light_map, item->pos);
}

void Session::request_chunk(const ChunkPos &pos) {
    int key = db->get_key(pos);
    client->chunk(pos, key);
}

void Session::init_chunk(Chunk *chunk) {
    world->mark_chunk_dirty(chunk);
    db->load_signs(chunk->signs, chunk->pos);
}

void Session::create_chunk(Chunk *chunk) {
    chunk->unloaded = false;
    init_chunk(chunk);

    WorkerItem item = {};
    item.pos = chunk->pos;
    item.block_maps[1][1] = &chunk->map;
    item.light_maps[1][1] = &chunk->lights;
    load_chunk(&item);
    request_chunk(chunk->pos);
}

void Session::force_chunks(Player *pl) {
    ChunkPos pos = pl->state.chunk();
    pos.surrounding<1>([&](ChunkPos pos2) {
        if (auto *chunk = world->force_chunk(pos2)) {
            if (chunk->unloaded)
                create_chunk(chunk);
            if (chunk->dirty)
                gen_chunk_buffer(chunk);
        }
    });
}

void Session::ensure_chunks(Player *p) {
    check_workers();
    force_chunks(p);
    for (auto &worker : workers) {
        worker->lock();
        if (worker->is_idle()) {
            worker->ensure_chunks(p);
        }
        worker->unlock();
    }
}

void Session::parse_buffer(char *buffer) {
    State &state = player->state;
    char *key;
    char *line = tokenize(buffer, "\n", &key);
    while (line) {
        int pid;
        float ux, uy, uz, urx, ury;
        if (sscanf(line, "U,%d,%f,%f,%f,%f,%f", &pid, &ux, &uy, &uz, &urx, &ury) == 6) {
            player->id = pid;
            state.x = ux;
            state.y = uy;
            state.z = uz;
            state.rx = urx;
            state.ry = ury;
            force_chunks(player);
            if (uy <= 0) {
                state.y = world->highest_block(state.x, state.z) + 2;
            }
        }
        int bp, bq, bx, by, bz, bw;
        if (sscanf(line, "B,%d,%d,%d,%d,%d,%d", &bp, &bq, &bx, &by, &bz, &bw) == 6) {
            Block block{{bx, by, bz}, bw};
            set_block({bp, bq}, block, false);
            if (player_intersects_block(2, player->state, block)) {
                state.y = world->highest_block(state.x, state.z) + 2;
            }
        }
        if (sscanf(line, "L,%d,%d,%d,%d,%d,%d", &bp, &bq, &bx, &by, &bz, &bw) == 6) {
            ChunkPos pos{bp, bq};
            ILoc3 loc{bx, by, bz};
            set_light(pos, loc, bw);
        }
        float px, py, pz, prx, pry;
        if (sscanf(line, "P,%d,%f,%f,%f,%f,%f", &pid, &px, &py, &pz, &prx, &pry) == 6) {
            Player *other = world->find_player(pid);
            if (!other) {
                other = world->add_player(pid);
                snprintf(other->name, MAX_NAME_LENGTH, "other%d", pid);
                update_player(other, px, py, pz, prx, pry, 1); // twice
            }
            if (other) {
                update_player(other, px, py, pz, prx, pry, 1);
            }
        }
        if (sscanf(line, "D,%d", &pid) == 1) {
            world->delete_player(pid);
        }
        int kp, kq, kk;
        if (sscanf(line, "K,%d,%d,%d", &kp, &kq, &kk) == 3) {
            db->set_key(kp, kq, kk);
        }
        if (sscanf(line, "R,%d,%d", &kp, &kq) == 2) {
            if (Chunk *chunk = world->find_chunk({kp, kq})) {
                world->mark_chunk_dirty(chunk);
            }
        }
        double elapsed;
        int day_length;
        if (sscanf(line, "E,%lf,%d", &elapsed, &day_length) == 2) {
            glfwSetTime(fmod(elapsed, day_length));
            world->day_length = day_length;
            time_changed = true;
        }
        if (line[0] == 'T' && line[1] == ',') {
            char *text = line + 2;
            add_message(text);
        }
        char format[64];
        snprintf(format, sizeof(format), "N,%%d,%%%ds", MAX_NAME_LENGTH - 1);
        char name[MAX_NAME_LENGTH];
        if (sscanf(line, format, &pid, name) == 2) {
            if (Player *other = world->find_player(pid)) {
                strncpy(other->name, name, MAX_NAME_LENGTH);
            }
        }
        snprintf(format, sizeof(format), "S,%%d,%%d,%%d,%%d,%%d,%%d,%%%d[^\n]", MAX_SIGN_LENGTH - 1);
        int face;
        char text[MAX_SIGN_LENGTH] = {0};
        if (sscanf(line, format, &bp, &bq, &bx, &by, &bz, &face, text) >= 6) {
            Sign sign{{bx, by, bz, face}, text};
            set_sign({bp, bq}, sign, false);
        }
        line = tokenize(nullptr, "\n", &key);
    }
}

void Session::render_sky(Shader *attrib, Player *p, int w, int h) { world->render_sky(attrib, p, w, h); }

void Session::render_signs(Shader *attrib, Player *p, int w, int h) { world->render_signs(attrib, p, w, h); }

void Session::render_players(Shader *attrib, Player *p, int w, int h) { world->render_players(attrib, p, w, h); }

void Session::render_chunks(Shader *attrib, Player *p, int w, int h) {
    ensure_chunks(p);
    window_->set_face_count(world->render_chunks(attrib, p, w, h));
}

void Session::render_wireframe(Shader *attrib, Player *p, int w, int h) {
    ::render_wireframe(world.get(), attrib, p, w, h);
}

void Session::set_identity(char *username, char *token) {
    db->auth_set(username, token);
    add_message("Successfully imported identity token!");
    reconnect();
}

void Session::logout() {
    db->auth_select_none();
    reconnect();
}

void Session::login_user(char *username) {
    if (db->auth_select(username)) {
        reconnect();
    } else {
        add_message("Unknown username.");
    }
}

void Session::set_draw_distance(int radius) {
    if (radius >= 1 && radius <= 24) {
        player->set_draw_distance(radius);
    } else {
        add_message("Viewing distance must be between 1 and 24.");
    }
}

void Session::add_message(const char *text) { chat->add_message(text); }

void Session::talk(const char *text) { client->talk(text); }

void Session::show_chat(bool cmd) { chat->show(cmd); }

void Session::close_interface(Interface *iface) {
    auto index = std::find_if(interfaces.begin(), interfaces.end(), [&](auto &i) { return i.get() == iface; });
    if (index != interfaces.end())
        interfaces.erase(index);
}
