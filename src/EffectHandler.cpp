#include <EffectHandler.hpp>
#include <FastUI.hpp>

EffectHandler::EffectHandler()
{
    finalComposingUniforms.add(ShaderUniform(&pixelEffectSize, 16));
    finalComposingUniforms.add(ShaderUniform(&mistEffectItensity, 17));
    finalComposingUniforms.add(ShaderUniform(&mistEffectColor1, 18));
    finalComposingUniforms.add(ShaderUniform(&mistEffectColor2, 19));
}

void EffectHandler::setDefaultMist()
{
    mistEffectItensity = 0.0075;
    mistEffectColor1 = vec3(0xf9, 0xb7, 0x2f)/255.f;
    mistEffectColor2 = vec3(0xa8, 0x5d, 0x2a)/255.f;
}

void EffectHandler::setDefaultPixel()
{
    pixelEffectSize = 0.f;
}

void EffectHandler::setMenu(FastUI_valueMenu &menu)
{
    menu.push_back(
        {FastUI_menuTitle(menu.ui, U"Effects"), FastUI_valueTab(menu.ui, {
            FastUI_value(&pixelEffectSize, U"Pixel Size\t"),
            FastUI_value(&mistEffectItensity, U"Mist Intensity\t"),
            FastUI_value(U"Color Mist 1"),
            FastUI_value((vec3*)&mistEffectColor1, U"\fHEX\t", U"", FUI_vec3Color),
            FastUI_value((vec3*)&mistEffectColor1, U"\fHue\t", U"°", FUI_vec3Hue),
            FastUI_value((vec3*)&mistEffectColor1, U"\fSaturation\t", U"%", FUI_vec3Saturation),
            FastUI_value((vec3*)&mistEffectColor1, U"\fLuminosity\t", U"%", FUI_vec3Value),
            FastUI_value(U"Color Mist 2"),
            FastUI_value((vec3*)&mistEffectColor2, U"\fHEX\t", U"", FUI_vec3Color),
            FastUI_value((vec3*)&mistEffectColor2, U"\fHue\t", U"°", FUI_vec3Hue),
            FastUI_value((vec3*)&mistEffectColor2, U"\fSaturation\t", U"%", FUI_vec3Saturation),
            FastUI_value((vec3*)&mistEffectColor2, U"\fLuminosity\t", U"%", FUI_vec3Value)
        })}
    );
}

void EffectHandler::update()
{
    // vec3 playerPos = glob
    // float zone2dist = 
}