#include "craft/world/World.h"

#include "GL/glew.h"

extern "C" {
#include "noise.h"
}

#include "config.h"

#include "craft/draw/Triangles.h"
#include "craft/support/matrix.h"

#include "craft/draw/Shader.h"
#include "craft/items/Item.h"
#include "craft/physics/Physics.h"
#include "craft/player/Player.h"
#include "craft/session/Worker.h"
#include "craft/world/Chunk.h"
#include "craft/world/Dimension.h"
#include "craft/world/Sign.h"

void World::reset() {
    day_length = DAY_LENGTH;
    glfwSetTime(day_length / 3.0);
    players.clear();
    chunks.clear();
    dimension = std::make_unique<Earth>();
}

Player *World::add_player(int id) {
    auto player = std::make_unique<Player>(id);
    players.push_back(std::move(player));
    return players.back().get();
}

Player *World::next_player(Player *p) {
    if (players.empty())
        return nullptr;
    if (p == nullptr)
        return players.front().get();

    auto iter = std::find_if(players.begin(), players.end(), [&](auto &player) { return player.get() == p; });
    if (iter == players.end()) {
        return players.front().get();
    } else {
        ++iter;
        if (iter == players.end())
            return players.front().get();
        else
            return iter->get();
    }
}

void World::interpolate_players() {
    for (auto &player : players) {
        if (player->id != 0)
            interpolate_player(player.get());
    }
}

Player *World::find_player(int64_t id) {
    for (auto &player : players) {
        if (player->id == id)
            return player.get();
    }
    return nullptr;
}

void World::delete_player(int64_t id) {
    auto index = std::find_if(players.begin(), players.end(), [&id](auto &player) { return player->id == id; });
    if (index != players.end())
        players.erase(index);
}

void World::delete_all_players() { players.clear(); }

void World::move_entities(double dt) {
    for (auto &player : players) {
        float ut = dt / player->step;
        for (int i = 0; i < player->step; i++) {
            if (!player->flying) {
                player->accel.y -= MAX(ut * 25, -250);
            }
            player->velocity += (player->accel * ut);
            player->state += player->velocity;

            if (collide(2, player->state)) {
                player->velocity = {};
            }
        }
        if (player->state.y < 0) {
            player->state.y = highest_block(player->state.x, player->state.z) + 2;
        }
    }
}

float World::time_of_day() {
    if (day_length <= 0) {
        return 0.5;
    }
    float t;
    t = glfwGetTime();
    t = t / day_length;
    t = t - (int)t;
    return t;
}

float World::get_daylight() {
    float timer = time_of_day();
    if (timer < 0.5) {
        float t = (timer - 0.25) * 100;
        return 1 / (1 + powf(2, -t));
    } else {
        float t = (timer - 0.85) * 100;
        return 1 - 1 / (1 + powf(2, -t));
    }
}

int World::get_block_material(const ILoc3 &pos) {
    if (Chunk *chunk = find_chunk(pos.chunk())) {
        Map *map = &chunk->map;
        return map_get(map, pos.x, pos.y, pos.z);
    }
    return 0;
}

Block World::get_block(const ILoc3 &pos) {
    Block block(pos, 0);
    if (Chunk *chunk = find_chunk(pos.chunk())) {
        Map *map = &chunk->map;
        block.w = map_get(map, pos.x, pos.y, pos.z);
    }
    return block;
}

Chunk *World::find_chunk(const ChunkPos &pos) {
    for (auto &chunk : chunks) {
        if (chunk->pos == pos) {
            return chunk.get();
        }
    }
    return nullptr;
}

Chunk *World::force_chunk(const ChunkPos &pos) {
    Chunk *chunk = find_chunk(pos);
    if (!chunk && chunks.size() < MAX_CHUNKS) {
        auto new_chunk = std::make_unique<Chunk>(pos);
        chunk = new_chunk.get();
        chunks.push_back(std::move(new_chunk));
    }
    return chunk;
}

void World::delete_all_chunks() { chunks.clear(); }

int World::highest_block(int nx, int nz) {
    int result = -1;
    int p = chunked(nx);
    int q = chunked(nz);
    Chunk *chunk = find_chunk({p, q});
    if (chunk) {
        Map *map = &chunk->map;
        MAP_FOR_EACH(map, ex, ey, ez, ew) {
            if (is_obstacle(ew) && ex == nx && ez == nz) {
                result = MAX(result, ey);
            }
        }
        END_MAP_FOR_EACH;
    }
    return result;
}

void create_world(int p, int q, world_func func, void *arg) {
    int pad = 1;
    for (int dx = -pad; dx < CHUNK_SIZE + pad; dx++) {
        for (int dz = -pad; dz < CHUNK_SIZE + pad; dz++) {
            int flag = 1;
            if (dx < 0 || dz < 0 || dx >= CHUNK_SIZE || dz >= CHUNK_SIZE) {
                flag = -1;
            }
            int x = p * CHUNK_SIZE + dx;
            int z = q * CHUNK_SIZE + dz;
            float f = simplex2(x * 0.01, z * 0.01, 4, 0.5, 2);
            float g = simplex2(-x * 0.01, -z * 0.01, 2, 0.9, 2);
            int mh = g * 32 + 16;
            int h = f * mh;
            int w = 1;
            int t = 12;
            if (h <= t) {
                h = t;
                w = 2;
            }
            // sand and grass terrain
            for (int y = 0; y < h; y++) {
                func(x, y, z, w * flag, arg);
            }
            if (w == 1) {
                if (SHOW_PLANTS) {
                    // grass
                    if (simplex2(-x * 0.1, z * 0.1, 4, 0.8, 2) > 0.6) {
                        func(x, h, z, 17 * flag, arg);
                    }
                    // flowers
                    if (simplex2(x * 0.05, -z * 0.05, 4, 0.8, 2) > 0.7) {
                        int w = 18 + simplex2(x * 0.1, z * 0.1, 4, 0.8, 2) * 7;
                        func(x, h, z, w * flag, arg);
                    }
                }
                // trees
                int ok = SHOW_TREES;
                if (dx - 4 < 0 || dz - 4 < 0 || dx + 4 >= CHUNK_SIZE || dz + 4 >= CHUNK_SIZE) {
                    ok = 0;
                }
                if (ok && simplex2(x, z, 6, 0.5, 2) > 0.84) {
                    for (int y = h + 3; y < h + 8; y++) {
                        for (int ox = -3; ox <= 3; ox++) {
                            for (int oz = -3; oz <= 3; oz++) {
                                int d = (ox * ox) + (oz * oz) + (y - (h + 4)) * (y - (h + 4));
                                if (d < 11) {
                                    func(x + ox, y, z + oz, 15, arg);
                                }
                            }
                        }
                    }
                    for (int y = h; y < h + 7; y++) {
                        func(x, y, z, 5, arg);
                    }
                }
            }
            // clouds
            if (SHOW_CLOUDS) {
                for (int y = 64; y < 72; y++) {
                    if (simplex3(x * 0.01, y * 0.1, z * 0.01, 8, 0.5, 2) > 0.75) {
                        func(x, y, z, 16 * flag, arg);
                    }
                }
            }
        }
    }
}

int World::chunk_has_lights(Chunk *chunk) {
    if (!SHOW_LIGHTS) {
        return 0;
    }
    for (int dp = -1; dp <= 1; dp++) {
        for (int dq = -1; dq <= 1; dq++) {
            Chunk *other = chunk;
            if (dp || dq) {
                other = find_chunk({chunk->pos.x + dp, chunk->pos.z + dq});
            }
            if (!other) {
                continue;
            }
            Map *map = &other->lights;
            if (map->size) {
                return 1;
            }
        }
    }
    return 0;
}

void World::mark_chunk_dirty(Chunk *chunk) {
    chunk->dirty = true;
    if (chunk_has_lights(chunk)) {
        chunk->pos.surrounding<1>([&](ChunkPos pos2) {
            if (Chunk *other = find_chunk(pos2)) {
                other->dirty = true;
            }
        });
    }
}

//// Physics
Block World::hit_test(const State &state, bool use_prev) {
    Block result;
    float best = 0;
    auto cpos = state.chunk();
    Vec3<float> v = get_sight_vector(state.rx, state.ry);
    for (auto &chunk : chunks) {
        if (chunk_distance(chunk->pos, cpos) > 1) {
            continue;
        }
        if (auto block = ::hit_test(&chunk->map, 8, use_prev, state, v)) {
            float d = distance(block, state);
            if (best == 0 || d < best) {
                best = d;
                result = block;
            }
        }
    }
    return result;
}

BlockFace World::hit_test_face(const State &state) {
    Block b = hit_test(state, false);
    int face = -1;
    if (is_obstacle(b.w)) {
        Block b2 = hit_test(state, true);
        Vec3<int> delta = b2 - b;

        if (delta == Left) {
            face = 0;
        }
        if (delta == Right) {
            face = 1;
        }
        if (delta == Back) {
            face = 2;
        }
        if (delta == Front) {
            face = 3;
        }
        if (delta == Top) {
            int degrees = roundf(DEGREES(atan2f(state.x - b2.x, state.z - b2.z)));
            if (degrees < 0) {
                degrees += 360;
            }
            int top = ((degrees + 45) / 90) % 4;
            face = 4 + top;
        }
        return {b, face};
    }
    return {};
}

bool World::collide(int height, State &state) {
    if (Chunk *chunk = find_chunk(state.chunk())) {
        Map *map = &chunk->map;
        int nx = roundf(state.x);
        int ny = roundf(state.y);
        int nz = roundf(state.z);
        float px = state.x - nx;
        float py = state.y - ny;
        float pz = state.z - nz;
        float pad = 0.25;

        bool result = false;
        for (int dy = 0; dy < height; dy++) {
            if (px < -pad && is_obstacle(map_get(map, nx - 1, ny - dy, nz))) {
                state.x = nx - pad;
            }
            if (px > pad && is_obstacle(map_get(map, nx + 1, ny - dy, nz))) {
                state.x = nx + pad;
            }
            if (py < -pad && is_obstacle(map_get(map, nx, ny - dy - 1, nz))) {
                state.y = ny - pad;
                result = true;
            }
            if (py > pad && is_obstacle(map_get(map, nx, ny - dy + 1, nz))) {
                state.y = ny + pad;
                result = true;
            }
            if (pz < -pad && is_obstacle(map_get(map, nx, ny - dy, nz - 1))) {
                state.z = nz - pad;
            }
            if (pz > pad && is_obstacle(map_get(map, nx, ny - dy, nz + 1))) {
                state.z = nz + pad;
            }
        }
        return result;
    }
    return false;
}

Player *World::closest_player_in_view(Player *player) {
    Player *result = nullptr;
    float threshold = RADIANS(5);
    float best = 0;
    for (auto &other : players) {
        if (other.get() == player) {
            continue;
        }
        float p = player_crosshair_distance(player, other.get());
        float d = player_player_distance(player, other.get());
        if (d < 96 && p / d < threshold) {
            if (best == 0 || d < best) {
                best = d;
                result = other.get();
            }
        }
    }
    return result;
}

void World::delete_chunks(const std::vector<Player *> &observers) {
    auto pend = std::remove_if(chunks.begin(), chunks.end(), [&](auto &chunk) {
        bool should_delete = true;
        for (auto *v : observers) {
            if (chunk_distance(chunk->pos, v->state.chunk()) < v->delete_radius) {
                should_delete = false;
                break;
            }
        }
        return should_delete;
    });
    chunks.erase(pend, chunks.end());
}

void World::render_players(Shader *attrib, Player *player, int width, int height) {
    State *s = &player->state;
    float matrix[16];
    set_matrix_3d(matrix, width, height, s->x, s->y, s->z, s->rx, s->ry, player->fov, player->ortho,
                  player->render_radius);
    glUseProgram(attrib->program);
    glUniformMatrix4fv(attrib->matrix, 1, GL_FALSE, matrix);
    glUniform3f(attrib->camera, s->x, s->y, s->z);
    glUniform1i(attrib->sampler, 0);
    glUniform1f(attrib->timer, time_of_day());

    for (auto &p : players) {
        if (p.get() != player) {
            draw_player(attrib, p.get());
        }
    }
}

int World::render_chunks(Shader *attrib, Player *player, int width, int height) {
    State *s = &player->state;
    auto pos = s->chunk();
    float light = get_daylight();
    auto matrix = Matrix::get3D(width, height, player, player->render_radius);
    auto planes = Planes::frustum(player->render_radius, matrix);
    glUseProgram(attrib->program);
    glUniformMatrix4fv(attrib->matrix, 1, GL_FALSE, matrix.data);
    glUniform3f(attrib->camera, s->x, s->y, s->z);
    glUniform1i(attrib->sampler, 0);
    glUniform1i(attrib->extra1, 2);
    glUniform1f(attrib->extra2, light);
    glUniform1f(attrib->extra3, player->render_radius * CHUNK_SIZE);
    glUniform1i(attrib->extra4, player->ortho);
    glUniform1f(attrib->timer, time_of_day());

    int total_faces = 0;
    for (auto &chunk : chunks) {
        if (chunk_distance(chunk.get()->pos, pos) > player->render_radius) {
            continue;
        }
        if (!chunk->is_visible(planes, player->ortho)) {
            continue;
        }
        draw_chunk(attrib, chunk.get());
        total_faces += chunk->faces;
    }
    return total_faces;
}

void World::render_signs(Shader *attrib, Player *player, int width, int height) {
    State *s = &player->state;
    auto pos = s->chunk();
    auto matrix = Matrix::get3D(width, height, player, player->render_radius);
    auto planes = Planes::frustum(player->render_radius, matrix);
    glUseProgram(attrib->program);
    glUniformMatrix4fv(attrib->matrix, 1, GL_FALSE, matrix.data);
    glUniform1i(attrib->sampler, 3);
    glUniform1i(attrib->extra1, 1);

    for (auto &chunk : chunks) {
        if (chunk_distance(chunk.get()->pos, pos) > player->sign_radius) {
            continue;
        }
        if (!chunk->is_visible(planes, player->ortho)) {
            continue;
        }
        draw_signs(attrib, chunk.get());
    }
}

void World::render_sky(Shader *attrib, Player *player, int width, int height) {
    State s = player->state;
    s.set_pos({0, 0, 0});
    auto matrix = Matrix::get3D(width, height, s, player->fov, false, player->render_radius);
    glUseProgram(attrib->program);
    glUniformMatrix4fv(attrib->matrix, 1, GL_FALSE, matrix.data);
    glUniform1i(attrib->sampler, 2);
    glUniform1f(attrib->timer, time_of_day());
    draw_triangles_3d(attrib, dimension->sky, 512 * 3);
}
