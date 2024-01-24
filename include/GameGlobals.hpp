#pragma once

#include <Mesh.hpp>
#include <random>
#include <Light.hpp>
#include <Monster.hpp>

class Player;

class FastUI_valueMenu;
class Monster;

class GameGlobals
{
public:
    static MeshMaterial PBR;
    static MeshMaterial PBRground;
    static MeshMaterial PBRstencil;
    static MeshMaterial PBRinstanced;

    static vec3 Zone1Center;
    static vec3 Zone2Center;

    static vec3 Zone1Objectif;
    static vec3 Zone2Objectif;

    static float zone1radius;
    static float zone2radius;

    static vec3 playerPosition;

    static Player* player; 

    static ModelRef house;
    static ModelRef foxAlive;
    static ModelRef foxDead;
    static ModelRef car;
    static ModelRef carWithBlood;
    static ModelRef foxTeddy;
    static ModelRef stumpTeddy;
    static ModelRef bookFox;
    static ModelRef stumpBook;
    static ModelRef signPost;
    static Scene* scene;

    static void setMenu(FastUI_valueMenu &menu);

    static std::random_device randomDevice;
    static std::mt19937_64 randomEngine;

    // random float between 0 and 1
    static std::uniform_real_distribution<float> randomFloatDist01;

    // random float between -1 and 1
    static std::uniform_real_distribution<float> randomFloatDist11;

    static SceneDirectionalLight sun;

    static Monster *monster;

    static float randomFloat01();

    static float randomFloat11();

    static bool isPlayerinZone1();
    static bool isPlayerinZone2();

    static bool E;
};