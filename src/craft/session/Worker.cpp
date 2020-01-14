#include "Worker.h"

#include <cmath>

#include "craft/physics/Physics.h"
#include "craft/player/Player.h"
#include "craft/session/Session.h"
#include "craft/session/Window.h"
#include "craft/support/matrix.h"
#include "craft/util/Logging.h"
#include "craft/world/Chunk.h"
#include "craft/world/world.h"

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
        while (worker->state != Worker::State::Busy) {
            cnd_wait(&worker->cnd, &worker->mtx);
        }
        mtx_unlock(&worker->mtx);
        WorkerItem *item = &worker->item;
        if (item->load) {
            worker->session->load_chunk(item);
        }
        compute_chunk(item);
        mtx_lock(&worker->mtx);
        worker->state = Worker::State::Done;
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
    REQUIRE(state == State::Done, "Cannot ack worker before it is complete.");
    state = State::Idle;
}

Chunk *Worker::update_chunk(World *world) {
    Chunk *chunk = world->find_chunk(item.p, item.q);
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
    auto *s = &player->state;
    float matrix[16];
    set_matrix_3d(
            matrix, session->window()->width(), session->window()->height(),
            s->x, s->y, s->z, s->rx, s->ry, player->fov, player->ortho, player->render_radius);
    float planes[6][4];
    frustum_planes(planes, player->render_radius, matrix);
    int p = chunked(s->x);
    int q = chunked(s->z);
    int r = player->create_radius;
    int start = 0x0fffffff;
    int best_score = start;
    int best_a = 0;
    int best_b = 0;
    for (int dp = -r; dp <= r; dp++) {
        for (int dq = -r; dq <= r; dq++) {
            int a = p + dp;
            int b = q + dq;
            int cindex = (ABS(a) ^ ABS(b)) % WORKERS;
            if (cindex != this->index) {
                continue;
            }
            Chunk *chunk = session->world->find_chunk(a, b);
            if (chunk && !chunk->dirty) {
                continue;
            }
            int distance = MAX(ABS(dp), ABS(dq));
            int invisible = !chunk_visible(planes, a, b, 0, 256, player->ortho);
            int priority = 0;
            if (chunk) {
                priority = chunk->buffer && chunk->dirty;
            }
            int score = (invisible << 24) | (priority << 16) | distance;
            if (score < best_score) {
                best_score = score;
                best_a = a;
                best_b = b;
            }
        }
    }
    if (best_score == start) {
        return;
    }
    int a = best_a;
    int b = best_b;
    if (Chunk *chunk = session->world->force_chunk(a, b)) {
        bool load = chunk->unloaded;
        if (load) {
            chunk->unloaded = false;
            session->init_chunk(chunk);
        }
        item.p = chunk->p;
        item.q = chunk->q;
        item.load = load;
        for (int dp = -1; dp <= 1; dp++) {
            for (int dq = -1; dq <= 1; dq++) {
                Chunk *other = chunk;
                if (dp || dq) {
                    other = session->world->find_chunk(chunk->p + dp, chunk->q + dq);
                }
                if (other) {
                    Map *block_map = (Map *)malloc(sizeof(Map));
                    Map *light_map = (Map *)malloc(sizeof(Map));
                    map_copy(block_map, &other->map);
                    map_copy(light_map, &other->lights);
                    item.block_maps[dp + 1][dq + 1] = block_map;
                    item.light_maps[dp + 1][dq + 1] = light_map;
                }
                else {
                    item.block_maps[dp + 1][dq + 1] = nullptr;
                    item.light_maps[dp + 1][dq + 1] = nullptr;
                }
            }
        }
        chunk->dirty = false;
        state = State::Busy;
        cnd_signal(&cnd);
    }
}

