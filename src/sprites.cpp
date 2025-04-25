#include "sprites/aliens.hpp"

namespace sprites {

static data::Sprite** animation_frames[3] = {nullptr, nullptr, nullptr};

const data::Sprite ALIEN_SPRITES[6] {
    // Alien 1
    {
        {8, 8}, // width, height
        const_cast<uint8_t*>(ALIEN_SPRITE_1)
    },
    // Alien 2
    {
        {8, 8}, // width, height
        const_cast<uint8_t*>(ALIEN_SPRITE_2)
    },
    // Alien 3
    {
        {11, 8}, // width, height
        const_cast<uint8_t*>(ALIEN_SPRITE_3)
    },
    // Alien 4
    {
        {11, 8}, // width, height
        const_cast<uint8_t*>(ALIEN_SPRITE_4)
    },
    // Alien 5
    {
        {12, 8}, // width, height
        const_cast<uint8_t*>(ALIEN_SPRITE_5)
    },
    // Alien 1
    {
        {12, 8}, // width, height
        const_cast<uint8_t*>(ALIEN_SPRITE_6)
    },
};

const data::Sprite ALIEN_DEATH_SPRITE {
    {13, 7}, // width, height
    const_cast<uint8_t*>(ALIEN_DEATH)
};

data::SpriteAnimation ALIEN_ANIMATIONS[3] = {
     {true, 2, 10, 0, nullptr},
     {true, 2, 10, 0, nullptr},
     {true, 2, 10, 0, nullptr},
};

void initialize_aliens() 
{
    for (size_t i = 0; i < 3; ++i) {
        animation_frames[i] = new data::Sprite*[2];
        animation_frames[i][0] = const_cast<data::Sprite*>(&ALIEN_SPRITES[2 * i]);
        animation_frames[i][1] = const_cast<data::Sprite*>(&ALIEN_SPRITES[2 * i + 1]);
        ALIEN_ANIMATIONS[i].frames = animation_frames[i];
    }
}

void cleanup_aliens() 
{
    for (size_t i = 0; i < 3; ++i) {
        delete[] animation_frames[i];
        animation_frames[i] = nullptr;
    }
}

} // sprites
