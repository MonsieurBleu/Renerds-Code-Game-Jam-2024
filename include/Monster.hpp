#pragma once

#include "../Engine/include/PhysicsEngine.hpp"
#include "../Engine/include/GameObject.hpp"
#include "../Engine/include/Camera.hpp"
#include "../Engine/include/Inputs.hpp"
#include "Audio.hpp"

#include <iostream>
#include <vector>

#include <GameGlobals.hpp>

using namespace glm;

class FastUI_valueMenu;

class Monster
{
private:
    ModelRef model;

    static bool enabled;

public:
    AudioSource *droneSource = nullptr;
    AudioSource *screamSource = nullptr;
    Monster(ModelRef monster);
    Monster();
    ~Monster();

    void enable();
    void disable();

    // whether or not the monster can be enabled when entering it's zone
    static bool activated;

    void update(float deltaTime);

    void setMenu(FastUI_valueMenu &menu);

    void initSounds();

    static float maxSpeed;
    static float speed;
    static float distance;

    static float lastScreamTime;
    static constexpr float screamP = 0.001f;
    static constexpr float screamCooldown = 15.0f;
    static constexpr float screamMinDist = 20.0f;
};
