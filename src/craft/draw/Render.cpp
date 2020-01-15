#include "Render.h"

#include <string>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "craft/util/util.h"

Attrib Render::block_ = {};
Attrib Render::line_ = {};
Attrib Render::text_ = {};
Attrib Render::sky_ = {};

void Render::init() {
    load_textures();
    load_shaders();
}

void Render::load_textures() {
    const std::string textures = "/Users/davidk/workspace/Craft/textures/";

    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    load_png_texture(textures + "texture.png");

    GLuint font;
    glGenTextures(1, &font);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, font);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    load_png_texture(textures + "font.png");

    GLuint sky;
    glGenTextures(1, &sky);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, sky);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    load_png_texture(textures + "sky.png");

    GLuint sign;
    glGenTextures(1, &sign);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, sign);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    load_png_texture(textures + "sign.png");
}

void Render::load_shaders() {
    const std::string shaders = "/Users/davidk/workspace/Craft/shaders/";
    GLuint program;

    program = load_program(shaders + "block_vertex.glsl", shaders + "block_fragment.glsl");
    block_.program = program;
    block_.position = glGetAttribLocation(program, "position");
    block_.normal = glGetAttribLocation(program, "normal");
    block_.uv = glGetAttribLocation(program, "uv");
    block_.matrix = glGetUniformLocation(program, "matrix");
    block_.sampler = glGetUniformLocation(program, "sampler");
    block_.extra1 = glGetUniformLocation(program, "sky_sampler");
    block_.extra2 = glGetUniformLocation(program, "daylight");
    block_.extra3 = glGetUniformLocation(program, "fog_distance");
    block_.extra4 = glGetUniformLocation(program, "ortho");
    block_.camera = glGetUniformLocation(program, "camera");
    block_.timer = glGetUniformLocation(program, "timer");

    program = load_program(shaders + "line_vertex.glsl", shaders + "line_fragment.glsl");
    line_.program = program;
    line_.position = glGetAttribLocation(program, "position");
    line_.matrix = glGetUniformLocation(program, "matrix");

    program = load_program(shaders + "text_vertex.glsl", shaders + "text_fragment.glsl");
    text_.program = program;
    text_.position = glGetAttribLocation(program, "position");
    text_.uv = glGetAttribLocation(program, "uv");
    text_.matrix = glGetUniformLocation(program, "matrix");
    text_.sampler = glGetUniformLocation(program, "sampler");
    text_.extra1 = glGetUniformLocation(program, "is_sign");

    program = load_program(shaders + "sky_vertex.glsl", shaders + "sky_fragment.glsl");
    sky_.program = program;
    sky_.position = glGetAttribLocation(program, "position");
    sky_.normal = glGetAttribLocation(program, "normal");
    sky_.uv = glGetAttribLocation(program, "uv");
    sky_.matrix = glGetUniformLocation(program, "matrix");
    sky_.sampler = glGetUniformLocation(program, "sampler");
    sky_.timer = glGetUniformLocation(program, "timer");
}







