#include "Monster.hpp"
#include "Audio.hpp"
#include "GameGlobals.hpp"
#include "Player.hpp"

#include <../Engine/include/FastUI.hpp>

float Monster::speed = 0.0f;
float Monster::maxSpeed = 50.0f;
bool Monster::enabled = false;

float Monster::lastScreamTime = -Monster::screamCooldown;
float Monster::distance = 0.0f;

bool Monster::activated = true;

Monster::Monster(ModelRef monster)
{
    this->model = monster;
}

Monster::Monster()
{
}

Monster::~Monster()
{
}

void Monster::initSounds()
{
    AudioFile drone, scream;
    drone.loadOGG("../build/ressources/Audio/drone.ogg");
    this->droneSource = new AudioSource();
    this->droneSource
        ->setBuffer(drone.getHandle())
        .loop(true);
    scream.loadOGG("../build/ressources/Audio/scream.ogg");
    this->screamSource = new AudioSource();
    this->droneSource
      ->setBuffer(scream.getHandle());

    alSource3f(droneSource->getHandle(), AL_DIRECTION, 0.0, 0.0, 0.0);
    alSourcef(droneSource->getHandle(), AL_REFERENCE_DISTANCE, 2.0);
    alSource3f(screamSource->getHandle(), AL_DIRECTION, 0.0, 0.0, 0.0);
    alSourcef(screamSource->getHandle(), AL_REFERENCE_DISTANCE, 2.0);
}

void Monster::update(float deltaTime)
{
    bool inZone = GameGlobals::isPlayerinZone2();
    if (inZone && enabled)
    {
        vec3 playerPosition = GameGlobals::playerPosition;
        vec3 monsterPosition = model->state.position;

        vec3 direction = playerPosition - monsterPosition;
        direction.y = 0.0f;

        distance = length(direction) - (Player::hasTeddyBear ? 10.0f : 0.0f);

        if (distance > 0.0f)
        {
            direction /= distance;

            // make speed decrease proportionally with distance
            // max speed is when the monster is at 2* the radius of the zone away
            float actualSpeed;
            if (Player::hasTeddyBear)
                actualSpeed = maxSpeed * (distance / (2.0f * GameGlobals::zone2radius));
            else
            {
                actualSpeed = max(maxSpeed * (distance / (2.0f * GameGlobals::zone2radius)), 10.0f);
            }

            speed = actualSpeed * deltaTime;

            model->state.setPosition(monsterPosition + direction * speed);
            // audioSource->setPosition( - GameGlobals::playerPosition);

            droneSource->pause();
            vec3 pos = (monsterPosition + direction * speed);
            droneSource->setPosition(vec3(-pos.x, pos.y, -pos.z));
            droneSource->play();
        }

        if ((lastScreamTime + screamCooldown < globals.appTime.getElapsedTime()) && distance > screamMinDist - (Player::hasTeddyBear ? 10.0f : 0.0f))
        {
            if (GameGlobals::randomFloat01() <= screamP)
            {
                lastScreamTime = globals.appTime.getElapsedTime();
                std::cout << "scream" << std::endl;
                Player::invertedControls = true;
                Player::invertStart = globals.appTime.getElapsedTime();
                screamSource->play();
            }
        }
    }

    if (!enabled && inZone)
    {
        enable();
    }

    else if (enabled && !inZone)
    {
        disable();
    }

    GameGlobals::Zone2Objectif = model->state.position;
    // GameGlobals::Zone2Center = model->state.position;
}

void Monster::setMenu(FastUI_valueMenu &menu)
{
    menu.push_back(
        {FastUI_menuTitle(menu.ui, U"Monster"), FastUI_valueTab(menu.ui, {FastUI_value((const float *)(&speed), U"speed\t"),
                                                                          FastUI_value(&maxSpeed, U"maxSpeed\t"),
                                                                          FastUI_value(&enabled, U"enabled\t"),
                                                                          FastUI_value(&distance, U"distance\t")})});
}

void Monster::enable()
{
    if (!activated)
        return;
    enabled = true;
    // spawn the monster at the center of the zone
    model->state.setPosition(GameGlobals::Zone2Center);
    model->state.hide = ModelStateHideStatus::SHOW;

    droneSource->play();
}

void Monster::disable()
{
    enabled = false;
    model->state.hide = ModelStateHideStatus::HIDE;

    model->state.setPosition(vec3(1e16)); // far far away (shrek reference)
    GameGlobals::Zone2Objectif = model->state.position;
}
