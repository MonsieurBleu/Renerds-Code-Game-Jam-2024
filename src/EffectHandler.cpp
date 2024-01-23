#include <EffectHandler.hpp>
#include <FastUI.hpp>
#include <GameGlobals.hpp>

EffectHandler::EffectHandler()
{
    finalComposingUniforms.add(ShaderUniform(&pixelEffectSize, 16));
    finalComposingUniforms.add(ShaderUniform(&mistEffectItensity, 17));
    finalComposingUniforms.add(ShaderUniform(&mistEffectColor1, 18));
    finalComposingUniforms.add(ShaderUniform(&mistEffectColor2, 19));
}

void EffectHandler::setDefaultMist()
{
    mistEffectItensity = 0.0075 * 0.0;
    mistEffectColor1 = vec3(0xf9, 0xb7, 0x2f) / 255.f;
    mistEffectColor2 = vec3(0xa8, 0x5d, 0x2a) / 255.f;
}

void EffectHandler::setDefaultPixel()
{
    pixelEffectSize = 0.f;
}

void EffectHandler::setMenu(FastUI_valueMenu &menu)
{
    menu.push_back(
        {FastUI_menuTitle(menu.ui, U"Effects"), FastUI_valueTab(menu.ui, {FastUI_value(&pixelEffectSize, U"Pixel Size\t"),
                                                                          FastUI_value(&mistEffectItensity, U"Mist Intensity\t"),
                                                                          FastUI_value(U"Color Mist 1"),
                                                                          FastUI_value((vec3 *)&mistEffectColor1, U"\fHEX\t", U"", FUI_vec3Color),
                                                                          FastUI_value((vec3 *)&mistEffectColor1, U"\fHue\t", U"°", FUI_vec3Hue),
                                                                          FastUI_value((vec3 *)&mistEffectColor1, U"\fSaturation\t", U"%", FUI_vec3Saturation),
                                                                          FastUI_value((vec3 *)&mistEffectColor1, U"\fLuminosity\t", U"%", FUI_vec3Value),
                                                                          FastUI_value(U"Color Mist 2"),
                                                                          FastUI_value((vec3 *)&mistEffectColor2, U"\fHEX\t", U"", FUI_vec3Color),
                                                                          FastUI_value((vec3 *)&mistEffectColor2, U"\fHue\t", U"°", FUI_vec3Hue),
                                                                          FastUI_value((vec3 *)&mistEffectColor2, U"\fSaturation\t", U"%", FUI_vec3Saturation),
                                                                          FastUI_value((vec3 *)&mistEffectColor2, U"\fLuminosity\t", U"%", FUI_vec3Value)})});
}

void EffectHandler::update()
{
    setDefaultMist();
    setDefaultPixel();

    float zone2dist = distance(GameGlobals::playerPosition, GameGlobals::Zone2Center);

    /* Update Mist for Zone 2 */
    const float z2MinDist = 5.0;
    const float z2MaxDist = z2MinDist + 50.0;
    float z2lerp = (max(zone2dist - z2MinDist, 0.f)) / (z2MaxDist - z2MinDist);
    z2lerp = clamp(1.f - z2lerp, 0.f, 1.f);
    mistEffectItensity = mix(mistEffectItensity, 0.05f, z2lerp);
    mistEffectColor1 = mix(mistEffectColor1, vec3(0.8), z2lerp);
    mistEffectColor2 = mix(mistEffectColor2, vec3(0.8), z2lerp);

    float objectif2dist = distance(GameGlobals::playerPosition, GameGlobals::Zone2Objectif);

    /* Update Pixel effect for Zone 2 */
    const float o2MinDist = 5.0;
    const float o2MaxDist = o2MinDist + 60.0;
    float o2lerp = (max(objectif2dist - o2MinDist, 0.f)) / (o2MaxDist - o2MinDist);
    o2lerp = clamp(1.f - o2lerp, 0.f, 1.f);
    pixelEffectSize = mix(pixelEffectSize, 0.02f, o2lerp);

    // std::cout << zone2dist << " " << z2lerp << "\n";
}