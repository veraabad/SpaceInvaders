#pragma once

#include <cstdint>
#include <GLFW/glfw3.h>
#include "data/data.hpp"

namespace util {

uint32_t rgbToUint32(uint8_t r, uint8_t g, uint8_t b);

void validateShader(GLuint shader, const char* file = 0);

bool validateProgram(GLuint program);

bool spriteOverlapCheck(
    const data::Sprite& sp_a, size_t x_a, size_t y_a,
    const data::Sprite& sp_b, size_t x_b, size_t y_b
);

void errorCallback(int error, const char* description);

} // util
