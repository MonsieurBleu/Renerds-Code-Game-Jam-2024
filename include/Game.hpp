#pragma once
#include <../Engine/include/App.hpp>
// #include <demos/FPS/FPSController.hpp>
#include <Player.hpp>
#include <Fonts.hpp>
#include <FastUI.hpp>

#include <GameGlobals.hpp>
#include <HandItem.hpp>
#include <EffectHandler.hpp>

class Game final : public App
{
private:
    /* 3D Materials */
    MeshMaterial skyboxMaterial;
    MeshMaterial depthOnlyMaterial;
    MeshMaterial depthOnlyStencilMaterial;
    MeshMaterial depthOnlyInstancedMaterial;

    /* Fast-UI */
    FontRef FUIfont;
    MeshMaterial defaultFontMaterial;
    MeshMaterial defaultSUIMaterial;
    SimpleUiTileBatchRef fuiBatch;

    /* Physics */
    std::shared_ptr<Player> player;
    PhysicsEngine physicsEngine;
    LimitTimer physicsTicks;
    void physicsLoop();

    /* Hand Item */
    HandItemHandlerRef handItems;

    EffectHandler effects;

public:
    Game(GLFWwindow *window);
    void init(int paramSample);
    bool userInput(GLFWKeyInfo input);
    void mainloop();
};