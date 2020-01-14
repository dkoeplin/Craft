#include "DebugInterface.h"

#include <cmath>

#include "craft/draw/Text.h"
#include "craft/draw/Render.h"
#include "craft/player/Player.h"
#include "craft/session/Session.h"
#include "craft/session/Window.h"
#include "craft/world/world.h"

DebugInterface::DebugInterface(Session *session, World *world, Player *player)
: Interface(session), world(world), player(player) {}

bool DebugInterface::on_key_press(Key key, int scancode, ButtonMods mods) {
    if (key == Key::Escape) {
        close();
        return true;
    }
    return false;
}


bool DebugInterface::tick(double dt) {
    frames++;
    double now = glfwGetTime();
    double elapsed = now - since;
    if (elapsed >= 1) {
        fps = round(frames / elapsed);
        frames = 0;
        since = now;
    }
    return false;
}

bool DebugInterface::render(bool top) {
    char text_buffer[1024];
    float ts = 12 * window_scale();
    float tx = ts / 2;
    float ty = window_height() - ts;

    State &s = player->state;
    int hour = world->time_of_day() * 24;
    char am_pm = hour < 12 ? 'a' : 'p';
    hour = hour % 12;
    hour = hour ? hour : 12;
    snprintf(text_buffer, 1024,
             "(%d, %d) (%.2f, %.2f, %.2f) [%zu, %d, %d] %d%cm %dfps",
             chunked(s.x), chunked(s.z), s.x, s.y, s.z,
             world->player_count(), world->chunk_count(),
             session->window()->face_count() * 2, hour, am_pm, fps);
    render_text(session->window(), Render::text(), Justify::Left, tx, ty, ts, text_buffer);
    return false;
}
