#include "WorldInterface.h"

#include <cmath>

#include "GLFW/glfw3.h"
#include "GL/glew.h"

#include "craft/draw/Crosshairs.h"
#include "craft/draw/Render.h"
#include "craft/draw/Text.h"
#include "craft/items/item.h"
#include "craft/physics/Physics.h"
#include "craft/player/Player.h"
#include "craft/session/Session.h"
#include "craft/session/Window.h"
#include "craft/world/world.h"

WorldInterface::WorldInterface(Session *session, World *world, Player *player)
 : Interface(session), world(world), player(player) {

}

bool WorldInterface::on_mouse_button(MouseButton button, MouseAction action, ButtonMods mods) {
    if (action != MouseAction::Press) {
        return true;
    }
    bool control = mods.control() || mods.super();
    switch (button) {
        case MouseButton::Left: {
            if (control) on_right_click();
            else on_left_click();
            return true;
        }
        case MouseButton::Right: {
            if (control) on_light();
            else on_right_click();
            return true;
        }
        case MouseButton::Middle: {
            on_middle_click();
            return true;
        }
    }
}

bool WorldInterface::on_scroll(double dx, double dy) {
    static double ypos = 0;
    ypos += dy;
    if (ypos < -SCROLL_THRESHOLD) {
        player->item_index = (player->item_index + 1) % item_count;
        ypos = 0;
    }
    if (ypos > SCROLL_THRESHOLD) {
        player->item_index = (player->item_index == 0) ? item_count - 1 : player->item_index - 1;
        ypos = 0;
    }
    return true;
}


bool WorldInterface::on_key_press(Key key, int scancode, ButtonMods mods) {
    bool control = mods.control() || mods.super();
    if (key == Key::Escape) {
        session->window()->defocus();
        return true;
    } else if (key == player->keys.Chat) {
        session->show_chat(/*cmd*/false);
        return true;
    } else if (key == player->keys.Command) {
        session->show_chat(/*cmd*/true);
        return true;
    } else if (key == player->keys.Sign) {
        // TODO: sign window
        /*if (BlockFace block = world->hit_test_face(player->state)) {
            session->set_sign(block.x, block.y, block.z, block.face, typing_buffer + 1);
        }*/
        return true;
    } else if (key == Key::Enter) {
        if (control) {
            on_right_click();
        } else {
            on_left_click();
        }
        return true;
    } else if (key == player->keys.Fly) {
        player->flying = !player->flying;
        return true;
    } else if (key.value >= '1' && key.value <= '9') {
        player->item_index = key.value - '1';
        return true;
    } else if (key == Key::Key0) {
        player->item_index = 9;
        return true;
    } else if (key == player->keys.Next) {
        player->item_index = (player->item_index + 1) % item_count;
        return true;
    } else if (key == player->keys.Prev) {
        player->item_index = (player->item_index == 0) ? item_count - 1 : player->item_index - 1;
        return true;
    } else if (key == player->keys.Observe) {
        observe1 = world->next_player(observe1);
        return true;
    } else if (key == player->keys.Inset) {
        observe2 = world->next_player(observe2);
        return true;
    }
    return false;
}

bool WorldInterface::mouse_movement(double x, double y, double dx, double dy, double dt) {
    State &s = player->state;

    s.rx += dx * MOVE_SENSITIVITY;
    if (INVERT_MOUSE) {
        s.ry += dy * MOVE_SENSITIVITY;
    }
    else {
        s.ry -= dy * MOVE_SENSITIVITY;
    }
    if (s.rx < 0) {
        s.rx += RADIANS(360);
    }
    if (s.rx >= RADIANS(360)){
        s.rx -= RADIANS(360);
    }
    s.ry = MAX(s.ry, -RADIANS(90));
    s.ry = MIN(s.ry, RADIANS(90));
    return true;
}

bool WorldInterface::held_keys(double dt) {
    int sz = 0;
    int sx = 0;
    float m = dt * KEY_SENSITIVITY;
    State &state = player->state;
    player->ortho = is_key_pressed(player->keys.Ortho) ? 64 : 0;
    player->fov = is_key_pressed(player->keys.Zoom) ? 15 : 65;
    if (is_key_pressed(player->keys.Forward)) sz--;
    if (is_key_pressed(player->keys.Backward)) sz++;
    if (is_key_pressed(player->keys.Left)) sx--;
    if (is_key_pressed(player->keys.Right)) sx++;
    if (is_key_pressed(Key::Left)) state.rx -= m;
    if (is_key_pressed(Key::Right)) state.rx += m;
    if (is_key_pressed(Key::Up)) state.ry += m;
    if (is_key_pressed(Key::Down)) state.ry -= m;
    FVec vec = get_motion_vector(player->flying, sz, sx, state.rx, state.ry);
    if (is_key_pressed(player->keys.Jump)) {
        if (player->flying) {
            vec.y = 1;
        }
        else if (vec.y == 0) {
            player->accel.y = 8;
        }
    }
    float speed = player->flying ? FLYING_SPEED : WALKING_SPEED;
    int estimate = roundf(sqrtf(powf(vec.x * speed, 2) +
                                        powf(vec.y * speed + ABS(player->accel.y) * 2, 2) +
                                        powf(vec.z * speed, 2)) * dt * 8);
    int step = MAX(8, estimate);
    float ut = dt / step;
    vec *= ut * speed;
    player->velocity = vec;
    player->step = step;
    return true;
}

void WorldInterface::on_light() {
    Block block = world->hit_test(player->state);
    if (block.y > 0 && block.y < 256 && is_destructable(block.w)) {
        session->toggle_light(block.x, block.y, block.z);
    }
}

void WorldInterface::on_left_click() {
    Block block = world->hit_test(player->state);
    if (block.y > 0 && block.y < 256 && is_destructable(block.w)) {
        session->set_block(block.x, block.y, block.z, 0);
        if (is_plant(world->get_block(block.x, block.y + 1, block.z))) {
            session->set_block(block.x, block.y + 1, block.z, 0);
        }
    }
}

void WorldInterface::on_right_click() {
    Block block = world->hit_test(player->state);
    if (block.y > 0 && block.y < 256 && is_obstacle(block.w)) {
        if (!player_intersects_block(2, player->state, block)) {
            session->set_block(block.x, block.y, block.z, items[player->item_index]);
        }
    }
}

void WorldInterface::on_middle_click() {
    Block block = world->hit_test(player->state);
    for (int i = 0; i < item_count; i++) {
        if (items[i] == block.w) {
            player->item_index = i;
            break;
        }
    }
}

bool WorldInterface::render(bool top) {
    int height = window_height();
    int width = window_width();
    int scale = window_scale();
    int ts = 0;

    // RENDER 3-D SCENE //
    Player *target = observe1 ? observe1 : player;
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    session->render_sky(Render::sky(), target, width, height);
    glClear(GL_DEPTH_BUFFER_BIT);
    session->render_chunks(Render::block(), target, width, height);
    session->render_signs(Render::text(), target, width, height);
    session->render_players(Render::text(), target, width, height);
    if (SHOW_WIREFRAME) {
        session->render_wireframe(Render::line(), target, width, height);
    }
    glClear(GL_DEPTH_BUFFER_BIT);

    // RENDER HUD //
    if (SHOW_CROSSHAIRS) {
        render_crosshairs(session->window(), Render::line());
    }
    if (SHOW_ITEM) {
        render_item(Render::block(), world, target);
    }

    if (SHOW_PLAYER_NAMES) {
        if (observe1 != player) {
            render_text(session->window(), Render::text(), Justify::Center, width / 2, ts, ts, observe1->name);
        }
        Player *other = world->closest_player_in_view(observe1);
        if (other) {
            render_text(session->window(), Render::text(), Justify::Center,
                        width / 2, height / 2 - ts - 24, ts,
                        other->name);
        }
    }

    // RENDER PICTURE IN PICTURE //
    if (observe2 && observe2 != player) {
        int pw = 256 * scale;
        int ph = 256 * scale;
        int offset = 32 * scale;
        int pad = 3 * scale;
        int sw = pw + pad * 2;
        int sh = ph + pad * 2;

        glEnable(GL_SCISSOR_TEST);
        glScissor(width - sw - offset + pad, offset - pad, sw, sh);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        glViewport(width - pw - offset, offset, pw, ph);

        session->render_sky(Render::sky(), observe2, pw, ph);
        glClear(GL_DEPTH_BUFFER_BIT);
        session->render_chunks(Render::block(), observe2, pw, ph);
        session->render_signs(Render::text(), observe2, pw, ph);
        session->render_players(Render::block(), observe2, pw, ph);
        glClear(GL_DEPTH_BUFFER_BIT);
        if (SHOW_PLAYER_NAMES) {
            render_text(session->window(), Render::text(), Justify::Center, pw / 2, ts, ts, observe2->name);
        }
    }
    return false;
}


