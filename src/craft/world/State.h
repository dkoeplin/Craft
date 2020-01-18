#ifndef CRAFT_SRC_CRAFT_WORLD_STATE_H_
#define CRAFT_SRC_CRAFT_WORLD_STATE_H_

#include <cmath>

#include "craft/util/Util.h"

#include "craft/world/Vec3.h"

enum class Side {
    Left = 0,   // smaller x
    Right = 1,  // larger x
    Back = 2,   // smaller z
    Front = 3,  // larger z
    Top = 4,    // larger y
    Bottom = 5, // smaller y
};

/// A block in the world. Includes world position and material.
struct Block : public Vec3<int> {
    Block() : Vec3<int>(), w(0) {}
    Block(const Vec3<int> &pos, int w) : Vec3(pos), w(w) {}
    Block(int x, int y, int z, int w) : Vec3(x, y, z), w(w) {}
    operator bool() const { return w > 0; }

    int w;
};

/// A face of a block in the world.
struct Face : public Vec3<int> {
    Face() : Vec3<int>(), side(-1) {}
    Face(const ILoc3 &pos, int face) : Vec3(pos), side(face) {}
    Face(int x, int y, int z, int face) : Vec3(x, y, z), side(face) {}
    operator bool() const { return side > 0; }

    bool operator==(const Face &rhs) const;
    bool operator!=(const Face &rhs) const;

    Face &face() { return *this; }
    const Face &face() const { return *this; }

    int side;
};

/// A face of a block in the world with material information.
struct BlockFace : public Block {
    BlockFace() : Block(), face(-1) {}
    BlockFace(const Vec3<int> &pos, int w, int face) : Block(pos, w), face(face) {}
    BlockFace(const Block &block, int face) : Block(block), face(face) {}
    int face;
};

/// Entity state. Includes floating world position, rotation on the x, y, and z axes.
/// Also includes last update time (for movement interpolation).
struct State : public Vec3<float> {
    State() : Vec3<float>(), rx(0), ry(0), rz(0), t(0) {}
    float rx;
    float ry;
    float rz;
    float t;
};

#endif // CRAFT_SRC_CRAFT_WORLD_STATE_H_
