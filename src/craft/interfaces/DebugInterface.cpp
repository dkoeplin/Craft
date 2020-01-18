#include "DebugInterface.h"

#include <cmath>

#include "craft/draw/Render.h"
#include "craft/draw/Text.h"
#include "craft/player/Player.h"
#include "craft/session/Session.h"
#include "craft/world/World.h"

DebugInterface::DebugInterface(Session *session, World *world, Player *player)
    : Interface(session), world(world), player(player) {}

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
    float ts = 12 * window->scale();
    float tx = ts / 2;
    float ty = window->height() - ts;

    State &s = player->state;
    int hour = world->time_of_day() * 24;
    char am_pm = hour < 12 ? 'a' : 'p';
    hour = hour % 12;
    hour = hour ? hour : 12;
    snprintf(text_buffer, 1024, "(%d, %d) (%.2f, %.2f, %.2f) [%zu, %d, %d] %d%cm %dfps", chunked(s.x), chunked(s.z),
             s.x, s.y, s.z, world->player_count(), world->chunk_count(), window->face_count() * 2, hour, am_pm, fps);
    render_text(window, Render::text(), Justify::Left, tx, ty, ts, text_buffer);

    auto &v = player->velocity;
    auto &a = player->accel;
    auto vm = v.len();
    auto am = a.len();
    snprintf(text_buffer, 1024, "v: %.2f (%.2f, %.2f, %.2f) a: %.2f (%.2f, %.2f, %.2f)", vm, v.x, v.y, v.z, am, a.x, a.y, a.z);
    render_text(window, Render::text(), Justify::Left, tx, ty - 2*ts, ts, text_buffer);
    return false;
}
