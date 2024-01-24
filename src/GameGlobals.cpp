#include <GameGlobals.hpp>
#include <FastUI.hpp>

MeshMaterial GameGlobals::PBR;
MeshMaterial GameGlobals::PBRinstanced;
MeshMaterial GameGlobals::PBRstencil;

vec3 GameGlobals::playerPosition;

vec3 GameGlobals::Zone1Center;
vec3 GameGlobals::Zone2Center;

vec3 GameGlobals::Zone1Objectif;
vec3 GameGlobals::Zone2Objectif;

float GameGlobals::zone2radius;
float GameGlobals::zone1radius;
SceneDirectionalLight GameGlobals::sun;

Monster *GameGlobals::monster;

Player* GameGlobals::player; 

ModelRef GameGlobals::foxAlive;
ModelRef GameGlobals::foxDead;
Scene*   GameGlobals::scene;

bool GameGlobals::E = false;

void GameGlobals::setMenu(FastUI_valueMenu &menu)
{
    menu.push_back(
        {FastUI_menuTitle(menu.ui, U"Game Globals"), FastUI_valueTab(menu.ui, {FastUI_value(U"Player Position "),
                                                                               FastUI_value((const float *)&playerPosition.x, U"\fx\t"),
                                                                               FastUI_value((const float *)&playerPosition.y, U"\fx\t"),
                                                                               FastUI_value((const float *)&playerPosition.z, U"\fx\t"),
                                                                               FastUI_value(U"Zone 1 Center "),
                                                                               FastUI_value(&Zone1Center.y, U"\fx\t"),
                                                                               FastUI_value(&Zone1Center.x, U"\fx\t"),
                                                                               FastUI_value(&Zone1Center.z, U"\fx\t"),
                                                                               FastUI_value(U"Zone 2 Center "),
                                                                               FastUI_value(&Zone2Center.y, U"\fx\t"),
                                                                               FastUI_value(&Zone2Center.x, U"\fx\t"),
                                                                               FastUI_value(&Zone2Center.z, U"\fx\t"),
                                                                               FastUI_value(U"Zone 1 objectif "),
                                                                               FastUI_value((const float *)&Zone1Objectif.y, U"\fx\t"),
                                                                               FastUI_value((const float *)&Zone1Objectif.x, U"\fx\t"),
                                                                               FastUI_value((const float *)&Zone1Objectif.z, U"\fx\t"),
                                                                               FastUI_value(U"Zone 2 objectif "),
                                                                               FastUI_value((const float *)&Zone2Objectif.y, U"\fx\t"),
                                                                               FastUI_value((const float *)&Zone2Objectif.x, U"\fx\t"),
                                                                               FastUI_value((const float *)&Zone2Objectif.z, U"\fx\t")})});
}
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

bool GameGlobals::isPlayerinZone2()
{
    vec2 playerPosition = vec2(GameGlobals::playerPosition.x, GameGlobals::playerPosition.z);

    vec2 direction = playerPosition - vec2(GameGlobals::Zone2Center.x, GameGlobals::Zone2Center.z);

    float distance = length(direction);

    return distance < GameGlobals::zone2radius;
}

bool GameGlobals::isPlayerinZone1()
{
    vec2 playerPosition = vec2(GameGlobals::playerPosition.x, GameGlobals::playerPosition.z);

    vec2 direction = playerPosition - vec2(GameGlobals::Zone1Center.x, GameGlobals::Zone1Center.z);

    float distance = length(direction);

    return distance < GameGlobals::zone1radius;
}