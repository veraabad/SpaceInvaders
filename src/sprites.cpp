#include "sprites/aliens.hpp"
#include "sprites/player.hpp"
#include "sprites/text.hpp"

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

const data::Sprite PLAYER_SPRITE{
    {11, 7}, // width, height
    const_cast<uint8_t*>(PLAYER)
};

const data::Sprite BULLET_SPRITE{
    {1, 3}, // width, height
    const_cast<uint8_t*>(BULLET)
};

const data::Sprite TEXT_SPRITESHEET{
    {5, 7}, // width, height
    const_cast<uint8_t*>(TEXT_SP)
};

const data::Sprite NUMBER_SPRITESHEET{
    {5, 7}, // width, height
    const_cast<uint8_t*>(TEXT_SP + 16 * 35)
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
