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
        {FastUI_menuTitle(menu.ui, U"Effects"), FastUI_valueTab(menu.ui, {FastUI_value(&(GameGlobals::zone2radius), U"Zone2 Size\t"),
                                                                          FastUI_value(&z2Transition, U"Zone2 Transition Distance\t"),
                                                                          FastUI_value(&o2MinDist, U"Objectif2 Distance\t"),
                                                                          FastUI_value(&o2Transition, U"Objectif2 Transition Distance\t")})});
}

void EffectHandler::update()
{
    setDefaultMist();
    setDefaultPixel();

    float zone2dist = distance(GameGlobals::playerPosition, GameGlobals::Zone2Center);

    /* Update Mist for Zone 2 */
    const float z2MaxDist = GameGlobals::zone2radius + z2Transition;
    float z2lerp = (max(zone2dist - GameGlobals::zone2radius, 0.f)) / (z2MaxDist - GameGlobals::zone2radius);
    z2lerp = clamp(1.f - z2lerp, 0.f, 1.f);
    mistEffectItensity = mix(mistEffectItensity, 0.05f, z2lerp);
    mistEffectColor1 = mix(mistEffectColor1, vec3(0.8), z2lerp);
    mistEffectColor2 = mix(mistEffectColor2, vec3(0.8), z2lerp);

    float objectif2dist = distance(GameGlobals::playerPosition, GameGlobals::Zone2Objectif);

    /* Update Pixel effect for Zone 2 */
    const float o2MaxDist = o2MinDist + o2Transition;
    float o2lerp = (max(objectif2dist - o2MinDist, 0.f)) / (o2MaxDist - o2MinDist);
    o2lerp = clamp(1.f - o2lerp, 0.f, 1.f);
    pixelEffectSize = mix(pixelEffectSize, 0.02f, o2lerp);

    // std::cout << zone2dist << " " << z2lerp << "\n";
}