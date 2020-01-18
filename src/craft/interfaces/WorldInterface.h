#ifndef CRAFT_SRC_CRAFT_SESSION_PLAYERINTERFACE_H_
#define CRAFT_SRC_CRAFT_SESSION_PLAYERINTERFACE_H_

#include "craft/draw/Shader.h"
#include "craft/interfaces/Interface.h"
#include "craft/world/State.h"

/// Controls
#define INVERT_MOUSE 0
#define MOVE_SENSITIVITY 0.0025 // Sensitivity for look movement with mouse
#define KEY_SENSITIVITY 1.0     // Sensitivity for look movement with keys
#define SCROLL_THRESHOLD 0.1

/// Keys

/// Speed
#define FLYING_SPEED 20
#define WALKING_SPEED 5

struct Player;
struct Session;
struct World;

struct WorldInterface : public Interface {
  public:
    explicit WorldInterface(Session *session, World *world, Player *player);

    bool on_scroll(double dx, double dy) override;
    bool on_key_press(Key key, int scancode, ButtonMods mods) override;
    bool on_mouse_button(MouseButton button, MouseAction action, ButtonMods mods) override;
    bool mouse_movement(double x, double y, double dx, double dy, double dt) override;
    bool held_keys(double dt) override;

    bool render(bool top) override;

  private:
    void on_light();
    void on_left_click();
    void on_right_click();
    void on_middle_click();

    Player *player;
    World *world;
    Player *observe1 = nullptr;
    Player *observe2 = nullptr;
};

#endif // CRAFT_SRC_CRAFT_SESSION_PLAYERINTERFACE_H_
