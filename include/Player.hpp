#pragma once

#include "../Engine/include/PhysicsEngine.hpp"
#include "../Engine/include/Camera.hpp"
#include "../Engine/include/Inputs.hpp"

#include <iostream>
#include <vector>

#include <GameGlobals.hpp>

using namespace glm;

class FastUI_valueMenu;

class Player
{
private:
    RigidBodyRef body;
    InputBuffer *inputs;

    bool doJump;

public:
    Player(GLFWwindow *window, RigidBodyRef body, Camera *camera, InputBuffer *inputs);
    ~Player();

    void update(float deltaTime);

    void doInputs(GLFWKeyInfo &input);

    void move(float forward, float side, float deltaTime);
    void accelerate(vec3 wishDirection, float wishSpeed, float accel, float deltaTime);

    void jump(float deltaTime);
    void flyForward(float speed, float deltaTime);
    void flySide(float speed, float deltaTime);
    void flyUp(float speed, float deltaTime);

    void mouseLook();

    bool isInShadow();
    void setMenu(FastUI_valueMenu &menu);

    static constexpr float maxSpeed = 12.0f;
    static constexpr float maxSpeedRunning = 15.0f;
    static constexpr float stopSpeed = 15.0f;
    static constexpr float acceleration = 3.0f;

    static float cursorXOld;
    static float cursorYOld;

    static float cursorXNew;
    static float cursorYNew;

    static bool W;
    static bool A;
    static bool S;
    static bool D;
    static bool Q;
    static bool E;

    static bool grounded;
    static bool lockJump;

    static bool running;
    static float stamina;
    static constexpr float staminaDrain = 50.0f;
    static constexpr float staminaRegen = 50.0f;

    static float stressIncreaseRate;
    static float stressDecreaseRate;

    // probably should have a better name
    static std::vector<RigidBodyRef> thingsYouCanStandOn;

    static constexpr float sensitivity = 0.005f;

    static constexpr float forwardSpeed = 15.0f; // 20
    static constexpr float sideSpeed = 15.0f;    // 35
    static constexpr float backSpeed = 10.0f;    // 20

    static constexpr float jumpForce = 0.0f;

    static float startFOV;

    static float stress;
    static float stressFactor;
    static float stressSmoothing;

    bool flying = false;

    static bool invertedControls;
    static float invertStart;
    static constexpr float invertLength = 1.0f;

    static bool hasTeddyBear;
};
