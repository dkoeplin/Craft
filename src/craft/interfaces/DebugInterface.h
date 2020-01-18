
#ifndef CRAFT_SRC_CRAFT_INTERFACES_DEBUGINTERFACE_H_
#define CRAFT_SRC_CRAFT_INTERFACES_DEBUGINTERFACE_H_

#include "craft/interfaces/Interface.h"

struct Player;
struct World;

struct DebugInterface : public Interface {
  public:
    DebugInterface(Session *session, World *world, Player *player);

    bool on_key_press(Key key, int scancode, ButtonMods mods) override;

    bool tick(double dt) override;
    bool render(bool top) override;

  private:
    World *world;
    Player *player;
    unsigned int fps = 0;
    unsigned int frames = 0;
    double since = 0.0;
};

#endif // CRAFT_SRC_CRAFT_INTERFACES_DEBUGINTERFACE_H_
