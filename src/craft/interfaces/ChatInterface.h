
#ifndef CRAFT_SRC_CRAFT_INTERFACES_CHATINTERFACE_H_
#define CRAFT_SRC_CRAFT_INTERFACES_CHATINTERFACE_H_

#include "craft/interfaces/Interface.h"

#include "craft/session/Session.h"

struct Player;
struct World;

struct ChatInterface : public Interface {
  public:
    static std::string name() { return "Chat"; }
    std::string get_name() const override { return name(); }

    ChatInterface(Session *session, World *world, Player *player);

    bool on_char(uint32_t u) override;
    bool on_key_press(Key key, int scancode, ButtonMods mods) override;
    bool held_keys(double dt) override;

    bool render(bool top) override;

    void add_message(const char *text);

    void show(bool cmd) {
        visible = true;
        if (cmd) {
            typing_buffer[0] = '/';
            typing_buffer[1] = '\0';
        } else {
            typing_buffer[0] = '\0';
        }
    }
    void hide() { visible = false; }

  private:
    bool parse_command(const char *buffer);
    void copy();
    void paste();

    Player *player;
    World *world;

    /// Copy Pasting
    Block block0;
    Block block1;
    Block copy0;
    Block copy1;

    /// Chat/Commands/Messages
    char typing_buffer[MAX_TEXT_LENGTH];
    char messages[MAX_MESSAGES][MAX_TEXT_LENGTH];
    int message_index = 0;
    bool visible = false;
};

#endif // CRAFT_SRC_CRAFT_INTERFACES_CHATINTERFACE_H_
