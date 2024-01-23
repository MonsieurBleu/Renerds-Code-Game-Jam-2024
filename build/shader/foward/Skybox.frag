#version 460

#define USING_VERTEX_TEXTURE_UV
#define NO_REFLECT_TEXTURE

#include uniform/Base3D.glsl
#include uniform/Model3D.glsl
#include uniform/Ligths.glsl

layout (binding = 0) uniform sampler2D bTexture;

#include globals/Fragment3DInputs.glsl
#include globals/Fragment3DOutputs.glsl

#include functions/standardMaterial.glsl
#include functions/Skybox.glsl

in vec3 viewPos;

float gamma = 1.8;
float exposure = 1.0;

void main()
{
    color = getSkyColor(uv);
    fragColor.rgb = color;

    float v = rgb2v(fragColor.rgb);
    fragEmmisive = fragColor.rgb*fragColor.rgb*pow(v, 10.0)*2.0;

    fragNormal = vec3(1);
}
