#ifndef CRAFT_SRC_CRAFT_SESSION_INTERFACE_H_
#define CRAFT_SRC_CRAFT_SESSION_INTERFACE_H_

#include <string>

#include "craft/interfaces/Keys.h"

struct Shader;
struct Player;
struct Session;
struct Window;
struct World;

enum class MouseButton {
    Left = GLFW_MOUSE_BUTTON_LEFT,
    Middle = GLFW_MOUSE_BUTTON_MIDDLE,
    Right = GLFW_MOUSE_BUTTON_RIGHT
};

enum class MouseAction { Press = GLFW_PRESS, Release = GLFW_RELEASE };

enum class KeyAction { Press = GLFW_PRESS, Repeat = GLFW_REPEAT, Release = GLFW_RELEASE };

struct ButtonMods {
  public:
    explicit ButtonMods(unsigned int value) : value(value) {}

    bool shift() { return value & GLFW_MOD_SHIFT; }
    bool control() { return value & GLFW_MOD_CONTROL; }
    bool alt() { return value & GLFW_MOD_ALT; }
    bool super() { return value & GLFW_MOD_SUPER; }

  private:
    unsigned int value;
};

struct Interface {
  public:
    virtual ~Interface() = default;
    virtual std::string get_name() const = 0;

    virtual bool on_char(uint32_t c) { return false; }
    virtual bool on_key_press(Key key, int scancode, ButtonMods mods) { return false; }
    virtual bool on_key_repeat(Key key, int scancode, ButtonMods mods) { return false; }
    virtual bool on_key_release(Key key, int scancode, ButtonMods mods) { return false; }
    virtual bool on_scroll(double dx, double dy) { return false; }
    virtual bool on_mouse_button(MouseButton button, MouseAction action, ButtonMods mods) { return false; }
    virtual bool mouse_movement(double x, double y, double dx, double dy, double dt) { return false; }
    virtual bool held_keys(double dt) { return false; }

    virtual bool tick(double dt) { return false; };
    virtual bool render(bool top) { return false; };

    void render_item(Shader *attrib, World *world, Player *player);

    void close();

  protected:
    explicit Interface(Session *session);

    bool is_key_pressed(const Key &key);

    Session *session;
    Window *window;
};

#endif // CRAFT_SRC_CRAFT_SESSION_INTERFACE_H_
