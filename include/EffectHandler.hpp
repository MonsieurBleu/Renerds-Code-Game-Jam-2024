#pragma once

#include <Uniforms.hpp>

class FastUI_valueMenu;

class EffectHandler
{
    private :
        float pixelEffectSize;
        float mistEffectItensity;
        vec3  mistEffectColor1;
        vec3  mistEffectColor2;

    public :

        EffectHandler();

        void setDefaultMist();
        void setDefaultPixel();

        void update();
        
        ShaderUniformGroup finalComposingUniforms;

        void setMenu(FastUI_valueMenu &menu);
};