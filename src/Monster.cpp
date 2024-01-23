#include "Monster.hpp"

#include <../Engine/include/FastUI.hpp>

float Monster::speed = 0.0f;

Monster::Monster(GameObject monster)
{
    this->monster = monster;
}

Monster::~Monster()
{
}

void Monster::update(float deltaTime)
{
    if (isPlayerInZone() && enabled)
    {
        vec3 playerPosition = GameGlobals::playerPosition;
        vec3 monsterPosition = monster.getPosition();

        vec3 direction = playerPosition - monsterPosition;
        direction.y = 0.0f;

        float distance = length(direction);

        if (distance > 0.0f)
        {
            direction /= distance;

            // make speed decrease proportionally with distance
            // max speed is when the monster is at 2* the radius of the zone away
            float actualSpeed = speed * (1.0f - distance / (2.0f * GameGlobals::zone2radius));

            float speed = actualSpeed * deltaTime;

            if (distance < speed)
            {
                speed = distance;
            }

            monster.translate(direction * speed);
        }
    }

    if (!enabled && isPlayerInZone())
    {
        enabled = true;
        // spawn the monster at the center of the zone
        monster.setPosition(GameGlobals::Zone2Center);
        monster.getGroup()->state.hide = ModelStateHideStatus::SHOW;
    }

    if (enabled && !isPlayerInZone())
    {
        enabled = false;
        monster.getGroup()->state.hide = ModelStateHideStatus::HIDE;
    }

    GameGlobals::Zone2Objectif = monster.getPosition();
}

bool Monster::isPlayerInZone()
{
    vec2 playerPosition = vec2(GameGlobals::playerPosition.x, GameGlobals::playerPosition.z);

    vec2 direction = playerPosition - vec2(GameGlobals::Zone2Center.x, GameGlobals::Zone2Center.z);
    direction.y = 0.0f;

    float distance = length(direction);

    return distance < GameGlobals::zone2radius;
}