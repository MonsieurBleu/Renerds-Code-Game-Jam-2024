#include <EffectHandler.hpp>
#include <FastUI.hpp>
#include <GameGlobals.hpp>
#include <Player.hpp>

EffectHandler::EffectHandler()
{
    finalComposingUniforms.add(ShaderUniform(&pixelEffectSize, 16));
    finalComposingUniforms.add(ShaderUniform(&mistEffectItensity, 17));
    finalComposingUniforms.add(ShaderUniform(&mistEffectColor1, 18));
    finalComposingUniforms.add(ShaderUniform(&mistEffectColor2, 19));
    finalComposingUniforms.add(ShaderUniform(&Player::deathAnimationProgress, 20));
    finalComposingUniforms.add(ShaderUniform(&Player::reviveAnimationProgress, 21));
    // skyboxUniforms.add(ShaderUniform(&sunColor, 20));
    // skyboxUniforms.add(ShaderUniform(&z1lerp, 21));
}

void EffectHandler::setDefaultMist()
{
    mistEffectItensity = 0.0075;
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
    float zone1dist = distance(GameGlobals::playerPosition, GameGlobals::Zone1Center);

    float objectif2dist = distance(GameGlobals::playerPosition, GameGlobals::Zone2Objectif);
    float objectif1dist = distance(GameGlobals::playerPosition, GameGlobals::Zone1Objectif);

    /* Update Mist for Zone 2 */
    const float z2MaxDist = GameGlobals::zone2radius + z2Transition;
    float z2lerp = (max(zone2dist - GameGlobals::zone2radius, 0.f)) / (z2MaxDist - GameGlobals::zone2radius);
    z2lerp = clamp(1.f - z2lerp, 0.f, 1.f);
    mistEffectItensity = mix(mistEffectItensity, 0.05f, z2lerp);
    mistEffectColor1 = mix(mistEffectColor1, vec3(0.8), z2lerp);
    mistEffectColor2 = mix(mistEffectColor2, vec3(0.8), z2lerp);
    

    /* Update Pixel effect for Zone 2 */
    const float o2MaxDist = o2MinDist + o2Transition;
    float o2lerp = (max(objectif2dist - o2MinDist, 0.f)) / (o2MaxDist - o2MinDist);
    o2lerp = clamp(1.f - o2lerp, 0.f, 1.f);
    pixelEffectSize = mix(pixelEffectSize, 0.02f, o2lerp);

    /* Update Zone 1 */
    const float z1MaxDist = GameGlobals::zone1radius + z1Transition;
    z1lerp = (max(zone1dist - GameGlobals::zone1radius, 0.f)) / (z1MaxDist - GameGlobals::zone1radius);
    z1lerp = clamp(1.f - z1lerp, 0.f, 1.f);

    /* Update Objectif 1*/
    const float o1MaxDist = o1MinDist + o1Transition;
    o1lerp = (max(objectif1dist - o1MinDist, 0.f)) / (o1MaxDist - o1MinDist);
    o1lerp = clamp(1.f - o1lerp, 0.f, 1.f);


    mistEffectItensity = mix(mistEffectItensity, 0.0175f, z1lerp);
    mistEffectColor1 = mix(mistEffectColor1, vec3(0.75, 0.8, 0.9)*vec3(0.5), z1lerp);
    mistEffectColor2 = mix(mistEffectColor2, vec3(0.75, 0.8, 0.9)*vec3(0.5), z1lerp);

    mistEffectColor1 = mix(mistEffectColor1, vec3(0.9, 0.3, 0.3)*vec3(0.5), o1lerp);
    mistEffectColor2 = mix(mistEffectColor2, vec3(0.9, 0.3, 0.3)*vec3(0.5), o1lerp);


    sunColor = mix(
            vec3(143, 107, 71) / vec3(255), 
            vec3(0x5e, 0x7a, 0x9e) / vec3(255), 
            z1lerp);

    sunColor = mix(
            sunColor, 
            vec3(0xFF, 0x0e, 0x08) / vec3(255), 
            o1lerp);

    float sunIntensity = mix(5.0f, 1.f, z1lerp);
    sunIntensity = mix(sunIntensity, 4.f, o1lerp);

    GameGlobals::sun->
        setColor(sunColor)
        .setIntensity(sunIntensity);
}