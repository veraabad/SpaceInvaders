#include <cstdio>
#include <cstdint>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "sprites/aliens.hpp"
#include "sprites/player.hpp"
#include "sprites/text.hpp"
#include "data/data.hpp"
#include "util/utility.hpp"

bool game_running = false;
int move_dir = 0;
bool fire_pressed = 0;
size_t score = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (key) {
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_PRESS) {
                game_running = false;
            }
            break;
        case GLFW_KEY_RIGHT:
            if (action == GLFW_PRESS) {
                move_dir += 1;
            } else if (action == GLFW_RELEASE) {
                move_dir -= 1;
            }
            break;
        case GLFW_KEY_LEFT:
            if (action == GLFW_PRESS) {
                move_dir -= 1;
            } else if (action == GLFW_RELEASE) {
                move_dir += 1;
            }
            break;
        case GLFW_KEY_SPACE:
            if (action == GLFW_RELEASE) {
                fire_pressed = true;
            }
            break;
        default:
            break;
    }
}

int main() 
{
    const size_t buffer_width = 224;
    const size_t buffer_height = 256;

    glfwSetErrorCallback(util::error_callback);

    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(2 * buffer_width, 2* buffer_height, "Space Invaders", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error initializing GLEW.\n");
        glfwTerminate();
        return -1;
    }
    int glVersion[2] = {-1, 1};
    glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
    glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);
    printf("Using OpenGL: %d.%d\n", glVersion[0], glVersion[1]);
    printf("Renderer used: %s\n", glGetString(GL_RENDERER));
    printf("Shading Language: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glfwSwapInterval(1);

    glClearColor(1.0, 0.0, 0.0, 1.0);

    // Create graphics buffer
    data::Buffer buffer;
    buffer.width = buffer_width;
    buffer.height = buffer_height;
    buffer.data = new uint32_t[buffer.width * buffer.height];

    util::buffer_clear(&buffer, 0);

    // Create texture for presenting buffer to OpenGL
    GLuint buffer_texture;
    glGenTextures(1, &buffer_texture);
    glBindTexture(GL_TEXTURE_2D, buffer_texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB8,
        buffer.width, buffer.height, 0,
        GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer.data
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create vao for generating fullscreen triangle
    GLuint fullscreen_triangle_vao;
    glGenVertexArrays(1, &fullscreen_triangle_vao);

    // Create shader for displaying buffer
    const char* vertex_shader = 
        "\n"
        "#version 330\n"
        "\n"
        "noperspective out vec2 TexCoord;\n"
        "\n"
        "void main(void){\n"
        "\n"
        "    TexCoord.x = (gl_VertexID == 2) ? 2.0 : 0.0;\n"
        "    TexCoord.y = (gl_VertexID == 1) ? 2.0 : 0.0;\n"
        "    \n"
        "    gl_Position = vec4(2.0 * TexCoord - 1.0, 0.0, 1.0);\n"
        "}\n";
    const char* fragment_shader = 
        "\n"
        "#version 330\n"
        "\n"
        "uniform sampler2D buffer;\n"
        "noperspective in vec2 TexCoord;\n"
        "\n"
        "out vec3 outColor;\n"
        "\n"
        "void main(void) {\n"
        "    outColor = texture(buffer, TexCoord).rgb;\n"
        "}\n";

    GLuint shader_id = glCreateProgram();
    // Create vertex shader
    {
        GLuint shader_vp = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(shader_vp, 1, &vertex_shader, 0);
        glCompileShader(shader_vp);
        util::validate_shader(shader_vp, vertex_shader);
        glAttachShader(shader_id, shader_vp);

        glDeleteShader(shader_vp);
    }

    // Create fragment shader
    {
        GLuint shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(shader_fp, 1, &fragment_shader, 0);
        glCompileShader(shader_fp);
        util::validate_shader(shader_fp, fragment_shader);
        glAttachShader(shader_id, shader_fp);

        glDeleteShader(shader_fp);
    }

    glLinkProgram(shader_id);

    if (!util::validate_program(shader_id)) {
        fprintf(stderr, "Error while validating shader.\n");
        glfwTerminate();
        glDeleteVertexArrays(1, &fullscreen_triangle_vao);
        delete[] buffer.data;
        return -1;
    }

    glUseProgram(shader_id);

    GLint location = glGetUniformLocation(shader_id, "buffer");
    glUniform1i(location, 0);

    // OpenGL setup
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(fullscreen_triangle_vao);

    // Prepare game
    sprites::initialize_aliens();

    data::Game game;
    game.width = buffer_width;
    game.height = buffer_height;
    game.num_aliens = 55;
    game.num_bullets = 0;
    game.aliens = new data::Alien[game.num_aliens];

    game.player.x = 112 - 5;
    game.player.y = 32;

    game.player.life = 3;

    for (size_t yi = 0; yi < 5; ++yi) {
        for (size_t xi = 0; xi < 11; ++xi) {
            data::Alien& alien = game.aliens[yi * 11 + xi];
            alien.type = (5 - yi) / 2 + 1;
            
            const data::Sprite& sprite = sprites::ALIEN_SPRITES[2 * (alien.type - 1)];

            alien.x = 16 * xi + 20 + (sprites::ALIEN_DEATH_SPRITE.width - sprite.width) / 2;
            alien.y = 17 * yi + 128;
        }
    }

    uint8_t* death_counters = new uint8_t[game.num_aliens];
    for (size_t i = 0; i < game.num_aliens; ++i) {
        death_counters[i] = 10;
    }

    uint32_t clear_color = util::rgb_to_uint32(0, 128, 0);

    game_running = true;

    int player_move_dir = 0;
    // Game loop
    while (!glfwWindowShouldClose(window) & game_running) {
        util::buffer_clear(&buffer, clear_color);

        util::buffer_draw_text(
            &buffer, 
            sprites::TEXT_SPRITESHEET, "SCORE",
            4, game.height - sprites::TEXT_SPRITESHEET.height - 7,
            util::rgb_to_uint32(128, 0, 0)
        );

        util::buffer_draw_number(
            &buffer,
            sprites::NUMBER_SPRITESHEET, score,
            4 + 2 * sprites::NUMBER_SPRITESHEET.width, game.height - 2 * sprites::NUMBER_SPRITESHEET.height - 12,
            util::rgb_to_uint32(128, 0, 0)
        );

        util::buffer_draw_text(
            &buffer, 
            sprites::TEXT_SPRITESHEET, "CREDIT 00",
            164, 7,
            util::rgb_to_uint32(128, 0, 0)
        );

        // Line at bottom
        for (size_t i = 0; i < game.width; ++i) {
            buffer.data[game.width * 16 + i] = util::rgb_to_uint32(128, 0, 0);
        }

        // Draw aliens
        for (size_t ai = 0; ai < game.num_aliens; ++ai) {
            if (!death_counters[ai]) {
                // Dead alien; don't draw
                continue;
            }

            const data::Alien& alien = game.aliens[ai];
            if (alien.type == data::ALIEN_DEAD) {
                util::buffer_sprite_draw(&buffer, sprites::ALIEN_DEATH_SPRITE, alien.x, alien.y, util::rgb_to_uint32(128, 0, 0));
            } else {
                const data::SpriteAnimation& animation = sprites::ALIEN_ANIMATIONS[alien.type - 1];
                size_t current_frame = animation.time / animation.frame_duration;
                const data::Sprite& sprite = *animation.frames[current_frame];
                util::buffer_sprite_draw(&buffer, sprite, alien.x, alien.y, util::rgb_to_uint32(128, 0, 0));
            }
        }

        // Draw bullets
        for (size_t bi = 0; bi < game.num_bullets; ++bi) {
            const data::Bullet& bullet = game.bullets[bi];
            const data::Sprite& sprite = sprites::BULLET_SPRITE;
            util::buffer_sprite_draw(&buffer, sprite, bullet.x, bullet.y, util::rgb_to_uint32(128, 0, 0));
        }

        // Draw player
        util::buffer_sprite_draw(&buffer, sprites::PLAYER_SPRITE, game.player.x, game.player.y, util::rgb_to_uint32(128, 0, 0));

        // Update animations
        for (size_t i = 0; i < 3; ++i) {
            ++sprites::ALIEN_ANIMATIONS[i].time;
            if (sprites::ALIEN_ANIMATIONS[i].time == sprites::ALIEN_ANIMATIONS[i].num_frames * sprites::ALIEN_ANIMATIONS[i].frame_duration) {
                sprites::ALIEN_ANIMATIONS[i].time = 0;
            }
        }

        glTexSubImage2D(
            GL_TEXTURE_2D, 0, 0, 0,
            buffer.width, buffer.height,
            GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
            buffer.data
        );

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);

        // Update death_counters
        for (size_t ai = 0; ai < game.num_aliens; ++ai) {
            const data::Alien& alien = game.aliens[ai];
            if (alien.type == data::ALIEN_DEAD && death_counters[ai]) {
                --death_counters[ai];
            }
        }

        // Set direction of bullets
        for (size_t bi = 0; bi < game.num_bullets;) {
            game.bullets[bi].y += game.bullets[bi].dir;
            if (game.bullets[bi].y >= game.height || game.bullets[bi].y < sprites::BULLET_SPRITE.height) {
                game.bullets[bi] = game.bullets[game.num_bullets - 1];
                --game.num_bullets;
                continue;
            }
            // Check for alien collision 
            for (size_t ai = 0; ai < game.num_aliens; ++ai) {
                const data::Alien& alien = game.aliens[ai];
                if (alien.type == data::ALIEN_DEAD) {
                    continue;
                }
                const data::SpriteAnimation& animation = sprites::ALIEN_ANIMATIONS[alien.type - 1];
                size_t current_frame = animation.time / animation.frame_duration;
                const data::Sprite& alien_sprite = *animation.frames[current_frame];
                bool overlap = util::sprite_overlap_check(
                    sprites::BULLET_SPRITE, game.bullets[bi].x, game.bullets[bi].y,
                    alien_sprite, alien.x, alien.y
                );
                if (overlap) {
                    score += 10 * (4 - game.aliens[ai].type);
                    game.aliens[ai].type = data::ALIEN_DEAD;
                    game.aliens[ai].x -= (sprites::ALIEN_DEATH_SPRITE.width - alien_sprite.width) / 2;
                    game.bullets[bi] = game.bullets[game.num_bullets - 1];
                    --game.num_bullets;
                    continue;
                }
            }

            ++bi;
        }

        player_move_dir = 2 * move_dir;

        if (player_move_dir != 0) {
            if (game.player.x + sprites::PLAYER_SPRITE.width + player_move_dir >= game.width) {
                game.player.x = game.width - sprites::PLAYER_SPRITE.width;
            } else if ((int)game.player.x + player_move_dir <= 0) {
                game.player.x = 0;
            } else {
                game.player.x += player_move_dir;
            }
        }

        if (fire_pressed && game.num_bullets < GAME_MAX_BULLETS) {
            game.bullets[game.num_bullets].x = game.player.x + sprites::PLAYER_SPRITE.width / 2;
            game.bullets[game.num_bullets].y = game.player.y + sprites::PLAYER_SPRITE.height;
            game.bullets[game.num_bullets].dir = 2;
            ++game.num_bullets;
        }
        fire_pressed = false;

        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();

    glDeleteVertexArrays(1, &fullscreen_triangle_vao);
    sprites::cleanup_aliens();
    delete[] buffer.data;
    delete[] game.aliens;
    delete[] death_counters;

    return 0;
}
