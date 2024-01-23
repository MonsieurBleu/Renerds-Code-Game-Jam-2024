#pragma once

#include <Mesh.hpp>
#include <random>

class GameGlobals
{
public:
    static MeshMaterial PBR;
    static MeshMaterial PBRstencil;
    static MeshMaterial PBRinstanced;

    static vec3 Zone1Center;
    static vec3 Zone2Center;

    static vec3 Zone1Objectif;
    static vec3 Zone2Objectif;

    static vec3 playerPosition;

    static std::random_device randomDevice;
    static std::mt19937_64 randomEngine;

    // random float between 0 and 1
    static std::uniform_real_distribution<float> randomFloatDist01;

    // random float between -1 and 1
    static std::uniform_real_distribution<float> randomFloatDist11;

    static float randomFloat01();

    static float randomFloat11();
};