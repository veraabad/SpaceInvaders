#include "util/utility.hpp"
#include <cstdio>

namespace util {

uint32_t rgb_to_uint32(uint8_t r, uint8_t g, uint8_t b) 
{
    return (r << 24) | (g << 16) | (b << 8) | 0xFF;
}

void validate_shader(GLuint shader, const char* file)
{
    static const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    GLsizei length = 0;

    glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);

    if (length > 0) {
        printf("Shader %d(%s) compile error: %s\n",
               shader, (file ? file : ""), buffer);
    }
}

bool validate_program(GLuint program)
{
    static const GLsizei BUFFER_SIZE = 512;
    GLchar buffer[BUFFER_SIZE];
    GLsizei length = 0;

    glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
    if (length > 0) {
        printf("Program %d link error: %s\n", program, buffer);
        return false;
    }
    return true;
}

bool sprite_overlap_check(
    const data::Sprite& sp_a, size_t x_a, size_t y_a,
    const data::Sprite& sp_b, size_t x_b, size_t y_b
){
    if (x_a < x_b + sp_b.width && x_a + sp_a.width > x_b 
        && y_a < y_b + sp_b.height && y_a + sp_b.height > y_b) {
        return true;
    }
    return false;
}

void error_callback(int error, const char* description) 
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

} // util


