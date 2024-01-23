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
    GameObject monster;

    bool isPlayerInZone();

public:
    Monster(GameObject monster);
    ~Monster();

    void update(float deltaTime);

    void setMenu(FastUI_valueMenu &menu);

    static constexpr float maxSpeed = 20.0f;
    static float speed;
};