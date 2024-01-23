#ifndef FNCT_SKYBOX_GLSL
#define FNCT_SKYBOX_GLSL

layout (binding = 4) uniform sampler2D bSkyTexture;
layout (binding = 5) uniform sampler2D bSkyTexture2;
layout (binding = 6) uniform sampler2D bSkyTexture3;




// Reinhard tone mapping
vec3 toneMapReinhard(vec3 color, float exposure, float whitePoint) {
    color = color * exposure;
    return color / (color + vec3(1.0)) * whitePoint;
}

vec3 getSkyColor(vec2 uv
//, float noReflect = true
)
{
    #ifdef NO_REFLECT_TEXTURE
    vec3 c = texture(bSkyTexture, uv).rgb;
    #else
    vec3 c = texture(bSkyTexture, uv).rgb;
    #endif
    if(zone1Lerp > 0.0)
    {
        //vec2 uv2 = uv - mod(uv, vec2(0.00075));

        vec3 c2 = texture(bSkyTexture2, uv).rgb;

        //c2 = c2*step(vec3(0.05), c2);
        c2 = rgb2hsv(c2);
        c2.b *= 0.35;
        c2.g *= 1.25;
        c2 = hsv2rgb(c2);

        c = mix(c, c2, pow(zone1Lerp, 0.25));

        c *= mix(vec3(1.0), sunColor, zone1Lerp);
        //c += sunColor;
    }

    return c;
}

#endif
