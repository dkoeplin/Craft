#ifndef CRAFT_SRC_CRAFT_SESSION_WINDOW_H_
#define CRAFT_SRC_CRAFT_SESSION_WINDOW_H_

#include "Session.h"

typedef struct GLFWwindow GLFWwindow;
struct Key;

struct Window {
  Window();

  bool init(Session *session);

  void update();
  bool in_focus();
  void focus();
  void defocus();

  const char *clipboard();
  void get_cursor_pos(double &x, double &y);
  bool is_key_pressed(const Key &key);

  void swap();
  bool should_close();

  int scale() { return scale_; }
  int width() { return width_; }
  int height() { return height_; }
  int face_count() { return face_count_; }
  void set_face_count(int count) { face_count_ = count; }

 private:
  int scale_ = 1;
  int width_ = 0;
  int height_ = 0;
  int face_count_ = 0;
  GLFWwindow *window_ = nullptr;
};



#endif //CRAFT_SRC_CRAFT_SESSION_WINDOW_H_
