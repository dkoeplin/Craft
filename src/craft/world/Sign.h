#ifndef _sign_h_
#define _sign_h_

#define MAX_SIGN_LENGTH 64

#include <vector>

struct Attrib;
struct Chunk;

struct Sign {
    int x;
    int y;
    int z;
    int face;
    char text[MAX_SIGN_LENGTH];
};

int sign_list_remove(std::vector<Sign> &list, int x, int y, int z, int face);
int sign_list_remove_all(std::vector<Sign> &list, int x, int y, int z);

void gen_sign_buffer(Chunk *chunk);

void draw_signs(Attrib *attrib, Chunk *chunk);


#endif
