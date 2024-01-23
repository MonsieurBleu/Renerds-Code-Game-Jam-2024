#ifndef FNCT_SKYBOX_GLSL
#define FNCT_SKYBOX_GLSL


layout (binding = 4) uniform sampler2D bSkyTexture;
layout (binding = 5) uniform sampler2D bSkyTexture2;



// Reinhard tone mapping
vec3 toneMapReinhard(vec3 color, float exposure, float whitePoint) {
    color = color * exposure;
    return color / (color + vec3(1.0)) * whitePoint;
}

vec3 getSkyColor(vec2 uv)
{
    vec3 c = texture(bSkyTexture, uv).rgb;

    if(zone1Lerp > 0.0)
    {
        vec3 c2 = texture(bSkyTexture2, uv).rgb;
        c = mix(c, c2, pow(zone1Lerp, 0.25));

        c *= mix(vec3(1.0), sunColor, zone1Lerp);
        //c += sunColor;
    }

    return c;
}

#endif
