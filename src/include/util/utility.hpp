#pragma once

#include <cstdint>
#include <GLFW/glfw3.h>
#include "data/data.hpp"

namespace util {

uint32_t rgb_to_uint32(uint8_t r, uint8_t g, uint8_t b);

void validate_shader(GLuint shader, const char* file = 0);

bool validate_program(GLuint program);

bool sprite_overlap_check(
    const data::Sprite& sp_a, size_t x_a, size_t y_a,
    const data::Sprite& sp_b, size_t x_b, size_t y_b
);

void error_callback(int error, const char* description);

}
