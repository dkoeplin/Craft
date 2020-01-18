#include "Worker.h"

#include <cmath>

#include "craft/physics/Physics.h"
#include "craft/player/Player.h"
#include "craft/session/Session.h"
#include "craft/session/Window.h"
#include "craft/support/matrix.h"
#include "craft/util/Logging.h"
#include "craft/world/Chunk.h"
#include "craft/world/State.h"
#include "craft/world/World.h"

void WorkerItem::free_maps() {
    for (int a = 0; a < 3; a++) {
        for (int b = 0; b < 3; b++) {
            if (Map *block_map = block_maps[a][b]) {
                map_free(block_map);
                free(block_map);
            }
            if (Map *light_map = light_maps[a][b]) {
                map_free(light_map);
                free(light_map);
            }
        }
    }
}

int worker_run(void *arg) {
    auto *worker = (Worker *)arg;
    bool running = true;
    while (running) {
        mtx_lock(&worker->mtx);
        while (worker->state != Worker::Status::Busy) {
            cnd_wait(&worker->cnd, &worker->mtx);
        }
        mtx_unlock(&worker->mtx);
        WorkerItem *item = &worker->item;
        if (item->load) {
            worker->session->load_chunk(item);
        }
        compute_chunk(item);
        mtx_lock(&worker->mtx);
        worker->state = Worker::Status::Done;
        mtx_unlock(&worker->mtx);
    }
    return 0;
}

Worker::Worker(Session *session, int index) : index(index), session(session) {
    mtx_init(&mtx, mtx_plain);
    cnd_init(&cnd);
    thrd_create(&thrd, worker_run, this);
}

void Worker::ack() {
    REQUIRE(state == Status::Done, "Cannot ack worker before it is complete.");
    state = Status::Idle;
}

Chunk *Worker::update_chunk(World *world) {
    Chunk *chunk = world->find_chunk(item.pos);
    if (chunk) {
        if (item.load) {
            map_free(&chunk->map);
            map_free(&chunk->lights);
            map_copy(&chunk->map, item.block_maps[1][1]);
            map_copy(&chunk->lights, item.light_maps[1][1]);
        }
        generate_chunk(chunk, &item);
    }
    item.free_maps();
    ack();
    return item.load ? chunk : nullptr;
}

void Worker::ensure_chunks(Player *player) {
    State &s = player->state;
    Matrix matrix = Matrix::get3D(session->window(), player, player->render_radius);
    Planes planes = Planes::frustum(player->render_radius, matrix);
    int r = player->create_radius;
    int start = INT_MAX;
    ChunkPos pos = s.chunk();
    ChunkPos best;
    int best_score = start;
    for (int dp = -r; dp <= r; dp++) {
        for (int dq = -r; dq <= r; dq++) {
            ChunkPos current{pos.x + dp, pos.z + dq};
            if (current.hash(WORKERS) != this->index) {
                continue;
            }
            Chunk *chunk = session->world->find_chunk(current);
            if (chunk && !chunk->dirty) {
                continue;
            }
            int distance = MAX(ABS(dp), ABS(dq));
            int invisible = !chunk_visible(planes, current, 0, 256, player->ortho);
            int priority = 0;
            if (chunk) {
                priority = chunk->buffer && chunk->dirty;
            }
            int score = (invisible << 24) | (priority << 16) | distance;
            if (score < best_score) {
                best_score = score;
                best = current;
            }
        }
    }
    if (best_score == start) {
        return;
    }
    if (Chunk *chunk = session->world->force_chunk(best)) {
        bool load = chunk->unloaded;
        if (load) {
            chunk->unloaded = false;
            session->init_chunk(chunk);
        }
        item.pos = chunk->pos;
        item.load = load;
        chunk->pos.surrounding<1>([&](ChunkPos pos2, int dx, int dz) {
            if (Chunk *other = session->world->find_chunk(pos2)) {
                Map *block_map = (Map *)malloc(sizeof(Map));
                Map *light_map = (Map *)malloc(sizeof(Map));
                map_copy(block_map, &other->map);
                map_copy(light_map, &other->lights);
                item.block_maps[dx + 1][dz + 1] = block_map;
                item.light_maps[dx + 1][dz + 1] = light_map;
            } else {
                item.block_maps[dx + 1][dz + 1] = nullptr;
                item.light_maps[dx + 1][dz + 1] = nullptr;
            }
        });
        chunk->dirty = false;
        state = Status::Busy;
        cnd_signal(&cnd);
    }
}
