#include "curl/curl.h"
#include "GLFW/glfw3.h"
#include "craft/session/Session.h"

int main(int argc, char **argv) {
    // INITIALIZATION //
    const char *server = argc >= 2 ? argv[1] : nullptr;
    const char *port = argc >= 3 ? argv[2] : nullptr;

    Session::init();
    Session::current->create_window();
    Session::current->init_workers();
    Session::current->update_server(server, port, false);

    Session::Mode mode = Session::Mode::Running;
    while (mode != Session::Mode::Exiting) {
        Session::current->reconnect();
        Session::current->init_database();
        Session::current->load_world();

        while (mode == Session::Mode::Running) {
            Session::current->tick();
            mode = Session::current->render();
        }

        Session::current->shutdown();
    }

    glfwTerminate();
    curl_global_cleanup();
    return 0;
}
