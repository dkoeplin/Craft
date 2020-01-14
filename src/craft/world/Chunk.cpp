#include "Chunk.h"

extern "C" {
#include "noise.h"
}

#include "craft/draw/cube.h"
#include "craft/draw/Plant.h"
#include "craft/draw/Triangles.h"
#include "craft/items/item.h"
#include "craft/physics/Physics.h"
#include "craft/player/Player.h"
#include "craft/session/Worker.h"
#include "craft/world/Attrib.h"
#include "craft/world/world.h"

Chunk::Chunk(int p, int q) : p(p), q(q), map{}, lights{} {
    int dx = p * CHUNK_SIZE - 1;
    int dy = 0;
    int dz = q * CHUNK_SIZE - 1;
    map_alloc(&map, dx, dy, dz, 0x7fff);
    map_alloc(&lights, dx, dy, dz, 0xf);
}

Chunk::~Chunk() {
    map_free(&map);
    map_free(&lights);
    del_buffer(buffer);
    del_buffer(sign_buffer);
}


void draw_chunk(Attrib *attrib, Chunk *chunk) {
    draw_triangles_3d_ao(attrib, chunk->buffer, chunk->faces * 6);
}

void compute_chunk(WorkerItem *item) {
    char *opaque = (char *)calloc(XZ_SIZE * XZ_SIZE * Y_SIZE, sizeof(char));
    char *light = (char *)calloc(XZ_SIZE * XZ_SIZE * Y_SIZE, sizeof(char));
    char *highest = (char *)calloc(XZ_SIZE * XZ_SIZE, sizeof(char));

    int ox = item->p * CHUNK_SIZE - CHUNK_SIZE - 1;
    int oy = -1;
    int oz = item->q * CHUNK_SIZE - CHUNK_SIZE - 1;

    // check for lights
    int has_light = 0;
    if (SHOW_LIGHTS) {
        for (int a = 0; a < 3; a++) {
            for (int b = 0; b < 3; b++) {
                Map *map = item->light_maps[a][b];
                if (map && map->size) {
                    has_light = 1;
                }
            }
        }
    }

    // populate opaque array
    for (auto &block_map : item->block_maps) {
        for (auto map : block_map) {
            if (!map) {
                continue;
            }
            MAP_FOR_EACH(map, ex, ey, ez, ew) {
                    int x = ex - ox;
                    int y = ey - oy;
                    int z = ez - oz;
                    int w = ew;
                    // TODO: this should be unnecessary
                    if (x < 0 || y < 0 || z < 0) {
                        continue;
                    }
                    if (x >= XZ_SIZE || y >= Y_SIZE || z >= XZ_SIZE) {
                        continue;
                    }
                    // END TODO
                    opaque[XYZ(x, y, z)] = !is_transparent(w);
                    if (opaque[XYZ(x, y, z)]) {
                        highest[XZ(x, z)] = MAX(highest[XZ(x, z)], y);
                    }
                } END_MAP_FOR_EACH;
        }
    }

    // flood fill light intensities
    if (has_light) {
        for (int a = 0; a < 3; a++) {
            for (int b = 0; b < 3; b++) {
                Map *map = item->light_maps[a][b];
                if (!map) {
                    continue;
                }
                MAP_FOR_EACH(map, ex, ey, ez, ew) {
                        int x = ex - ox;
                        int y = ey - oy;
                        int z = ez - oz;
                        light_fill(opaque, light, x, y, z, ew, 1);
                    } END_MAP_FOR_EACH;
            }
        }
    }

    Map *map = item->block_maps[1][1];

    // count exposed faces
    int miny = 256;
    int maxy = 0;
    int faces = 0;
    MAP_FOR_EACH(map, ex, ey, ez, ew) {
            if (ew <= 0) {
                continue;
            }
            int x = ex - ox;
            int y = ey - oy;
            int z = ez - oz;
            int f1 = !opaque[XYZ(x - 1, y, z)];
            int f2 = !opaque[XYZ(x + 1, y, z)];
            int f3 = !opaque[XYZ(x, y + 1, z)];
            int f4 = !opaque[XYZ(x, y - 1, z)] && (ey > 0);
            int f5 = !opaque[XYZ(x, y, z - 1)];
            int f6 = !opaque[XYZ(x, y, z + 1)];
            int total = f1 + f2 + f3 + f4 + f5 + f6;
            if (total == 0) {
                continue;
            }
            if (is_plant(ew)) {
                total = 4;
            }
            miny = MIN(miny, ey);
            maxy = MAX(maxy, ey);
            faces += total;
        } END_MAP_FOR_EACH;

    // generate geometry
    GLfloat *data = malloc_faces(10, faces);
    int offset = 0;
    MAP_FOR_EACH(map, ex, ey, ez, ew) {
            if (ew <= 0) {
                continue;
            }
            int x = ex - ox;
            int y = ey - oy;
            int z = ez - oz;
            int f1 = !opaque[XYZ(x - 1, y, z)];
            int f2 = !opaque[XYZ(x + 1, y, z)];
            int f3 = !opaque[XYZ(x, y + 1, z)];
            int f4 = !opaque[XYZ(x, y - 1, z)] && (ey > 0);
            int f5 = !opaque[XYZ(x, y, z - 1)];
            int f6 = !opaque[XYZ(x, y, z + 1)];
            int total = f1 + f2 + f3 + f4 + f5 + f6;
            if (total == 0) {
                continue;
            }
            char neighbors[27] = {0};
            char lights[27] = {0};
            float shades[27] = {0};
            int index = 0;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dz = -1; dz <= 1; dz++) {
                        neighbors[index] = opaque[XYZ(x + dx, y + dy, z + dz)];
                        lights[index] = light[XYZ(x + dx, y + dy, z + dz)];
                        shades[index] = 0;
                        if (y + dy <= highest[XZ(x + dx, z + dz)]) {
                            for (int oy = 0; oy < 8; oy++) {
                                if (opaque[XYZ(x + dx, y + dy + oy, z + dz)]) {
                                    shades[index] = 1.0 - oy * 0.125;
                                    break;
                                }
                            }
                        }
                        index++;
                    }
                }
            }
            float ao[6][4];
            float light[6][4];
            occlusion(neighbors, lights, shades, ao, light);
            if (is_plant(ew)) {
                total = 4;
                float min_ao = 1;
                float max_light = 0;
                for (int a = 0; a < 6; a++) {
                    for (int b = 0; b < 4; b++) {
                        min_ao = MIN(min_ao, ao[a][b]);
                        max_light = MAX(max_light, light[a][b]);
                    }
                }
                float rotation = simplex2(ex, ez, 4, 0.5, 2) * 360;
                make_plant(
                        data + offset, min_ao, max_light,
                        ex, ey, ez, 0.5, ew, rotation);
            }
            else {
                make_cube(
                        data + offset, ao, light,
                        f1, f2, f3, f4, f5, f6,
                        ex, ey, ez, 0.5, ew);
            }
            offset += total * 60;
        } END_MAP_FOR_EACH;

    free(opaque);
    free(light);
    free(highest);

    item->miny = miny;
    item->maxy = maxy;
    item->faces = faces;
    item->data = data;
}

void generate_chunk(Chunk *chunk, WorkerItem *item) {
    chunk->miny = item->miny;
    chunk->maxy = item->maxy;
    chunk->faces = item->faces;
    del_buffer(chunk->buffer);
    chunk->buffer = gen_faces(10, item->faces, item->data);
    gen_sign_buffer(chunk);
}

WorkerItem create_chunk_work_item(World *world, Chunk *chunk) {
    WorkerItem item = {};
    item.p = chunk->p;
    item.q = chunk->q;
    for (int dp = -1; dp <= 1; dp++) {
        for (int dq = -1; dq <= 1; dq++) {
            Chunk *other = chunk;
            if (dp || dq) {
                other = world->find_chunk(chunk->p + dp, chunk->q + dq);
            }
            if (other) {
                item.block_maps[dp + 1][dq + 1] = &other->map;
                item.light_maps[dp + 1][dq + 1] = &other->lights;
            }
            else {
                item.block_maps[dp + 1][dq + 1] = nullptr;
                item.light_maps[dp + 1][dq + 1] = nullptr;
            }
        }
    }
    return item;
}
