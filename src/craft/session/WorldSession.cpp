#include "WorldSession.h"

#include "craft/items/Item.h"
#include "craft/multiplayer/Client.h"
#include "craft/support/db.h"
#include "craft/util/Util.h"
#include "craft/world/State.h"
#include "craft/world/World.h"

WorldSession::WorldSession() {
    db = std::make_unique<Database>();
    world = std::make_unique<World>();
    client = std::make_unique<Client>();
}

void WorldSession::unset_sign(const ILoc3 &pos) {
    if (auto *chunk = world->find_chunk(pos.chunk())) {
        if (sign_list_remove_all(chunk->signs, pos)) {
            chunk->dirty = true;
            db->delete_signs(pos);
        }
    } else {
        db->delete_signs(pos);
    }
}

void WorldSession::unset_sign_face(const Face &face) {
    if (auto *chunk = world->find_chunk(face.chunk())) {
        if (sign_list_remove(chunk->signs, face)) {
            chunk->dirty = true;
            db->delete_sign(face);
        }
    } else {
        db->delete_sign(face);
    }
}

void WorldSession::set_block(const ChunkPos &pos, const Block &block, bool dirty) {
    if (auto *chunk = world->find_chunk(pos)) {
        Map *map = &chunk->map;
        if (map_set(map, block.x, block.y, block.z, block.w)) {
            if (dirty) {
                world->mark_chunk_dirty(chunk);
            }
            db->insert_block(pos, block);
        }
    } else {
        db->insert_block(pos, block);
    }
    if (block.w == 0 && block.chunk() == pos) {
        unset_sign(block);
        set_light(pos, block.loc(), 0);
    }
}

void WorldSession::set_block(const Block &block) {
    auto pos = block.chunk();
    set_block(pos, block, true);
    Block update(block.loc(), -block.w);
    block.horizontal<1>([&](ILoc3 bpos2) {
        auto pos2 = bpos2.chunk();
        if (pos2 != pos)
            set_block(pos2, update, true);
    });
    client->block(block);
}

void WorldSession::builder_block(const Block &block) {
    if (block.y <= 0 || block.y >= 256) {
        return;
    }
    if (is_destructable(world->get_block_material(block))) {
        set_block({block.loc(), 0});
    }
    if (block) {
        set_block(block);
    }
}

void WorldSession::set_sign(const ChunkPos &pos, const Sign &sign, bool dirty) {
    if (!sign) {
        unset_sign_face(sign.face());
        return;
    }
    if (auto *chunk = world->find_chunk(pos)) {
        chunk->signs.push_back(sign);
        if (dirty) {
            chunk->dirty = true;
        }
    }
    db->insert_sign(pos, sign);
}

void WorldSession::set_sign(const Sign &sign) {
    set_sign(sign.chunk(), sign, true);
    client->sign(sign);
}

void WorldSession::toggle_light(const ILoc3 &loc) {
    auto pos = loc.chunk();
    if (Chunk *chunk = world->find_chunk(pos)) {
        Map *map = &chunk->lights;
        int w = map_get(map, loc.x, loc.y, pos.z) ? 0 : 15;
        map_set(map, loc.x, loc.y, loc.z, w);
        db->insert_light(pos.x, pos.z, loc.x, loc.y, loc.z, w);
        client->light(loc.x, loc.y, loc.z, w);
        world->mark_chunk_dirty(chunk);
    }
}

void WorldSession::set_light(const ChunkPos &pos, const ILoc3 &loc, int w) {
    if (Chunk *chunk = world->find_chunk(pos)) {
        Map *map = &chunk->lights;
        if (map_set(map, loc.x, loc.y, loc.z, w)) {
            world->mark_chunk_dirty(chunk);
            db->insert_light(pos, loc, w);
        }
    } else {
        db->insert_light(pos, loc, w);
    }
}

void WorldSession::array(Block *b1, Block *b2, int xc, int yc, int zc) {
    if (b1->w != b2->w) {
        return;
    }
    int w = b1->w;
    int dx = b2->x - b1->x;
    int dy = b2->y - b1->y;
    int dz = b2->z - b1->z;
    xc = dx ? xc : 1;
    yc = dy ? yc : 1;
    zc = dz ? zc : 1;
    for (int i = 0; i < xc; i++) {
        int x = b1->x + dx * i;
        for (int j = 0; j < yc; j++) {
            int y = b1->y + dy * j;
            for (int k = 0; k < zc; k++) {
                int z = b1->z + dz * k;
                builder_block({x, y, z, w});
            }
        }
    }
}

void WorldSession::cube(Block *b1, Block *b2, int fill) {
    if (b1->w != b2->w) {
        return;
    }
    int w = b1->w;
    int x1 = MIN(b1->x, b2->x);
    int y1 = MIN(b1->y, b2->y);
    int z1 = MIN(b1->z, b2->z);
    int x2 = MAX(b1->x, b2->x);
    int y2 = MAX(b1->y, b2->y);
    int z2 = MAX(b1->z, b2->z);
    int a = (x1 == x2) + (y1 == y2) + (z1 == z2);
    for (int x = x1; x <= x2; x++) {
        for (int y = y1; y <= y2; y++) {
            for (int z = z1; z <= z2; z++) {
                if (!fill) {
                    int n = 0;
                    n += x == x1 || x == x2;
                    n += y == y1 || y == y2;
                    n += z == z1 || z == z2;
                    if (n <= a) {
                        continue;
                    }
                }
                builder_block({x, y, z, w});
            }
        }
    }
}

void WorldSession::sphere(Block *center, int radius, int fill, int fx, int fy, int fz) {
    static const float offsets[8][3] = {{-0.5, -0.5, -0.5}, {-0.5, -0.5, 0.5}, {-0.5, 0.5, -0.5}, {-0.5, 0.5, 0.5},
                                        {0.5, -0.5, -0.5},  {0.5, -0.5, 0.5},  {0.5, 0.5, -0.5},  {0.5, 0.5, 0.5}};
    int cx = center->x;
    int cy = center->y;
    int cz = center->z;
    int w = center->w;
    for (int x = cx - radius; x <= cx + radius; x++) {
        if (fx && x != cx) {
            continue;
        }
        for (int y = cy - radius; y <= cy + radius; y++) {
            if (fy && y != cy) {
                continue;
            }
            for (int z = cz - radius; z <= cz + radius; z++) {
                if (fz && z != cz) {
                    continue;
                }
                int inside = 0;
                int outside = fill;
                for (int i = 0; i < 8; i++) {
                    float dx = x + offsets[i][0] - cx;
                    float dy = y + offsets[i][1] - cy;
                    float dz = z + offsets[i][2] - cz;
                    float d = sqrtf(dx * dx + dy * dy + dz * dz);
                    if (d < radius) {
                        inside = 1;
                    } else {
                        outside = 1;
                    }
                }
                if (inside && outside) {
                    builder_block({x, y, z, w});
                }
            }
        }
    }
}

void WorldSession::cylinder(Block *b1, Block *b2, int radius, int fill) {
    if (b1->w != b2->w) {
        return;
    }
    int w = b1->w;
    int x1 = MIN(b1->x, b2->x);
    int y1 = MIN(b1->y, b2->y);
    int z1 = MIN(b1->z, b2->z);
    int x2 = MAX(b1->x, b2->x);
    int y2 = MAX(b1->y, b2->y);
    int z2 = MAX(b1->z, b2->z);
    int fx = x1 != x2;
    int fy = y1 != y2;
    int fz = z1 != z2;
    if (fx + fy + fz != 1) {
        return;
    }
    Block block = {x1, y1, z1, w};
    if (fx) {
        for (int x = x1; x <= x2; x++) {
            block.x = x;
            sphere(&block, radius, fill, 1, 0, 0);
        }
    }
    if (fy) {
        for (int y = y1; y <= y2; y++) {
            block.y = y;
            sphere(&block, radius, fill, 0, 1, 0);
        }
    }
    if (fz) {
        for (int z = z1; z <= z2; z++) {
            block.z = z;
            sphere(&block, radius, fill, 0, 0, 1);
        }
    }
}

void WorldSession::tree(Block *block) {
    int bx = block->x;
    int by = block->y;
    int bz = block->z;
    for (int y = by + 3; y < by + 8; y++) {
        for (int dx = -3; dx <= 3; dx++) {
            for (int dz = -3; dz <= 3; dz++) {
                int dy = y - (by + 4);
                int d = (dx * dx) + (dy * dy) + (dz * dz);
                if (d < 11) {
                    builder_block({bx + dx, y, bz + dz, 15});
                }
            }
        }
    }
    for (int y = by; y < by + 7; y++) {
        builder_block({bx, y, bz, 5});
    }
}
