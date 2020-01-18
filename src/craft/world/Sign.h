#ifndef _sign_h_
#define _sign_h_

#define MAX_SIGN_LENGTH 64

#include <utility>
#include <vector>
#include <string>

#include "craft/world/State.h"

struct Shader;
struct Chunk;

struct Sign : public Face {
    Sign() = default;
    Sign(const ILoc3 &pos, int dir, std::string text) : Face(pos, dir), text(std::move(text)) {}
    Sign(const Face &face, std::string text) : Face(face), text(std::move(text)) {}
    operator bool() const { return text.empty() || side < 0; }

    std::string text;
};

int sign_list_remove(std::vector<Sign> &list, const Face &face);
int sign_list_remove_all(std::vector<Sign> &list, const ILoc3 &pos);

void gen_sign_buffer(Chunk *chunk);

void draw_signs(Shader *attrib, Chunk *chunk);


#endif
