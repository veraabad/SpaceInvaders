#pragma once
#include <cstddef>
#include <cstdint>

namespace data {

#define GAME_MAX_BULLETS 128

struct Rectangle {
    size_t width;
    size_t height;
};

struct Location {
    size_t x;
    size_t y;
};

struct Buffer final: Rectangle
{
    uint32_t* data;
};

struct Sprite final: Rectangle
{
    uint8_t* data;
};

struct Alien final: Location
{
    uint8_t type;
};

struct Player final: Location
{
    size_t life;
};

struct Bullet final: Location
{
    int dir;
};

struct Game final: Rectangle
{
    size_t num_aliens;
    size_t num_bullets;
    Alien* aliens;
    Player player;
    Bullet bullets[GAME_MAX_BULLETS];
};

struct SpriteAnimation
{
    bool loop;
    size_t num_frames;
    size_t frame_duration;
    size_t time;
    Sprite** frames;
};

enum AlienType: uint8_t
{
    ALIEN_DEAD   = 0,
    ALIEN_TYPE_A = 1,
    ALIEN_TYPE_B = 2,
    ALIEN_TYPE_C = 3
};

}

