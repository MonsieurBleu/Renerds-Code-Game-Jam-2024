#version 460

#define USING_VERTEX_TEXTURE_UV
#define SKYBOX_REFLECTION
// #define CUBEMAP_SKYBOX

#include uniform/Base3D.glsl
#include uniform/Model3D.glsl
#include uniform/Ligths.glsl

layout (binding = 0) uniform sampler2D bColor;
layout (binding = 1) uniform sampler2D bMaterial;

layout (binding = 7) uniform sampler2D bForest;
layout (binding = 8) uniform sampler2D bColorPath;
layout (binding = 9) uniform sampler2D bMaterialPath;
layout (binding = 10) uniform sampler2D bColorRoad;
layout (binding = 11) uniform sampler2D bMaterialRoad;

#include globals/Fragment3DInputs.glsl
#include globals/Fragment3DOutputs.glsl

#include functions/standardMaterial.glsl
#include functions/Reflections.glsl
#include functions/NormalMap.glsl

void main() {
    vec2 uv2 = uv*2.0;
    vec4 CE = texture(bColor, uv2);
    vec4 NRM = texture(bMaterial, uv2);

    if(NRM.x <= 0.01 && NRM.y <= 0.01)
        discard;

    mEmmisive = 1.0 - CE.a;
    color = CE.rgb;


    vec2 uvForest = 0.5 + 0.5*position.xz/(512.0/1.5);
    vec4 map = texture(bForest, uvForest);

    vec3 colorForest = texture(bColorPath, uv2).rgb;
    vec4 normalForest = texture(bMaterialPath, uv2);

    float forest = smoothstep(0.0, 0.4, map.x);

    color = mix(colorForest, color, forest);
    NRM = mix(normalForest, NRM, forest);

    color = mix(color, texture(bColorRoad, uv2).rgb, distance(map.r, map.b));
    NRM = mix(NRM, texture(bMaterialRoad, uv2), distance(map.r, map.b));

    mMetallic = 1.0 - NRM.a;
    mMetallic = 0.0;
    mRoughness = NRM.b;
    mRoughness2 = mRoughness * mRoughness;

    normalComposed = perturbNormal(normal, viewVector, NRM.xy, uv2);
    viewDir = normalize(_cameraPosition - position);

    //color = map.xyz;


    normalComposed = gl_FrontFacing ? normalComposed : -normalComposed;

    Material material = getMultiLightPBR();
    vec3 rColor = getSkyboxReflection(viewDir, normalComposed);
    const float reflectFactor = getReflectionFactor(material.fresnel, mMetallic, mRoughness);
    fragColor.rgb = color * ambientLight + material.result + rColor * reflectFactor;

    fragColor.rgb = mix(fragColor.rgb, color, mEmmisive);
    fragEmmisive = getStandardEmmisive(fragColor.rgb);

    // fragNormal = normalize((vec4(normalComposed, 0.0) * _cameraInverseViewMatrix).rgb)*0.5 + 0.5;
    fragNormal = normalize((vec4(normalComposed, 0.0) * inverse(_cameraViewMatrix)).rgb) * 0.5 + 0.5;

    //fragColor.rgb = material.result;
}
