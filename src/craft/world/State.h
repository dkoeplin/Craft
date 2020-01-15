#ifndef CRAFT_SRC_CRAFT_WORLD_STATE_H_
#define CRAFT_SRC_CRAFT_WORLD_STATE_H_

#include <cmath>

#include "craft/util/util.h"

template <typename Index> struct Vec {
    Vec() : x(0), y(0), z(0) {}
    Vec(Index x, Index y, Index z) : x(x), y(y), z(z) {}
    Index x;
    Index y;
    Index z;

    Vec<Index> &operator+=(const Vec<Index> &rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
    Vec<Index> &operator*=(float scale) {
      x *= scale;
      y *= scale;
      z *= scale;
      return *this;
    }

    Vec<Index> &operator=(const Vec<Index> &rhs) {
        if (&rhs == this)
            return *this;
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        return *this;
    }

  Vec<Index> operator-(const Vec<Index> &rhs) {
      Vec<Index> result;
      result.x = x - rhs.x;
      result.y = y - rhs.y;
      result.z = z - rhs.z;
      return result;
  }

  Vec<Index> operator+(const Vec<Index> &rhs) {
      Vec<Index> result;
      result.x = x + rhs.x;
      result.y = y + rhs.y;
      result.z = z + rhs.z;
      return result;
  }

    Vec<Index> operator/(int divisor) const {
      Vec<Index> result;
      result.x = x / divisor;
      result.y = y / divisor;
      result.z = z / divisor;
      return result;
    }

    Vec<Index> operator*(float scale) const {
      Vec<Index> result;
      result.x = x * scale;
      result.y = y * scale;
      result.z = z * scale;
      return result;
    }

    bool operator==(const Vec<Index> &rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
    bool operator!=(const Vec<Index> &rhs) const { return x != rhs.x || y != rhs.y || z != rhs.z; }

    Vec<int> round() const {
        Vec<int> result;
        result.x = roundf(x);
        result.y = roundf(y);
        result.z = roundf(z);
        return result;
    }
    Vec<Index> abs() const {
        Vec<Index> result;
        result.x = ABS(x);
        result.y = ABS(y);
        result.z = ABS(z);
        return result;
    }

    float len() const { return sqrtf(x * x + y * y + z * z); }
};

using FPos = Vec<float>;
using IPos = Vec<int>;
using FVec = Vec<float>;

template <typename A, typename B>
float distance(const Vec<A> &a, const Vec<B> &b) {
    return sqrtf(powf(a.x - b.x, 2) + powf(a.y - b.y, 2) + powf(a.z - b.z, 2));
}

/*struct Face {
  enum Value {
    Left = 0,   // smaller x
    Right = 1,  // larger x
    Back = 2,   // smaller z
    Front = 3,  // larger z
    Top = 4,
    Bottom = 5,
  };

  Face(Value value) : value(value) {}
  operator Value() { return value; }

  bool operator==(const Face &rhs) const { return value == rhs.value; }
  bool operator!=(const Face &rhs) const { return value != rhs.value; }
  bool operator==(const Value &rhs) const { return value == rhs; }
  bool operator!=(const Value &rhs) const { return value != rhs; }

  Value value;
};*/

static const Vec<int> LeftFace {-1, 0, 0};
static const Vec<int> RightFace {1, 0, 0};
static const Vec<int> BackFace {0, 0, -1};
static const Vec<int> FrontFace {0, 0, 1};
static const Vec<int> BottomFace {0, -1, 0};
static const Vec<int> TopFace {0, 1, 0};

struct BlockFace;
struct Block : public Vec<int> {
  Block() : Vec(), w(0) {}
  Block(Vec<int> &pos, int w) : Vec(pos), w(w) {}
  Block(int x, int y, int z, int w) : Vec(x, y, z), w(w) {}
  operator bool() { return w > 0; }

  int w;
};

struct BlockFace : public Block {
  BlockFace() : Block(), face(-1) {}
  BlockFace(Vec<int> &pos, int w, int face) : Block(pos, w), face(face) {}
  BlockFace(Block &block, int face) : Block(block), face(face) {}
  int face;
};

struct State : public Vec<float> {
  State() : Vec(), rx(0), ry(0), t(0) {}
  float rx;
  float ry;
  float t;

  Vec<float> pos() const { return {x, y, z}; }
};



#endif //CRAFT_SRC_CRAFT_WORLD_STATE_H_
