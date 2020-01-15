#include "craft/session/Session.h"

int main(int argc, char **argv) {
    const char *server = argc >= 2 ? argv[1] : nullptr;
    const char *port = argc >= 3 ? argv[2] : nullptr;

    Session session;
    session.update_server(server, port, false);
    session.run();

    return 0;
}
