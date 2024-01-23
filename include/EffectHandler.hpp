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

    float z2Transition = 50.0;
    float z1Transition = 30.0;

    float o2MinDist = 5.0;
    float o2Transition = 60.0;

    float o1MinDist = 5.0;
    float o1Transition = 35.0;



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