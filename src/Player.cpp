#include "Player.hpp"
#include "../Engine/include/FastUI.hpp"

float Player::cursorXOld = 0.0f;
float Player::cursorYOld = 0.0f;
float Player::cursorXNew = 0.0f;
float Player::cursorYNew = 0.0f;

bool Player::W = false;
bool Player::A = false;
bool Player::S = false;
bool Player::D = false;

bool Player::grounded = false;
bool Player::lockJump = false;

bool Player::running = false;

float Player::startFOV = 1.0f;

float Player::stress = 0.0f;
float Player::stressFactor = 1.0f;
float Player::stressSmoothing = 0.7f;

float Player::stamina = 100.0f;

std::vector<RigidBodyRef>
    Player::thingsYouCanStandOn;

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
    mouseLook();

    float forward = 0.0f;
    float side = 0.0f;

    if (W)
        forward += forwardSpeed * (running ? 2.0f : 1.0f);
    if (S)
        forward -= backSpeed * (running ? 2.0f : 1.0f);
    if (A)
        side -= sideSpeed * (running ? 2.0f : 1.0f);
    if (D)
        side += sideSpeed * (running ? 2.0f : 1.0f);

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
    float bob = sin(globals.simulationTime.getElapsedTime() * 10.0f * (running ? 2.0 : 1.0)) * 0.1f;
    float speed = length(vec2(body->getVelocity().x, body->getVelocity().z));
    if (speed > 0)
        pos.y += bob;

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

    // std::cout << "stamina: " << stamina << "\n";
}

void Player::setMenu(FastUI_valueMenu &menu)
{
    menu.push_back(
        {FastUI_menuTitle(menu.ui, U"Player"),
         FastUI_valueTab(menu.ui, {
                                      FastUI_value((const float *)(&stamina), U"Stamina\t"),
                                      FastUI_value(&stress, U"Stress\t"),
                                      FastUI_value(&stressFactor, U"Stress Factor\t"),
                                      FastUI_value(&stressSmoothing, U"Stress Smoothing\t"),

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

bool Player::isInShadow(SceneDirectionalLight sun)
{
    sun->bindShadowMap();
    float d;
    glReadPixels(
        0, 
        0, 
        1,
        1,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        &d
    );

    std::cout << d << "\n";
}