#include <craft/player/Player.h>
#include "ChatInterface.h"

#include "craft/session/Session.h"
#include "craft/session/Window.h"
#include "craft/draw/Render.h"
#include "craft/draw/Text.h"
#include "craft/world/World.h"

ChatInterface::ChatInterface(Session *session, World *world)
 : Interface(session), world(world), typing_buffer{0}, messages{{0}} {
    memset(typing_buffer, 0, sizeof(char) * MAX_TEXT_LENGTH);
    memset(messages, 0, sizeof(char) * MAX_MESSAGES * MAX_TEXT_LENGTH);
}

void ChatInterface::add_message(const char *text) {
    printf("%s\n", text);
    snprintf(messages[message_index], MAX_TEXT_LENGTH, "%s", text);
    message_index = (message_index + 1) % MAX_MESSAGES;
}

bool ChatInterface::on_char(uint32_t u) {
    if (u >= 32 && u < 128) {
        char c = (char)u;
        int n = strlen(typing_buffer);
        if (n < MAX_TEXT_LENGTH - 1) {
            typing_buffer[n] = c;
            typing_buffer[n + 1] = '\0';
        }
    }
    return true;
}

bool ChatInterface::on_key_press(Key key, int scancode, ButtonMods mods) {
    bool control = mods.control() || mods.super();
    if (key == Key::Backspace) {
        int n = strlen(typing_buffer);
        if (n > 0) {
            typing_buffer[n - 1] = '\0';
        }
    } else if (key == Key::Escape) {
        hide();
    } else if (key == Key::Enter) {
        if (mods.shift()) {
            int n = strlen(typing_buffer);
            if (n < MAX_TEXT_LENGTH - 1) {
                typing_buffer[n] = '\r';
                typing_buffer[n + 1] = '\0';
            }
        } else if (typing_buffer[0] == '/') {
            if (!parse_command(typing_buffer)) {
                session->talk(typing_buffer);
            }
        } else {
            session->talk(typing_buffer);
        }
    } else if (control && key == Key::V) {
        const char *buffer = window->clipboard();
        session->suppress_char();
        strncat(typing_buffer, buffer, MAX_TEXT_LENGTH - strlen(typing_buffer) - 1);
    }
    return true;
}

bool ChatInterface::held_keys(double dt) {
    return visible;
}

bool ChatInterface::render(bool top) {
    float ts = 12 * window->scale();
    float tx = ts / 2;
    float ty = window->height() - ts;

    for (int i = 0; i < MAX_MESSAGES; i++) {
        int index = (message_index + i) % MAX_MESSAGES;
        if (strlen(messages[index])) {
            render_text(window, Render::text(), Justify::Left, tx, ty, ts, messages[index]);
            ty -= ts * 2;
        }
    }

    if (visible) {
        char text_buffer[1024];
        snprintf(text_buffer, 1024, "> %s", typing_buffer);
        render_text(window, Render::text(), Justify::Left, tx, ty, ts, text_buffer);
    }
    return false;
}

void ChatInterface::copy() {
    copy0 = block0;
    copy1 = block1;
}

void ChatInterface::paste() {
    Block *c1 = &copy1;
    Block *c2 = &copy0;
    Block *p1 = &block1;
    Block *p2 = &block0;
    int scx = SIGN(c2->x - c1->x);
    int scz = SIGN(c2->z - c1->z);
    int spx = SIGN(p2->x - p1->x);
    int spz = SIGN(p2->z - p1->z);
    int oy = p1->y - c1->y;
    int dx = ABS(c2->x - c1->x);
    int dz = ABS(c2->z - c1->z);
    for (int y = 0; y < 256; y++) {
        for (int x = 0; x <= dx; x++) {
            for (int z = 0; z <= dz; z++) {
                int w = world->get_block(c1->x + x * scx, y, c1->z + z * scz);
                session->builder_block(p1->x + x * spx, y + oy, p1->z + z * spz, w);
            }
        }
    }
}

bool ChatInterface::parse_command(const char *buffer) {
    char username[128] = {0};
    char token[128] = {0};
    char server_addr[MAX_ADDR_LENGTH];
    char server_port[MAX_ADDR_LENGTH];
    char filename[MAX_PATH_LENGTH];
    int radius, count, xc, yc, zc;
    if (sscanf(buffer, "/identity %128s %128s", username, token) == 2) {
        session->set_identity(username, token);
    }
    else if (strcmp(buffer, "/logout") == 0) {
        session->logout();
    }
    else if (sscanf(buffer, "/login %128s", username) == 1) {
        session->login_user(username);
    }
    else if (sscanf(buffer, "/online %128s %128s", server_addr, server_port) >= 1) {
        session->update_server(server_addr, server_port);
    }
    else if (sscanf(buffer, "/offline %128s", filename) == 1) {
        session->update_world_file(filename);
        session->update_server(nullptr);
    }
    else if (strcmp(buffer, "/offline") == 0) {
        session->update_server(nullptr);
    }
    else if (sscanf(buffer, "/view %d", &radius) == 1) {
        session->set_draw_distance(radius);
    }
    else if (strcmp(buffer, "/copy") == 0) {
        copy();
    }
    else if (strcmp(buffer, "/paste") == 0) {
        paste();
    }
    else if (strcmp(buffer, "/tree") == 0) {
        session->tree(&block0);
    }
    else if (sscanf(buffer, "/array %d %d %d", &xc, &yc, &zc) == 3) {
        session->array(&block1, &block0, xc, yc, zc);
    }
    else if (sscanf(buffer, "/array %d", &count) == 1) {
        session->array(&block1, &block0, count, count, count);
    }
    else if (strcmp(buffer, "/fcube") == 0) {
        session->cube(&block0, &block1, 1);
    }
    else if (strcmp(buffer, "/cube") == 0) {
        session->cube(&block0, &block1, 0);
    }
    else if (sscanf(buffer, "/fsphere %d", &radius) == 1) {
        session->sphere(&block0, radius, 1, 0, 0, 0);
    }
    else if (sscanf(buffer, "/sphere %d", &radius) == 1) {
        session->sphere(&block0, radius, 0, 0, 0, 0);
    }
    else if (sscanf(buffer, "/fcirclex %d", &radius) == 1) {
        session->sphere(&block0, radius, 1, 1, 0, 0);
    }
    else if (sscanf(buffer, "/circlex %d", &radius) == 1) {
        session->sphere(&block0, radius, 0, 1, 0, 0);
    }
    else if (sscanf(buffer, "/fcircley %d", &radius) == 1) {
        session->sphere(&block0, radius, 1, 0, 1, 0);
    }
    else if (sscanf(buffer, "/circley %d", &radius) == 1) {
        session->sphere(&block0, radius, 0, 0, 1, 0);
    }
    else if (sscanf(buffer, "/fcirclez %d", &radius) == 1) {
        session->sphere(&block0, radius, 1, 0, 0, 1);
    }
    else if (sscanf(buffer, "/circlez %d", &radius) == 1) {
        session->sphere(&block0, radius, 0, 0, 0, 1);
    }
    else if (sscanf(buffer, "/fcylinder %d", &radius) == 1) {
        session->cylinder(&block0, &block1, radius, 1);
    }
    else if (sscanf(buffer, "/cylinder %d", &radius) == 1) {
        session->cylinder(&block0, &block1, radius, 0);
    }
    else {
        return false;
    }
    return true;
}
