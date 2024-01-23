#include <GameGlobals.hpp>

MeshMaterial GameGlobals::PBR;
MeshMaterial GameGlobals::PBRinstanced;
MeshMaterial GameGlobals::PBRstencil;

vec3 GameGlobals::playerPosition;

vec3 GameGlobals::Zone1Center;
vec3 GameGlobals::Zone2Center;

vec3 GameGlobals::Zone1Objectif;
vec3 GameGlobals::Zone2Objectif;

std::random_device GameGlobals::randomDevice = std::random_device();
std::mt19937_64 GameGlobals::randomEngine = std::mt19937_64(randomDevice());
std::uniform_real_distribution<float> GameGlobals::randomFloatDist01 = std::uniform_real_distribution<float>(0.0f, 1.0f);
std::uniform_real_distribution<float> GameGlobals::randomFloatDist11 = std::uniform_real_distribution<float>(-1.0f, 1.0f);

float GameGlobals::randomFloat01()
{
    return randomFloatDist01(randomEngine);
}

float GameGlobals::randomFloat11()
{
    return randomFloatDist11(randomEngine);
}
