#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <vector>
#include <string>

#include "craft/util/Logging.h"
#include "craft/util/Util.h"

#define LODEPNG_NO_COMPILE_CPP
extern "C" {
#include "lodepng.h"
}

int rand_int(int n) {
    int result;
    while (n <= (result = rand() / (RAND_MAX / n)));
    return result;
}

double rand_double() {
    return (double)rand() / (double)RAND_MAX;
}

char *load_file(const char *path) {
    FILE *file = fopen(path, "rb");
    REQUIRE(file, "fopen " << std::string(path) << " failed. " << std::string(strerror(errno)));
    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    rewind(file);
    char *data = (char *)calloc(length + 1, sizeof(char));
    fread(data, 1, length, file);
    fclose(file);
    return data;
}

GLuint gen_buffer(GLsizei size, GLfloat *data) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return buffer;
}

void del_buffer(GLuint buffer) {
    glDeleteBuffers(1, &buffer);
}

GLfloat *malloc_faces(int components, int faces) {
    return (GLfloat *)malloc(sizeof(GLfloat) * 6 * components * faces);
}

GLuint gen_faces(int components, int faces, GLfloat *data) {
    GLuint buffer = gen_buffer(
        sizeof(GLfloat) * 6 * components * faces, data);
    free(data);
    return buffer;
}

GLuint make_shader(GLenum type, const char *source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        auto *info = (GLchar *)calloc(length, sizeof(GLchar));
        glGetShaderInfoLog(shader, length, nullptr, info);
        fprintf(stderr, "glCompileShader failed:\n%s\n", info);
        free(info);
    }
    return shader;
}

GLuint load_shader(GLenum type, const char *path) {
    char *data = load_file(path);
    GLuint result = make_shader(type, data);
    free(data);
    return result;
}

GLuint make_program(GLuint shader1, GLuint shader2) {
    GLuint program = glCreateProgram();
    glAttachShader(program, shader1);
    glAttachShader(program, shader2);
    glLinkProgram(program);
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        auto *info = (GLchar *)calloc(length, sizeof(GLchar));
        glGetProgramInfoLog(program, length, nullptr, info);
        fprintf(stderr, "glLinkProgram failed: %s\n", info);
        free(info);
    }
    glDetachShader(program, shader1);
    glDetachShader(program, shader2);
    glDeleteShader(shader1);
    glDeleteShader(shader2);
    return program;
}

GLuint load_program(const std::string &path1, const std::string &path2) {
    GLuint shader1 = load_shader(GL_VERTEX_SHADER, path1.c_str());
    GLuint shader2 = load_shader(GL_FRAGMENT_SHADER, path2.c_str());
    GLuint program = make_program(shader1, shader2);
    return program;
}

void flip_image_vertical(uint8_t *data, uint32_t width, uint32_t height) {
    uint32_t size = width * height * 4;
    uint32_t stride = sizeof(int8_t) * width * 4;
    auto *new_data = (uint8_t *)malloc(sizeof(uint8_t) * size);
    for (uint32_t i = 0; i < height; i++) {
        uint32_t j = height - i - 1;
        memcpy(new_data + j * stride, data + i * stride, stride);
    }
    memcpy(data, new_data, size);
    free(new_data);
}

void load_png_texture(const std::string &file_name) {
    uint32_t error;
    uint8_t *data;
    uint32_t width, height;
    error = lodepng_decode32_file(&data, &width, &height, file_name.c_str());
    REQUIRE(!error, "load_png_texture " << file_name << " failed. " << lodepng_error_text(error));
    flip_image_vertical(data, width, height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    free(data);
}

char *tokenize(char *str, const char *delim, char **key) {
    char *result;
    if (str == nullptr) {
        str = *key;
    }
    str += strspn(str, delim);
    if (*str == '\0') {
        return nullptr;
    }
    result = str;
    str += strcspn(str, delim);
    if (*str) {
        *str++ = '\0';
    }
    *key = str;
    return result;
}

int char_width(char input) {
    static const int lookup[128] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        4, 2, 4, 7, 6, 9, 7, 2, 3, 3, 4, 6, 3, 5, 2, 7,
        6, 3, 6, 6, 6, 6, 6, 6, 6, 6, 2, 3, 5, 6, 5, 7,
        8, 6, 6, 6, 6, 6, 6, 6, 6, 4, 6, 6, 5, 8, 8, 6,
        6, 7, 6, 6, 6, 6, 8,10, 8, 6, 6, 3, 6, 3, 6, 6,
        4, 7, 6, 6, 6, 6, 5, 6, 6, 2, 5, 5, 2, 9, 6, 6,
        6, 6, 6, 6, 5, 6, 6, 6, 6, 6, 6, 4, 2, 5, 7, 0
    };
    return lookup[(int)input];
}

int string_width(const char *input) {
    int result = 0;
    int length = strlen(input);
    for (int i = 0; i < length; i++) {
        result += char_width(input[i]);
    }
    return result;
}

int wrap(const char *input, int max_width, char *output, int max_length) {
    *output = '\0';
    char *text = (char *)malloc(sizeof(char) * (strlen(input) + 1));
    strcpy(text, input);
    int space_width = char_width(' ');
    int line_number = 0;
    char *key1, *key2;
    char *line = tokenize(text, "\r\n", &key1);
    while (line) {
        int line_width = 0;
        char *token = tokenize(line, " ", &key2);
        while (token) {
            int token_width = string_width(token);
            if (line_width) {
                if (line_width + token_width > max_width) {
                    line_width = 0;
                    line_number++;
                    strncat(output, "\n", max_length - strlen(output) - 1);
                }
                else {
                    strncat(output, " ", max_length - strlen(output) - 1);
                }
            }
            strncat(output, token, max_length - strlen(output) - 1);
            line_width += token_width + space_width;
            token = tokenize(nullptr, " ", &key2);
        }
        line_number++;
        strncat(output, "\n", max_length - strlen(output) - 1);
        line = tokenize(nullptr, "\r\n", &key1);
    }
    free(text);
    return line_number;
}