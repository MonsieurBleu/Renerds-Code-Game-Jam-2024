#include "Player.hpp"
#include <chrono>
#include "../Engine/include/FastUI.hpp"

float Player::cursorXOld = 0.0f;
float Player::cursorYOld = 0.0f;
float Player::cursorXNew = 0.0f;
float Player::cursorYNew = 0.0f;

bool Player::W = false;
bool Player::A = false;
bool Player::S = false;
bool Player::D = false;
bool Player::Q = false;
bool Player::E = false;

bool Player::grounded = false;
bool Player::lockJump = false;

bool Player::running = false;

float Player::startFOV = 1.0f;

float Player::stress = 0.0f;
float Player::stressFactor = 1.0f;
float Player::stressSmoothing = 0.7f;

float Player::stamina = 100.0f;

bool Player::invertedControls = false;
float Player::invertStart = 0.0f;
bool Player::hasTeddyBear = false;

float Player::stressDecreaseRate = 50.0f;
float Player::stressIncreaseRate = 35.0f;

float Player::deathAnimationProgress = 0.0f;

bool Player::dead = false;
float Player::deathTime = 0.0f;
float Player::respawnDelay = 3.0f;
bool Player::canDie = true;
vec3 Player::respawnPoint = vec3(0.0f);

vec3 Player::deathLookDir = vec3(0.0f);
vec3 Player::lifeLookDir = vec3(0.0f);

bool Player::reviveAnimation = false;
float Player::reviveAnimationStart = 0.0f;
float Player::reviveAnimationLength = 1.0f;
float Player::reviveAnimationProgress = 0.000f;

AudioSource *Player::heartbeat;
AudioSource *Player::step1;
AudioSource *Player::step2;
AudioSource *Player::step3;
AudioSource *Player::step4;

std::vector<RigidBodyRef>
    Player::thingsYouCanStandOn;

bool Player::locked = false;

Player::Player(GLFWwindow *window, RigidBodyRef body, Camera *camera, InputBuffer *inputs)
{
    this->body = body;
    this->inputs = inputs;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    double dcursorXOld, dcursorYOld;
    glfwGetCursorPos(window, &dcursorXOld, &dcursorYOld);
    cursorXOld = dcursorXOld;
    cursorYOld = dcursorYOld;

    double dcursorXNew, dcursorYNew;
    glfwGetCursorPos(window, &dcursorXNew, &dcursorYNew);
    cursorXNew = dcursorXNew;
    cursorYNew = dcursorYNew;

    // glfwSetCursorPosCallback(window, [](GLFWwindow *window, double x, double y)
    //                          {
    //     cursorXNew = x;
    //     cursorYNew = y; });

    // might break things later
    CameraState state = globals.currentCamera->getState();
    state.FOV = startFOV;
}

Player::~Player()
{
}

void Player::update(float deltaTime)
{
    if (locked)
        return;

    mouseLook();

    float forward = 0.0f;
    float side = 0.0f;

    vec3 monsterdiff = GameGlobals::Zone2Objectif - GameGlobals::playerPosition;
    monsterdiff.y = 0.0f;
    float monsterdist = length(monsterdiff);

    if (monsterdist < 1.0f)
    {
        if (!hasTeddyBear)
        {
            if (canDie)
                die();
        }

        // else win here or something
    }

    if (dead)
    {
        if (deathTime + respawnDelay < globals.appTime.getElapsedTime())
        {
            respawn();
        }
        else
        {
            deathAnimationProgress = (globals.appTime.getElapsedTime() - deathTime) / respawnDelay;

            vec3 finalLookDir = deathLookDir;
            finalLookDir.y = M_PI - (M_PI / 8.0f);
            finalLookDir = normalize(finalLookDir);

            vec3 camDir = lerp(deathLookDir, finalLookDir, min(deathAnimationProgress * 1.5f, 1.0f));
            globals.currentCamera->setDirection(camDir);
        }

        globals.currentCamera->setPosition(body->getPosition());
        return;
    }

    if (reviveAnimation)
    {
        if (reviveAnimationStart + reviveAnimationLength < globals.appTime.getElapsedTime())
        {
            reviveAnimation = false;
            reviveAnimationProgress = 0.0f;
        }
        else
        {
            reviveAnimationProgress = (globals.appTime.getElapsedTime() - reviveAnimationStart) / reviveAnimationLength;
            vec3 camDir = lerp(deathLookDir, lifeLookDir, reviveAnimationProgress);
            globals.currentCamera->setDirection(camDir);
        }

        globals.currentCamera->setPosition(body->getPosition());
        return;
    }

    if (W)
        forward += forwardSpeed * (running ? 2.0f : 1.0f) * (invertedControls ? -1 : 1);
    if (S)
        forward -= backSpeed * (running ? 2.0f : 1.0f) * (invertedControls ? -1 : 1);
    if (A)
        side -= sideSpeed * (running ? 2.0f : 1.0f) * (invertedControls ? -1 : 1);
    if (D)
        side += sideSpeed * (running ? 2.0f : 1.0f) * (invertedControls ? -1 : 1);

    if (flying && !W && !S && !A && !D)
    {
        body->setVelocity(vec3(0.0f));
    }

    if (!flying)
    {
        move(forward, side, deltaTime);

        // grounded test
        Ray ray{body->getPosition() + vec3(0, -1.95, 0), vec3(0.0f, -1.0f, 0.0f)};
        float t;
        RigidBodyRef bodyIntersect;
        grounded = raycast(ray, thingsYouCanStandOn, 0.2f, t, bodyIntersect);
        if (!grounded)
            lockJump = false;

        if (doJump && !lockJump)
        {
            jump(deltaTime);
        }

        vec3 pos = body->getPosition();

        // head bobbing
        static float lastBob = 0.0f;
        static bool rising = false;
        float bob = sin(globals.simulationTime.getElapsedTime() * 10.0f * (running ? 2.0 : 1.0)) * 0.1f;
        float speed = length(vec2(body->getVelocity().x, body->getVelocity().z));
        if (speed > 0.01)
        {
            pos.y += bob;

            if (rising && bob < lastBob)
            {
                rising = false;
                int r = rand() % 4;
                switch (r)
                {
                case 0:
                    step1->play();
                    break;
                case 1:
                    step2->play();
                    break;
                case 2:
                    step3->play();
                    break;
                case 3:
                    step4->play();
                    break;
                }
            }

            if (!rising && bob > lastBob)
            {
                rising = true;
            }
        }

        lastBob = bob;

        float diffBias = 0.0001;
        vec3 diff = globals.currentCamera->getPosition() - pos;
        if (dot(diff, diff) > diffBias)
        {
            globals.currentCamera->setPosition(pos);
            GameGlobals::playerPosition = pos * vec3(1, 0, 1);
        }

        if (running)
        {
            // drain stamina by staminaDrain per second
            stamina -= staminaDrain * deltaTime;
            if (stamina <= 0.0f)
            {
                stamina = 0.0f;
                running = false;
            }

            // adjust fov
            CameraState state = globals.currentCamera->getState();
            state.FOV = startFOV + 0.05f;
            globals.currentCamera->setState(state);
        }
        else
        {
            // regen stamina by staminaRegen per second
            stamina += staminaRegen * deltaTime;
            if (stamina > 100.0f)
            {
                stamina = 100.0f;
            }

            // adjust fov
            CameraState state = globals.currentCamera->getState();
            state.FOV = startFOV;
            globals.currentCamera->setState(state);
        }
    }
    else
    {
        if (W)
            flyForward((running ? 20.0f : 1.0f), deltaTime);
        if (S)
            flyForward(-(running ? 20.0f : 1.0f), deltaTime);
        if (A)
            flySide(-(running ? 20.0f : 1.0f), deltaTime);
        if (D)
            flySide((running ? 20.0f : 1.0f), deltaTime);
        if (Q)
            flyUp(-(running ? 2.0f : 1.0f), deltaTime);
        if (E)
            flyUp((running ? 2.0f : 1.0f), deltaTime);

        if (doJump)
            flyUp((running ? 2.0f : 1.0f), deltaTime);

        globals.currentCamera->setPosition(body->getPosition());
    }

    if (stress >= 20.0f)
    {
        // camera drift
        vec3 camDir = globals.currentCamera->getDirection();
        vec3 camRight = normalize(cross(camDir, vec3(0.0f, 1.0f, 0.0f)));
        vec3 camUp = normalize(cross(camRight, camDir));

        float randomFloatX = GameGlobals::randomFloat11() * ((stress - 20.0f) / 80.0f) * stressFactor;
        float randomFloatY = GameGlobals::randomFloat11() * ((stress - 20.0f) / 80.0f) * stressFactor;

        vec3 randomDir = camRight * randomFloatX + camUp * randomFloatY;

        vec3 newDir = camDir + randomDir * 0.01f;

        // smoothing
        newDir = normalize(newDir * (1.0f - stressSmoothing) + camDir * stressSmoothing);

        globals.currentCamera->setDirection(newDir);
    }

    static bool over40 = false;
    if (stress >= 40.0f && !over40) {
      heartbeat->loop(true);
      heartbeat->play();
      over40 = true;
    } else if (over40) {
      over40 = false;
      heartbeat->loop(false);
    }

    if (invertedControls && (invertStart + invertLength < globals.appTime.getElapsedTime()))
    {
        invertedControls = false;
    }

    // std::cout << "stamina: " << stamina << "\n";
}

void Player::setMenu(FastUI_valueMenu &menu)
{
    menu.push_back(
        {FastUI_menuTitle(menu.ui, U"Player"),
         FastUI_valueTab(menu.ui, {
                                      FastUI_value((const float *)(&stamina), U"Stamina\t"),
                                      FastUI_value(&stress, U"Stress\t"),
                                      FastUI_value(&stressDecreaseRate, U"Stress Decrease Rate\t"),
                                      FastUI_value(&stressIncreaseRate, U"Stress Increase Rate\t"),
                                      FastUI_value(&stressFactor, U"Stress Factor\t"),
                                      FastUI_value(&stressSmoothing, U"Stress Smoothing\t"),
                                      FastUI_value(&deathAnimationProgress, U"Death Animation Progress\t"),
                                      FastUI_value(&reviveAnimationProgress, U"Revive Animation Progress\t"),

                                  })});
}

void Player::doInputs(GLFWKeyInfo &input)
{
    if (input.key == GLFW_KEY_W)
    {
        if (input.action == GLFW_PRESS)
            W = true;
        else if (input.action == GLFW_RELEASE)
            W = false;
    }
    if (input.key == GLFW_KEY_A)
    {
        if (input.action == GLFW_PRESS)
            A = true;
        else if (input.action == GLFW_RELEASE)
            A = false;
    }
    if (input.key == GLFW_KEY_S)
    {
        if (input.action == GLFW_PRESS)
            S = true;
        else if (input.action == GLFW_RELEASE)
            S = false;
    }
    if (input.key == GLFW_KEY_D)
    {
        if (input.action == GLFW_PRESS)
            D = true;
        else if (input.action == GLFW_RELEASE)
            D = false;
    }
    if (input.key == GLFW_KEY_Q)
    {
        if (input.action == GLFW_PRESS)
            Q = true;
        else if (input.action == GLFW_RELEASE)
            Q = false;
    }
    if (input.key == GLFW_KEY_E)
    {
        if (input.action == GLFW_PRESS)
            E = true;
        else if (input.action == GLFW_RELEASE)
            E = false;
    }
    if (input.key == GLFW_KEY_SPACE)
    {
        if (input.action == GLFW_PRESS)
            doJump = true;
        else if (input.action == GLFW_RELEASE)
            doJump = false;
    }
    if (input.key == GLFW_KEY_LEFT_SHIFT)
    {
        if (input.action == GLFW_PRESS)
            running = true;
        else if (input.action == GLFW_RELEASE)
            running = false;
    }
    if (input.key == GLFW_KEY_F12)
    {
        if (input.action == GLFW_PRESS)
        {
            flying = !flying;
            body->setGravity(!body->getGravity());
        }
    }
}

vec3 nnormalize(vec3 v);

void Player::move(float fmove, float smove, float deltaTime)
{
    vec3 forward, right;

    const vec3 camDir = globals.currentCamera->getDirection();
    right = normalize(cross(camDir, vec3(0.0f, 1.0f, 0.0f)));
    forward = normalize(cross(vec3(0.0f, 1.0f, 0.0f), right));

    forward = nnormalize(forward);

    right = nnormalize(right);

    vec3 wishVel = forward * fmove + right * smove;

    vec3 wishDir;
    wishDir = nnormalize(wishVel);

    float wishSpeed = length(wishVel);

    if (!running)
    {

        if (wishSpeed > maxSpeed)
        {
            wishVel *= maxSpeed / wishSpeed;
            wishSpeed = maxSpeed;
        }
    }
    else
    {
        if (wishSpeed > maxSpeedRunning)
        {
            wishVel *= maxSpeedRunning / wishSpeed;
            wishSpeed = maxSpeedRunning;
        }
    }

    this->accelerate(wishDir, wishSpeed, acceleration * (running ? 2.0f : 1.0f), deltaTime);

    // clamp speed
    vec3 vel = body->getVelocity();
    float speed = length(vec2(vel.x, vel.z));

    if (!running)
    {

        if (speed > maxSpeed)
        {
            vel *= maxSpeed / speed;
            vel.y = body->getVelocity().y;
            body->setVelocity(vel);
        }
    }
    else
    {
        if (speed > maxSpeedRunning)
        {
            vel *= maxSpeedRunning / speed;
            vel.y = body->getVelocity().y;
            body->setVelocity(vel);
        }
    }

    if (GameGlobals::isPlayerinZone1())
    {
        if (!isInShadow())
        {
            stress += stressIncreaseRate * deltaTime;

            if (stress > 100.0f)
            {
                die();
            }
        }
        else
        {
            stress -= stressDecreaseRate * deltaTime;
            if (stress < 0.0f)
                stress = 0.0f;
        }
    }
    else
    {
        stress -= stressDecreaseRate * deltaTime;
        if (stress < 0.0f)
            stress = 0.0f;
    }
}

void Player::accelerate(vec3 wishDirection, float wishSpeed, float accel, float deltaTime)
{
    vec3 vel = body->getVelocity();
    float currentSpeed = dot(vel, wishDirection);
    float addSpeed = wishSpeed - currentSpeed;

    if (addSpeed <= 0)
    {
        return;
    }

    float accelSpeed = accel * deltaTime * wishSpeed;

    if (accelSpeed > addSpeed)
    {
        accelSpeed = addSpeed;
    }

    vel += accelSpeed * wishDirection;

    // std::cout << "accel: " << accel << "\n";
    // std::cout << "wishDirection: " << wishDirection.x << ", " << wishDirection.y << ", " << wishDirection.z << "\n";

    body->setVelocity(vel);
}

void Player::jump(float deltaTime)
{
    if (!grounded)
        return;
    vec3 vel = body->getVelocity();
    vel.y += jumpForce;

    body->setVelocity(vel);

    grounded = false;
    lockJump = true;
    doJump = false;
}

void Player::flyForward(float speed, float deltatime)
{
    vec3 pos = body->getPosition();

    body->setPosition(pos + globals.currentCamera->getDirection() * 10.0f * deltatime * speed);
}

void Player::flySide(float speed, float deltatime)
{
    vec3 pos = body->getPosition();

    vec3 camDir = globals.currentCamera->getDirection();
    vec3 camRight = normalize(cross(camDir, vec3(0.0f, 1.0f, 0.0f)));

    body->setPosition(pos + camRight * 10.0f * deltatime * speed);
}

void Player::flyUp(float speed, float deltatime)
{
    vec3 pos = body->getPosition();

    body->setPosition(pos + vec3(0.0f, 1.0f, 0.0f) * 10.0f * deltatime * speed);
}

void Player::mouseLook()
{
    // float xoffset = cursorXOld - cursorXNew;
    // float yoffset = cursorYOld - cursorYNew;

    // cursorXOld = cursorXNew;
    // cursorYOld = cursorYNew;

    // xoffset *= sensitivity;
    // yoffset *= sensitivity;

    // const vec3 camDir = globals.currentCamera->getDirection();
    // const vec3 camRight = normalize(cross(camDir, vec3(0.0f, 1.0f, 0.0f)));
    // // const vec3 camUp = normalize(cross(camRight, camDir));

    // quat pitchQuat = angleAxis(yoffset, camRight);
    // quat yawQuat = angleAxis(xoffset, vec3(0.0f, 1.0f, 0.0f));

    // quat newRotation = yawQuat * pitchQuat * globals.currentCamera->getDirection();

    // // print camera->getDirection()
    // // std::cout << "Camera direction: " << camera->getDirection().x << ", " << camera->getDirection().y << ", " << camera->getDirection().z << "\n";
    // // std::cout << "camera position: " << camera->getPosition().x << ", " << camera->getPosition().y << ", " << camera->getPosition().z << "\n";
    // vec3 rot = eulerAngles(newRotation);
    // globals.currentCamera->setDirection(rot);
}

void Player::die()
{
    lifeLookDir = globals.currentCamera->getDirection();
    canDie = false;
    // death animation
    ((SphereCollider *)body->getCollider())->setRadius(0.5f);
    body->setVelocity(vec3(0.0f));
    dead = true;
    deathTime = globals.appTime.getElapsedTime();
    GameGlobals::monster->disable();
    GameGlobals::monster->activated = false;
    deathLookDir = globals.currentCamera->getDirection();
}

void Player::respawn()
{
    // respawn animation
    ((SphereCollider *)body->getCollider())->setRadius(2.0f);
    body->setVelocity(vec3(0.0f));
    body->setPosition(respawnPoint + vec3(0.0f, 2.0f, 0.0f));
    GameGlobals::playerPosition = respawnPoint;
    dead = false;
    deathAnimationProgress = 0.0f;
    stress = 0;
    deathTime = globals.appTime.getElapsedTime();
    canDie = true;
    GameGlobals::monster->activated = true;

    reviveAnimation = true;
    reviveAnimationStart = globals.appTime.getElapsedTime();
    reviveAnimationProgress = 0.001f;

    deathLookDir = globals.currentCamera->getDirection();
}

bool Player::isInShadow()
{
    glBindFramebuffer(GL_FRAMEBUFFER, GameGlobals::sun->shadowMap.getHandle());
    glViewport(0, 0, GameGlobals::sun->cameraResolution.x, GameGlobals::sun->cameraResolution.y);

    vec4 mapPosition = GameGlobals::sun->getInfos()._rShadowMatrix * vec4(GameGlobals::playerPosition, 1.f);
    mapPosition /= mapPosition.w;
    mapPosition.x = mapPosition.x * 0.5 + 0.5;
    mapPosition.y = mapPosition.y * 0.5 + 0.5;

    float d;
    glReadPixels(
        mapPosition.x * GameGlobals::sun->cameraResolution.x,
        mapPosition.y * GameGlobals::sun->cameraResolution.y,
        1,
        1,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        &d);

    GameGlobals::sun->shadowMap.deactivate();

    return d - 0.001 > mapPosition.z;
}
