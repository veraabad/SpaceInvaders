#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>

namespace data {

#define GAME_MAX_BULLETS 128

/**
 * @brief Represents a rectangular shape with width and height.
 *
 * @var width Width of the rectangle.
 * @var height Height of the rectangle.
 */
struct Rectangle {
    size_t width;
    size_t height;
};

/**
 * @brief Represents a 2D coordinate position.
 *
 * @var x X-coordinate position.
 * @var y Y-coordinate position.
 */
struct Location {
    size_t x;
    size_t y;
};

/**
 * @brief A sprite with a specified width, height, and pixel data.
 * @details Inherits from Rectangle and adds sprite-specific pixel data.
 *
 * @inherit Rectangle
 *    - width: Width of the sprite.
 *    - height: Height of the sprite.
 *
 * @var data Pointer to pixel data of the sprite.
 */
struct Sprite final: Rectangle
{
    uint8_t* data;
};

/**
 * @brief Represents an alien entity with position and type information.
 * @details Inherits from Location and adds alien type information.
 *
 * @inherit Location
 *    - x: X-coordinate position of the alien.
 *    - y: Y-coordinate position of the alien.
 *
 * @var type Type of the alien (see AlienType enum).
 */
struct Alien final: Location
{
    uint8_t type;
};

/**
 * @brief Represents the player entity with position and life information.
 * @details Inherits from Location and adds player-specific attributes.
 *
 * @inherit Location
 *    - x: X-coordinate position of the player.
 *    - y: Y-coordinate position of the player.
 *
 * @var life Current life/health of the player.
 */
struct Player final: Location
{
    size_t life;
};

/**
 * @brief Represents a bullet entity with position and direction information.
 * @details Inherits from Location and adds bullet-specific attributes.
 *
 * @inherit Location
 *    - x: X-coordinate position of the bullet.
 *    - y: Y-coordinate position of the bullet.
 *
 * @var dir Direction of the bullet (-1 for up, 1 for down typically).
 */
struct Bullet final: Location
{
    int dir;
};

/**
 * @brief Represents the main game state with dimensions and game entities.
 * @details Inherits from Rectangle and contains all game entities and state information.
 *
 * @inherit Rectangle
 *    - width: Width of the game area.
 *    - height: Height of the game area.
 *
 * @var num_aliens Number of active aliens.
 * @var num_bullets Number of active bullets.
 * @var aliens Pointer to array of alien entities.
 * @var player The player entity.
 * @var bullets Array of bullet entities, limited by GAME_MAX_BULLETS.
 */
struct Game final: Rectangle
{
    size_t num_aliens;
    size_t num_bullets;
    Alien* aliens;
    Player player;
    Bullet bullets[GAME_MAX_BULLETS];
};

/**
 * @brief Represents an animation sequence for sprites.
 *
 * @var loop Whether the animation should loop.
 * @var num_frames Number of frames in the animation.
 * @var frame_duration Duration of each frame in time units.
 * @var time Current time position in the animation.
 * @var frames Array of pointers to sprite frames.
 */
struct SpriteAnimation
{
    bool loop;
    size_t num_frames;
    size_t frame_duration;
    size_t time;
    Sprite** frames;
};

/**
 * @brief Enumeration of alien types in the game.
 *
 * @var ALIEN_DEAD Dead alien (inactive).
 * @var ALIEN_TYPE_A Alien of type A.
 * @var ALIEN_TYPE_B Alien of type B.
 * @var ALIEN_TYPE_C Alien of type C.
 */
enum AlienType: uint8_t
{
    ALIEN_DEAD   = 0,
    ALIEN_TYPE_A = 1,
    ALIEN_TYPE_B = 2,
    ALIEN_TYPE_C = 3
};

/**
 * @brief A buffer of pixel data with a specified width and height.
 * @details Inherits from Rectangle and adds pixel data storage.
 *
 * @inherit Rectangle
 *    - width: Width of the buffer.
 *    - height: Height of the buffer.
 *
 * @var data Pointer to pixel data of the buffer.
 */
class Buffer 
{
public:
    Buffer(size_t width, size_t height) : width(width), height(height) 
    {
        data = std::vector<uint32_t>(width * height, 0);
    }
    ~Buffer(){}

    size_t getWidth()
    {
        return width;
    }

    size_t getHeight()
    {
        return height;
    }

    uint32_t* getData()
    {
        return data.data();
    }

    std::vector<uint32_t>& getVector()
    {
        return data;
    }

    void clear(uint32_t color) 
    {
        std::fill(data.begin(), data.end(), color);
    }

    void draw_sprite(
        const Sprite& sprite,
        size_t x, size_t y, uint32_t color
    ){
        for (size_t xi = 0; xi < sprite.width; ++xi) {
            for (size_t yi = 0; yi < sprite.height; ++yi) {
                size_t sy = sprite.height - 1 + y - yi;
                size_t sx = x + xi;
                if (sprite.data[yi * sprite.width + xi]
                    && sy < height && sx < width) {
                    data[sy * width + sx] = color;
                }
            }
        }
    }

    void draw_text(
        const data::Sprite& text_spritesheet,
        const char* text,
        size_t x, size_t y,
        uint32_t color)
    {
        size_t xp = x;
        size_t stride = text_spritesheet.width * text_spritesheet.height;
        data::Sprite sprite = text_spritesheet;
        for (const char* charp = text; *charp != '\0'; ++charp) {
            char character = *charp - 32;
            if (character < 0 || character >= 65) {
                continue;
            }
            sprite.data = text_spritesheet.data + character * stride;
            draw_sprite(sprite, xp, y, color);
            xp += sprite.width + 1;
        }
    }

    void draw_number(
        const data::Sprite& number_spritesheet, size_t number,
        size_t x, size_t y,
        uint32_t color
    ){
        uint8_t digits[64];
        size_t num_digits = 0;

        size_t current_number = number;
        do {
            digits[num_digits++] = current_number % 10;
            current_number = current_number / 10;
        } while (current_number > 0);

        size_t xp = x;
        size_t stride = number_spritesheet.width * number_spritesheet.height;
        data::Sprite sprite = number_spritesheet;
        for (size_t i = 0; i < num_digits; ++i) {
            uint8_t digit = digits[num_digits - i - 1];
            sprite.data = number_spritesheet.data + digit * stride;
            draw_sprite(sprite, xp, y, color);
            xp += sprite.width + 1;
        }
    }

private:
    size_t width;
    size_t height;
    std::vector<uint32_t> data;
};

}
