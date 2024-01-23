#pragma once

#include <Mesh.hpp>

class FastUI_valueMenu;

class GameGlobals
{
    public :
        static MeshMaterial PBR;
        static MeshMaterial PBRstencil;
        static MeshMaterial PBRinstanced;

        static vec3 Zone1Center;
        static vec3 Zone2Center;

        static vec3 Zone1Objectif;
        static vec3 Zone2Objectif;

        static vec3 playerPosition;

        static void setMenu(FastUI_valueMenu &menu);
};