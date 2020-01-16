#ifndef CRAFT_SRC_CRAFT_WORLD_STATE_H_
#define CRAFT_SRC_CRAFT_WORLD_STATE_H_

#include <cmath>

#include "craft/util/Util.h"

#include "craft/world/Vec.h"

enum class Dir {
  Left = 0,   // smaller x
  Right = 1,  // larger x
  Back = 2,   // smaller z
  Front = 3,  // larger z
  Top = 4,    // larger y
  Bottom = 5, // smaller y
};

/// A block in the world. Includes world position and material.
struct Block : public Vec<int> {
  Block() : Vec<int>(), w(0) {}
  Block(Vec<int> &pos, int w) : Vec(pos), w(w) {}
  Block(int x, int y, int z, int w) : Vec(x, y, z), w(w) {}
  operator bool() const { return w > 0; }

  int w;
};

/// A face of a block in the world.
struct Face : public Vec<int> {
  Face() : Vec<int>(), face(-1) {}
  Face(const ILoc &pos, int face) : Vec(pos), face(face) {}
  Face(int x, int y, int z, int face) : Vec(x, y, z), face(face) {}
  operator bool() const { return face > 0; }

  bool operator==(const Face &rhs) const { Vec<int>::operator==(rhs) && face == rhs.face; }
  bool operator!=(const Face &rhs) const { Vec<int>::operator!=(rhs) || face != rhs.face; }

  int face;
};

/// A face of a block in the world with material information.
struct BlockFace : public Block {
  BlockFace() : Block(), face(-1) {}
  BlockFace(Vec<int> &pos, int w, int face) : Block(pos, w), face(face) {}
  BlockFace(Block &block, int face) : Block(block), face(face) {}
  int face;
};

/// Entity state. Includes floating world position, rotation on the x, y, and z axes.
/// Also includes last update time (for movement interpolation).
struct State : public Vec<float> {
  State() : Vec<float>(), rx(0), ry(0), rz(0), t(0) {}
  float rx;
  float ry;
  float rz;
  float t;
};



#endif //CRAFT_SRC_CRAFT_WORLD_STATE_H_
