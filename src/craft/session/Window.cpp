#include "Window.h"

#include "GLFW/glfw3.h"
#include "GL/glew.h"

#include "craft/interfaces/Keys.h"
#include "craft/session/Session.h"
#include "craft/util/Util.h"

void on_key(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto *session = (Session *)glfwGetWindowUserPointer(window);
    return session->on_key(key, scancode, action, mods);
}
void on_char(GLFWwindow *window, uint32_t u) {
    auto *session = (Session *)glfwGetWindowUserPointer(window);
    return session->on_char(u);
}
void on_scroll(GLFWwindow *window, double xdelta, double ydelta) {
    auto *session = (Session *)glfwGetWindowUserPointer(window);
    return session->on_scroll(xdelta, ydelta);
}
void on_mouse_button(GLFWwindow *window, int button, int action, int mods) {
    auto *session = (Session *)glfwGetWindowUserPointer(window);
    return session->on_mouse_button(button, action, mods);
}

Window::Window() = default;

bool Window::init(Session *session) {
    int window_width = WINDOW_WIDTH;
    int window_height = WINDOW_HEIGHT;
    GLFWmonitor *monitor = nullptr;
    if (FULLSCREEN) {
        int mode_count;
        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *modes = glfwGetVideoModes(monitor, &mode_count);
        window_width = modes[mode_count - 1].width;
        window_height = modes[mode_count - 1].height;
    }
    window_ = glfwCreateWindow(window_width, window_height, "Craft", monitor, nullptr);

    if (!window_) {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(VSYNC);
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, ::on_key);
    glfwSetCharCallback(window_, ::on_char);
    glfwSetMouseButtonCallback(window_, ::on_mouse_button);
    glfwSetScrollCallback(window_, ::on_scroll);

    if (glewInit() != GLEW_OK) {
        return false;
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glLogicOp(GL_INVERT);
    glClearColor(0, 0, 0, 1);
    return true;
}

void Window::update() {
    update_scale_factor();
    glfwGetFramebufferSize(window_, &width_, &height_);
    glViewport(0, 0, width_, height_);
}

void Window::update_scale_factor() {
    int window_width, window_height;
    int buffer_width, buffer_height;
    glfwGetWindowSize(window_, &window_width, &window_height);
    glfwGetFramebufferSize(window_, &buffer_width, &buffer_height);
    int result = buffer_width / window_width;
    result = MAX(1, result);
    result = MIN(2, result);
    scale_ = result;
}

void Window::focus() {
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::defocus() {
    if (in_focus()) {
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

bool Window::in_focus() {
    return glfwGetInputMode(window_, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

const char *Window::clipboard() { return glfwGetClipboardString(window_); }

void Window::get_cursor_pos(double &x, double &y) { glfwGetCursorPos(window_, &x, &y); }

bool Window::is_key_pressed(const Key &key) { return glfwGetKey(window_, key.value); }

void Window::swap() {
    glfwSwapBuffers(window_);
    glfwPollEvents();
}

bool Window::should_close() { return glfwWindowShouldClose(window_); }
