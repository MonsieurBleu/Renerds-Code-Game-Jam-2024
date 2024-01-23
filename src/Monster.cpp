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
    if (isPlayerInZone())
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
            const float radius = 50.0f; // temporary
            float actualSpeed = speed * (1.0f - distance / (2.0f * radius));

            float speed = actualSpeed * deltaTime;

            if (distance < speed)
            {
                speed = distance;
            }

            monster.translate(direction * speed);
        }
    }

    GameGlobals::Zone2Objectif = monster.getPosition();
}

bool Monster::isPlayerInZone()
{
    vec3 playerPosition = GameGlobals::playerPosition;
    vec3 monsterPosition = monster.getPosition();

    vec3 direction = playerPosition - monsterPosition;
    direction.y = 0.0f;

    float distance = length(direction);

    return distance < 50.0f;
}