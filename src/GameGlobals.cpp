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

void GameGlobals::setMenu(FastUI_valueMenu &menu)
{
    menu.push_back(
        {FastUI_menuTitle(menu.ui, U"Game Globals"), FastUI_valueTab(menu.ui, {
            FastUI_value(U"Player Position "),
            FastUI_value((const float*)&playerPosition.x, U"\fx\t"),
            FastUI_value((const float*)&playerPosition.y, U"\fx\t"),
            FastUI_value((const float*)&playerPosition.z, U"\fx\t"),
            FastUI_value(U"Zone 1 Center "),
            FastUI_value(&Zone1Center.y, U"\fx\t"),
            FastUI_value(&Zone1Center.x, U"\fx\t"),
            FastUI_value(&Zone1Center.z, U"\fx\t"),
            FastUI_value(U"Zone 2 Center "),
            FastUI_value(&Zone2Center.y, U"\fx\t"),
            FastUI_value(&Zone2Center.x, U"\fx\t"),
            FastUI_value(&Zone2Center.z, U"\fx\t"),
            FastUI_value(U"Zone 1 objectif "),
            FastUI_value((const float*)&Zone1Objectif.y, U"\fx\t"),
            FastUI_value((const float*)&Zone1Objectif.x, U"\fx\t"),
            FastUI_value((const float*)&Zone1Objectif.z, U"\fx\t"),
            FastUI_value(U"Zone 2 objectif "),
            FastUI_value((const float*)&Zone2Objectif.y, U"\fx\t"),
            FastUI_value((const float*)&Zone2Objectif.x, U"\fx\t"),
            FastUI_value((const float*)&Zone2Objectif.z, U"\fx\t")
        })}
    );
}