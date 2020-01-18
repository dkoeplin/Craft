#ifndef _map_h_
#define _map_h_

#include <utility>

#define EMPTY_ENTRY(entry) ((entry)->value == 0)

#define MAP_FOR_EACH(map, ex, ey, ez, ew)                                                                              \
    for (unsigned int i = 0; i <= map->mask; i++) {                                                                    \
        MapEntry *entry = map->data + i;                                                                               \
        if (EMPTY_ENTRY(entry)) {                                                                                      \
            continue;                                                                                                  \
        }                                                                                                              \
        int ex = entry->e.x + map->dx;                                                                                 \
        int ey = entry->e.y + map->dy;                                                                                 \
        int ez = entry->e.z + map->dz;                                                                                 \
        int ew = entry->e.w;

#define END_MAP_FOR_EACH }

typedef union {
    unsigned int value;
    struct {
        unsigned char x;
        unsigned char y;
        unsigned char z;
        char w;
    } e;
} MapEntry;

struct Map {
    int32_t dx;
    int32_t dy;
    int32_t dz;
    uint32_t mask;
    uint32_t size;
    MapEntry *data;
};

void map_alloc(Map *map, int dx, int dy, int dz, int mask);
void map_free(Map *map);
void map_copy(Map *dst, Map *src);
void map_grow(Map *map);
int map_set(Map *map, int x, int y, int z, int w);
int map_get(Map *map, int x, int y, int z);

inline void map_set_func(int x, int y, int z, int w, void *arg) {
    Map *map = (Map *)arg;
    map_set(map, x, y, z, w);
}

#endif
