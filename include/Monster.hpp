#pragma once

#include "../Engine/include/PhysicsEngine.hpp"
#include "../Engine/include/GameObject.hpp"
#include "../Engine/include/Camera.hpp"
#include "../Engine/include/Inputs.hpp"

#include <iostream>
#include <vector>

#include <GameGlobals.hpp>

using namespace glm;

class FastUI_valueMenu;

class Monster
{
private:
    ModelRef model;

    bool isPlayerInZone();

    static bool enabled;

public:
    Monster(ModelRef monster);
    Monster();
    ~Monster();

    void update(float deltaTime);

    void setMenu(FastUI_valueMenu &menu);

    static float maxSpeed;
    static float speed;
    static float distance;

    static float lastScreamTime;
    static constexpr float screamP = 0.001f;
    static constexpr float screamCooldown = 15.0f;
    static constexpr float screamMinDist = 20.0f;
};