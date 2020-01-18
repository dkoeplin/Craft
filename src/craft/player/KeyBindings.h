#ifndef CRAFT_SRC_CRAFT_PLAYER_KEYBINDINGS_H_
#define CRAFT_SRC_CRAFT_PLAYER_KEYBINDINGS_H_

#include "craft/interfaces/Keys.h"

struct KeyBindings {
    /// Chat
    Key Chat = Key::T;
    Key Command = Key::Slash;

    Key Forward = Key::W;
    Key Backward = Key::S;
    Key Left = Key::A;
    Key Right = Key::D;
    Key Jump = Key::Space;
    Key Fly = Key::Tab;
    Key Observe = Key::O;
    Key Inset = Key::P;
    Key Next = Key::E;
    Key Prev = Key::R;
    Key Zoom = Key::LeftShift;
    Key Ortho = Key::F;
    Key Sign = Key::BackTick;
};

#endif // CRAFT_SRC_CRAFT_PLAYER_KEYBINDINGS_H_
