#ifndef CRAFT_SRC_CRAFT_WORLD_VEC_H_
#define CRAFT_SRC_CRAFT_WORLD_VEC_H_

#include "craft/world/Chunk.h"

struct ChunkPos {
  int x;
  int z;
  bool operator==(const ChunkPos &rhs) const { return x == rhs.x && z == rhs.z; }
  bool operator!=(const ChunkPos &rhs) const { return x != rhs.x || z != rhs.z; }

  template<int radius>
  void surrounding(std::function<void(ChunkPos)> func) const {
    for (int dx = -radius; dx <= radius; ++dx) {
        for (int dz = -radius; dz <= radius; ++dz) {
            func({x + dx, z + dz});
        }
    }
  }
};

inline int chunk_distance(ChunkPos a, ChunkPos b) {
    int dx = ABS(a.x - b.x);
    int dz = ABS(b.z - b.z);
    return MAX(dx, dz);
}

template <typename Index> struct Vec {
  using Self = Vec<Index>;

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

  void set_pos(const Vec<Index> &rhs) { x = rhs.x; y = rhs.y; z = rhs.z; }

  template <int radius>
  void horizontal(std::function<void(Self)> func) const {
      for (int dx = -radius; dx <= radius; ++dx) {
          for (int dz = -radius; dz <= radius; ++dz) {
              func({x + dx, z + dz});
          }
      }
  }

  float len() const { return sqrtf(x * x + y * y + z * z); }
  Vec<Index> &loc() const { return {x, y, z}; }
  ChunkPos chunk() const { return {chunked(x), chunked(z)}; }
};

using FVec = Vec<float>;
using FLoc = Vec<float>;
using ILoc = Vec<int>;

template <typename A, typename B>
float distance(const Vec<A> &a, const Vec<B> &b) {
    return sqrtf(powf(a.x - b.x, 2) + powf(a.y - b.y, 2) + powf(a.z - b.z, 2));
}

static const Vec<int> Left {-1, 0, 0};
static const Vec<int> Right {1, 0, 0};
static const Vec<int> Back {0, 0, -1};
static const Vec<int> Front {0, 0, 1};
static const Vec<int> Up {0, -1, 0};
static const Vec<int> Down {0, 1, 0};

static const Vec<int> Backward {0, 0, -1};
static const Vec<int> Forward {0, 0, 1};
static const Vec<int> Top {0, -1, 0};
static const Vec<int> Bottom {0, 1, 0};

#endif //CRAFT_SRC_CRAFT_WORLD_VEC_H_
