#include "Vec3.h"

#include "craft/world/Chunk.h"

int ChunkPos::hash(int max) const { return (ABS(x) ^ ABS(z)) % max; }

int chunk_distance(const ChunkPos &a, const ChunkPos &b) {
    int dx = ABS(a.x - b.x);
    int dz = ABS(b.z - b.z);
    return MAX(dx, dz);
}

template <typename Index> Vec3<Index> &Vec3<Index>::operator+=(const Vec3<Index> &rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

template <typename Index> Vec3<Index> &Vec3<Index>::operator*=(float scale) {
    x *= scale;
    y *= scale;
    z *= scale;
    return *this;
}

template <typename Index> Vec3<Index> Vec3<Index>::operator-(const Vec3<Index> &rhs) {
    Vec3<Index> result;
    result.x = x - rhs.x;
    result.y = y - rhs.y;
    result.z = z - rhs.z;
    return result;
}

template <typename Index> Vec3<Index> Vec3<Index>::operator+(const Vec3<Index> &rhs) {
    Vec3<Index> result;
    result.x = x + rhs.x;
    result.y = y + rhs.y;
    result.z = z + rhs.z;
    return result;
}

template <typename Index> Vec3<Index> Vec3<Index>::operator/(int divisor) const {
    Vec3<Index> result;
    result.x = x / divisor;
    result.y = y / divisor;
    result.z = z / divisor;
    return result;
}

template <typename Index> Vec3<Index> Vec3<Index>::operator*(float scale) const {
    Vec3<Index> result;
    result.x = x * scale;
    result.y = y * scale;
    result.z = z * scale;
    return result;
}

template <typename Index> Vec3<int> Vec3<Index>::round() const {
    Vec3<int> result;
    result.x = roundf(x);
    result.y = roundf(y);
    result.z = roundf(z);
    return result;
}

template <typename Index> Vec3<Index> Vec3<Index>::abs() const {
    Vec3<Index> result;
    result.x = ABS(x);
    result.y = ABS(y);
    result.z = ABS(z);
    return result;
}

template <typename Index> float Vec3<Index>::len() const { return sqrtf(x * x + y * y + z * z); }

template <typename Index> ChunkPos Vec3<Index>::chunk() const { return {chunked(x), chunked(z)}; }

template struct Vec3<int>;
template struct Vec3<float>;
