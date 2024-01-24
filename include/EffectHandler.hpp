#pragma once

#include <Uniforms.hpp>

class FastUI_valueMenu;

class EffectHandler
{
private:
    float pixelEffectSize;
    float mistEffectItensity;
    vec3 mistEffectColor1;
    vec3 mistEffectColor2;

    float z2Transition = 60.0/1.5;
    float z1Transition = 60.0/1.5;

    float o2MinDist = 5.0;
    float o2Transition = 30.0;

    float o1MinDist = 42.0/1.5;
    float o1Transition = 100.0/1.5;



public:

    float z1lerp;;
    float o1lerp;
    vec3 sunColor;

    EffectHandler();

    void setDefaultMist();
    void setDefaultPixel();

    void update();

    ShaderUniformGroup finalComposingUniforms;
    ShaderUniformGroup skyboxUniforms;

    void setMenu(FastUI_valueMenu &menu);
};