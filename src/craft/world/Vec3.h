#ifndef CRAFT_SRC_CRAFT_WORLD_VEC3_H_
#define CRAFT_SRC_CRAFT_WORLD_VEC3_H_

#include <functional>

struct ChunkPos {
    int x = 0;
    int z = 0;
    bool operator==(const ChunkPos &rhs) const { return x == rhs.x && z == rhs.z; }
    bool operator!=(const ChunkPos &rhs) const { return x != rhs.x || z != rhs.z; }

    template <int radius> void surrounding(const std::function<void(ChunkPos)> &func) const {
        for (int dx = -radius; dx <= radius; ++dx) {
            for (int dz = -radius; dz <= radius; ++dz) {
                func({x + dx, z + dz});
            }
        }
    }

    template <int radius> void surrounding(const std::function<void(ChunkPos, int, int)> &func) const {
        for (int dx = -radius; dx <= radius; ++dx) {
            for (int dz = -radius; dz <= radius; ++dz) {
                func({x + dx, z + dz}, dx, dz);
            }
        }
    }

    int hash(int max) const;
};

int chunk_distance(const ChunkPos &a, const ChunkPos &b);

template <typename Index> struct Vec3 {
    Vec3() : x(0), y(0), z(0) {}
    Vec3(Index x, Index y, Index z) : x(x), y(y), z(z) {}
    Index x;
    Index y;
    Index z;

    Vec3<Index> &operator+=(const Vec3<Index> &rhs);
    Vec3<Index> &operator*=(float scale);

    Vec3<Index> operator-(const Vec3<Index> &rhs);
    Vec3<Index> operator+(const Vec3<Index> &rhs);

    Vec3<Index> operator/(int divisor) const;
    Vec3<Index> operator/(float divisor) const;
    Vec3<Index> operator*(float scale) const;

    bool operator==(const Vec3<Index> &rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
    bool operator!=(const Vec3<Index> &rhs) const { return x != rhs.x || y != rhs.y || z != rhs.z; }

    Vec3<int> round() const;
    Vec3<Index> abs() const;

    void set_pos(const Vec3<Index> &rhs) {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
    }

    template <int radius> void horizontal(std::function<void(Vec3<Index>)> func) const {
        for (int dx = -radius; dx <= radius; ++dx) {
            for (int dz = -radius; dz <= radius; ++dz) {
                func({x + dx, y, z + dz});
            }
        }
    }

    float len() const;
    void normalize();

    /// Return a self pointer of Vec type.
    Vec3<Index> &loc() { return *this; }
    const Vec3<Index> &loc() const { return *this; }

    ChunkPos chunk() const;
};

using FVec3 = Vec3<float>;
using FLoc3 = Vec3<float>;
using ILoc3 = Vec3<int>;

template <typename A, typename B> float distance(const Vec3<A> &a, const Vec3<B> &b) {
    return sqrtf(powf(a.x - b.x, 2) + powf(a.y - b.y, 2) + powf(a.z - b.z, 2));
}

static const Vec3<int> Left{-1, 0, 0};
static const Vec3<int> Right{1, 0, 0};
static const Vec3<int> Back{0, 0, -1};
static const Vec3<int> Front{0, 0, 1};
static const Vec3<int> Up{0, -1, 0};
static const Vec3<int> Down{0, 1, 0};

static const Vec3<int> Backward{0, 0, -1};
static const Vec3<int> Forward{0, 0, 1};
static const Vec3<int> Top{0, -1, 0};
static const Vec3<int> Bottom{0, 1, 0};

#endif // CRAFT_SRC_CRAFT_WORLD_VEC3_H_
