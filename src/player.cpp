#include "player.h"

#include <algorithm>

void Sprite::Draw() const{
    DrawTexturePro(texture, {0, 0, 8 * (float)direction, 8}, dest_rect, {0, 0}, 0, WHITE);
};

//PLAYER

    void Player::Move(float horizontal, bool space_pressed){
        velocity.x = 0;
        velocity.x += std::clamp(horizontal * speed_factor, -200.f, 200.f);
        if (space_pressed){
            velocity.y = -200;
        }

    }

    void Player::FreeMove(float horizontal, float vertical){
        velocity.x = 0;
        velocity.y = 0;
        velocity.x += std::clamp(horizontal * speed_factor, -400.f, 400.f);
        velocity.y += -1 * std::clamp(vertical * speed_factor, -400.f, 400.f);

    }

    void Player::ApplyVelocity(float delta_time){
        sprite.dest_rect.x += velocity.x * delta_time;
        sprite.dest_rect.y += velocity.y * delta_time;
    }

    void Player::ApplyGravity(float gravity, float delta_time){
        velocity.y += gravity * delta_time;
        if (velocity.y > 600){velocity.y = 600;}
    }


    Vector2 Player::GetCenter() const {
        return {sprite.dest_rect.x + size.x / 2, sprite.dest_rect.y + size.y / 2};
    }

    Player Player::New(
        Texture2D texture,
        Vector2 size,
        float move_speed
    )
    {
        Sprite sprite{texture, {0, 0, size.x, size.y}};
        return {
            .sprite = sprite,
            .size = size,
            .speed_factor = move_speed};
    }

    void Player::Update(
        GameMode game_mode,
        const Input& input,
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

            Move(horizontal, input.pressed.space);
            ApplyGravity(gravity, delta_time);
            break;

            case EDITOR:
            FreeMove(horizontal, vertical);
            break;
        }

        ApplyVelocity(delta_time);

    }
