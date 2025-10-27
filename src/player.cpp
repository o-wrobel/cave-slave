#include "player.h"

#include <algorithm>
#include <cstdint>
#include <raylib.h>
#include <raymath.h>

#define MAX_EDITOR_SPEED 400.f
#define JUMP_POWER 140.f

//PLAYER
Player Player::New(
    Texture2D texture,
    Vector2 size,
    float max_horizontal_speed
)
{
    Sprite sprite{texture, {0, 0, size.x, size.y}};
    return {
        .sprite = sprite,
        .max_horizontal_speed = max_horizontal_speed};
}

void Player::ResolveCollision(Rectangle tile_rect){
    Rectangle overlap = GetCollisionRec(sprite.dest_rect, tile_rect);

       if (overlap.width < overlap.height) {
           if (sprite.dest_rect.x < tile_rect.x) {
               sprite.dest_rect.x -= overlap.width;
           } else {
               sprite.dest_rect.x += overlap.width;
           }
           velocity.x = 0;
       } else {
           // Vertical collision - push up or down
           if (sprite.dest_rect.y < tile_rect.y) {
               // Player is above, push up (hit ceiling)
               sprite.dest_rect.y -= overlap.height;
               if (velocity.y > 0) velocity.y = 0;
           } else {
               // Player is below, push down (hit ground)
               sprite.dest_rect.y += overlap.height;
               if (velocity.y < 0) velocity.y = 0;
           }
       }
}

void Player::CheckCollision(const Grid& grid, uint16_t tile_resolution){ // Check collision with grid
    Vector2u player_grid_position = {
        static_cast<uint16_t>(GetCenterPosition().x / tile_resolution),
        static_cast<uint16_t>(GetCenterPosition().y / tile_resolution)
    };
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            Vector2 tile_position = Vector2Add(
                player_grid_position.ToVector2(),
                {static_cast<float>(dx), static_cast<float>(dy)}
            );

            // Bounds check
            if (tile_position.x < 0 || tile_position.x >= grid.size_x ||
                tile_position.y < 0 || tile_position.y >= grid.size_y)
                continue;

            // Skip empty tiles
            if (grid.GetTile(tile_position.x, tile_position.y).type == 0) continue;

            // Create tile rectangle
            Rectangle tile_rect = {
                tile_position.x * tile_resolution,
                tile_position.y * tile_resolution,
                1.f * (tile_resolution),
                1.f * (tile_resolution)
            };

            // Check collision
            if (CheckCollisionRecs(sprite.dest_rect, tile_rect)) {
                ResolveCollision(tile_rect);
            }
        }
    }
}

void Player::SetVelocity(float horizontal_input, bool space_pressed){
    if (horizontal_input == 0) {
        velocity.x *= 0.1;
    } else {
        velocity.x = horizontal_input * max_horizontal_speed;
    }
    if (space_pressed){
        velocity.y = -1 * JUMP_POWER;
    }

}

void Player::ApplyVelocityFree(float horizontal, float vertical){
    velocity.x = 0;
    velocity.y = 0;
    velocity.x += horizontal * MAX_EDITOR_SPEED;
    velocity.y += -1 * vertical * MAX_EDITOR_SPEED;

}

void Player::ApplyVelocity(float delta_time){
    sprite.dest_rect.x += velocity.x * delta_time;
    sprite.dest_rect.y += velocity.y * delta_time;
}

void Player::ApplyGravity(float gravity, float delta_time){
    velocity.y += gravity * delta_time;
    if (velocity.y > 600){velocity.y = 600;}
}

Vector2 Player::GetCenterPosition() const {
    return {sprite.dest_rect.x + sprite.dest_rect.width / 2, sprite.dest_rect.y + sprite.dest_rect.height / 2};
}

void Player::Update(
    GameMode game_mode,
    const Input& input,
    const Grid& grid,
    float gravity,
    float delta_time)
{
    // Calculate movement based on input
    float horizontal = (input.held.right ? 1.0f : 0.0f) - (input.held.left ? 1.0f : 0.0f);
    float vertical = (input.held.up ? 1.0f : 0.0f) - (input.held.down ? 1.0f : 0.0f);

    switch (game_mode) {
        case PLAY:
            if (horizontal < 0){
                sprite.direction = LEFT;
            }
            if (horizontal > 0){
                sprite.direction = RIGHT;
            }

            SetVelocity(horizontal, input.pressed.space);
            ApplyGravity(gravity, delta_time);

            // CRITICAL: Apply axis separation - move X first, check collision
            CheckCollision(grid, 8);
            velocity.x = std::clamp(velocity.x, -1 * max_horizontal_speed, max_horizontal_speed);
            sprite.dest_rect.x += velocity.x * delta_time;

            // Then move Y, check collision
            CheckCollision(grid, 8);
            sprite.dest_rect.y += velocity.y * delta_time;

        break;

        case EDITOR:
            ApplyVelocityFree(horizontal, vertical);
            ApplyVelocity(delta_time);

        break;
    }
}
