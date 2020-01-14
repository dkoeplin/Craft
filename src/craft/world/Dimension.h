#ifndef CRAFT_SRC_CRAFT_WORLD_DIMENSION_H_
#define CRAFT_SRC_CRAFT_WORLD_DIMENSION_H_

#include "GL/glew.h"

struct Dimension {
  virtual ~Dimension();
  GLuint sky;
};

struct Earth : public Dimension {
  Earth();
};

#endif //CRAFT_SRC_CRAFT_WORLD_DIMENSION_H_
