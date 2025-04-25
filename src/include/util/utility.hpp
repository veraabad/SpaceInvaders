#pragma once

#include <cstdint>
#include <GLFW/glfw3.h>
#include "data/data.hpp"

namespace util {

void buffer_sprite_draw(
    data::Buffer* buffer, const data::Sprite& sprite,
    size_t x, size_t y, uint32_t color
);

uint32_t rgb_to_uint32(uint8_t r, uint8_t g, uint8_t b);

void buffer_clear(data::Buffer* buffer, uint32_t color);

void validate_shader(GLuint shader, const char* file = 0);

bool validate_program(GLuint program);

bool sprite_overlap_check(
    const data::Sprite& sp_a, size_t x_a, size_t y_a,
    const data::Sprite& sp_b, size_t x_b, size_t y_b
);

void buffer_draw_text(
    data::Buffer* buffer,
    const data::Sprite& text_spritesheet,
    const char* text,
    size_t x, size_t y,
    uint32_t color);

void buffer_draw_number(
    data::Buffer* buffer,
    const data::Sprite& number_spritesheet, size_t number,
    size_t x, size_t y,
    uint32_t color
);

void error_callback(int error, const char* description);

}
