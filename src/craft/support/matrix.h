#ifndef _matrix_h_
#define _matrix_h_

#include "craft/player/Player.h"
#include "craft/world/State.h"

void normalize(float *x, float *y, float *z);
void mat_identity(float *matrix);
void mat_translate(float *matrix, float dx, float dy, float dz);
void mat_rotate(float *matrix, float x, float y, float z, float angle);
void mat_vec_multiply(float *vector, float *a, float *b);
void mat_multiply(float *matrix, float *a, float *b);
void mat_apply(float *data, float *matrix, int count, int offset, int stride);
void frustum_planes(float planes[6][4], int radius, float *matrix);
void mat_frustum(
    float *matrix, float left, float right, float bottom,
    float top, float znear, float zfar);
void mat_perspective(
    float *matrix, float fov, float aspect,
    float near, float far);
void mat_ortho(
    float *matrix,
    float left, float right, float bottom, float top, float near, float far);
void set_matrix_2d(float *matrix, int width, int height);
void set_matrix_3d(
    float *matrix, int width, int height,
    float x, float y, float z, float rx, float ry,
    float fov, bool ortho, int radius);
void set_matrix_item(float *matrix, int width, int height, int scale);

struct Matrix {
 public:
  static Matrix get3D(int width, int height, const State &state, float fov, bool ortho, int radius) {
      Matrix matrix = {};
      set_matrix_3d(matrix.data, width, height, state.x, state.y, state.z, state.rx, state.ry, fov, ortho, radius);
      return matrix;
  }
  static Matrix get3D(int width, int height, Player *player, int radius) {
      auto &state = player->state;
      Matrix matrix = {};
      set_matrix_3d(matrix.data, width, height, state.x, state.y, state.z, state.rx, state.ry, player->fov, player->ortho, radius);
      return matrix;
  }

  Matrix() = default;

  float[16] data;
};

struct Planes {
 public:
  static Planes frustum(int radius, Matrix &matrix) {
      Planes planes = {};
      frustum_planes(planes.data, radius, matrix.data);
      return planes;
  }

  float &operator()(int i, int j) { return data[i][j]; }

  float[6][4] data;
};

#endif
