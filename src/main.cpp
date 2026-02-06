#include <cstdio>
#include <cstdint>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "sprites/aliens.hpp"
#include "sprites/player.hpp"
#include "sprites/text.hpp"
#include "data/data.hpp"
#include "util/utility.hpp"

bool gameRunning = false;
int moveDir = 0;
bool firePressed = 0;
size_t score = 0;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (key) {
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_PRESS) {
                gameRunning = false;
            }
            break;
        case GLFW_KEY_RIGHT:
            if (action == GLFW_PRESS) {
                moveDir += 1;
            } else if (action == GLFW_RELEASE) {
                moveDir -= 1;
            }
            break;
        case GLFW_KEY_LEFT:
            if (action == GLFW_PRESS) {
                moveDir -= 1;
            } else if (action == GLFW_RELEASE) {
                moveDir += 1;
            }
            break;
        case GLFW_KEY_SPACE:
            if (action == GLFW_RELEASE) {
                firePressed = true;
            }
            break;
        default:
            break;
    }
}

int main()
{
    const size_t bufferWidth = 224;
    const size_t bufferHeight = 256;

    glfwSetErrorCallback(util::errorCallback);

    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(2 * bufferWidth, 2* bufferHeight, "Space Invaders", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, keyCallback);

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
    data::Buffer buffer(bufferWidth, bufferHeight);

    buffer.clear(0);

    // Create texture for presenting buffer to OpenGL
    GLuint bufferTexture;
    glGenTextures(1, &bufferTexture);
    glBindTexture(GL_TEXTURE_2D, bufferTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB8,
        buffer.getWidth(), buffer.getHeight(), 0,
        GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer.getData()
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create vao for generating fullscreen triangle
    GLuint fullscreenTriangleVao;
    glGenVertexArrays(1, &fullscreenTriangleVao);

    // Create shader for displaying buffer
    const char* vertexShader =
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
    const char* fragmentShader =
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

    GLuint shaderId = glCreateProgram();
    // Create vertex shader
    {
        GLuint shaderVp = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(shaderVp, 1, &vertexShader, 0);
        glCompileShader(shaderVp);
        util::validateShader(shaderVp, vertexShader);
        glAttachShader(shaderId, shaderVp);

        glDeleteShader(shaderVp);
    }

    // Create fragment shader
    {
        GLuint shaderFp = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(shaderFp, 1, &fragmentShader, 0);
        glCompileShader(shaderFp);
        util::validateShader(shaderFp, fragmentShader);
        glAttachShader(shaderId, shaderFp);

        glDeleteShader(shaderFp);
    }

    glLinkProgram(shaderId);

    if (!util::validateProgram(shaderId)) {
        fprintf(stderr, "Error while validating shader.\n");
        glfwTerminate();
        glDeleteVertexArrays(1, &fullscreenTriangleVao);
        return -1;
    }

    glUseProgram(shaderId);

    GLint location = glGetUniformLocation(shaderId, "buffer");
    glUniform1i(location, 0);

    // OpenGL setup
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(fullscreenTriangleVao);

    // Prepare game
    sprites::initializeAliens();

    data::Game game;
    game.width = bufferWidth;
    game.height = bufferHeight;
    game.numAliens = 55;
    game.numBullets = 0;
    game.aliens = new data::Alien[game.numAliens];

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

    uint8_t* deathCounters = new uint8_t[game.numAliens];
    for (size_t i = 0; i < game.numAliens; ++i) {
        deathCounters[i] = 10;
    }

    uint32_t clearColor = util::rgbToUint32(0, 128, 0);

    gameRunning = true;

    int playerMoveDir = 0;
    // Game loop
    while (!glfwWindowShouldClose(window) & gameRunning) {
        buffer.clear(clearColor);

        buffer.drawText(
            sprites::TEXT_SPRITESHEET, "SCORE",
            4, game.height - sprites::TEXT_SPRITESHEET.height - 7,
            util::rgbToUint32(128, 0, 0)
        );

        buffer.drawNumber(
            sprites::NUMBER_SPRITESHEET, score,
            4 + 2 * sprites::NUMBER_SPRITESHEET.width, game.height - 2 * sprites::NUMBER_SPRITESHEET.height - 12,
            util::rgbToUint32(128, 0, 0)
        );

        buffer.drawText(
            sprites::TEXT_SPRITESHEET, "CREDIT 00",
            164, 7,
            util::rgbToUint32(128, 0, 0)
        );

        // Line at bottom
        for (size_t i = 0; i < game.width; ++i) {
            buffer.getVector()[game.width * 16 + i] = util::rgbToUint32(128, 0, 0);
        }

        // Draw aliens
        for (size_t ai = 0; ai < game.numAliens; ++ai) {
            if (!deathCounters[ai]) {
                // Dead alien; don't draw
                continue;
            }

            const data::Alien& alien = game.aliens[ai];
            if (alien.type == data::ALIEN_DEAD) {
                buffer.drawSprite(sprites::ALIEN_DEATH_SPRITE, alien.x, alien.y, util::rgbToUint32(128, 0, 0));
            } else {
                const data::SpriteAnimation& animation = sprites::ALIEN_ANIMATIONS[alien.type - 1];
                size_t current_frame = animation.time / animation.frameDuration;
                const data::Sprite& sprite = *animation.frames[current_frame];
                buffer.drawSprite(sprite, alien.x, alien.y, util::rgbToUint32(128, 0, 0));
            }
        }

        // Draw bullets
        for (size_t bi = 0; bi < game.numBullets; ++bi) {
            const data::Bullet& bullet = game.bullets[bi];
            const data::Sprite& sprite = sprites::BULLET_SPRITE;
            buffer.drawSprite(sprite, bullet.x, bullet.y, util::rgbToUint32(128, 0, 0));
        }

        // Draw player
        buffer.drawSprite(sprites::PLAYER_SPRITE, game.player.x, game.player.y, util::rgbToUint32(128, 0, 0));

        // Update animations
        for (size_t i = 0; i < 3; ++i) {
            ++sprites::ALIEN_ANIMATIONS[i].time;
            if (sprites::ALIEN_ANIMATIONS[i].time == sprites::ALIEN_ANIMATIONS[i].numFrames * sprites::ALIEN_ANIMATIONS[i].frameDuration) {
                sprites::ALIEN_ANIMATIONS[i].time = 0;
            }
        }

        glTexSubImage2D(
            GL_TEXTURE_2D, 0, 0, 0,
            buffer.getWidth(), buffer.getHeight(),
            GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
            buffer.getData()
        );

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);

        // Update deathCounters
        for (size_t ai = 0; ai < game.numAliens; ++ai) {
            const data::Alien& alien = game.aliens[ai];
            if (alien.type == data::ALIEN_DEAD && deathCounters[ai]) {
                --deathCounters[ai];
            }
        }

        // Set direction of bullets
        for (size_t bi = 0; bi < game.numBullets;) {
            game.bullets[bi].y += game.bullets[bi].dir;
            if (game.bullets[bi].y >= game.height || game.bullets[bi].y < sprites::BULLET_SPRITE.height) {
                game.bullets[bi] = game.bullets[game.numBullets - 1];
                --game.numBullets;
                continue;
            }
            // Check for alien collision
            for (size_t ai = 0; ai < game.numAliens; ++ai) {
                const data::Alien& alien = game.aliens[ai];
                if (alien.type == data::ALIEN_DEAD) {
                    continue;
                }
                const data::SpriteAnimation& animation = sprites::ALIEN_ANIMATIONS[alien.type - 1];
                size_t current_frame = animation.time / animation.frameDuration;
                const data::Sprite& alien_sprite = *animation.frames[current_frame];
                bool overlap = util::spriteOverlapCheck(
                    sprites::BULLET_SPRITE, game.bullets[bi].x, game.bullets[bi].y,
                    alien_sprite, alien.x, alien.y
                );
                if (overlap) {
                    score += 10 * (4 - game.aliens[ai].type);
                    game.aliens[ai].type = data::ALIEN_DEAD;
                    game.aliens[ai].x -= (sprites::ALIEN_DEATH_SPRITE.width - alien_sprite.width) / 2;
                    game.bullets[bi] = game.bullets[game.numBullets - 1];
                    --game.numBullets;
                    continue;
                }
            }

            ++bi;
        }

        playerMoveDir = 2 * moveDir;

        if (playerMoveDir != 0) {
            if (game.player.x + sprites::PLAYER_SPRITE.width + playerMoveDir >= game.width) {
                game.player.x = game.width - sprites::PLAYER_SPRITE.width;
            } else if ((int)game.player.x + playerMoveDir <= 0) {
                game.player.x = 0;
            } else {
                game.player.x += playerMoveDir;
            }
        }

        if (firePressed && game.numBullets < GAME_MAX_BULLETS) {
            game.bullets[game.numBullets].x = game.player.x + sprites::PLAYER_SPRITE.width / 2;
            game.bullets[game.numBullets].y = game.player.y + sprites::PLAYER_SPRITE.height;
            game.bullets[game.numBullets].dir = 2;
            ++game.numBullets;
        }
        firePressed = false;

        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();

    glDeleteVertexArrays(1, &fullscreenTriangleVao);
    sprites::cleanupAliens();
    delete[] game.aliens;
    delete[] deathCounters;

    return 0;
}
