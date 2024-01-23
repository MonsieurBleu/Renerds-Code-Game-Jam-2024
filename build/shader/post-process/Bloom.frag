#version 460

layout(location = 0) uniform ivec2 iResolution;
layout(location = 1) uniform float iTime;
layout(location = 2) uniform mat4 MVP;
layout(location = 3) uniform int pass;

layout(binding = 4) uniform sampler2D bSource;

in vec2 uvScreen;

out vec3 fragColor;

#include functions/HSV.glsl

// How far from the center to take samples from the fragment you are currently on
const float radius = 70.0;
// Keep it between 1.0f and 2.0f (the higher this is the further the blur reaches)
float spreadBlur = 2.0;
float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
bool horizontal = true;

vec3 blur(sampler2D screenTexture, vec2 texCoords) {             
    // Calculate the weights using the Gaussian equation
    float x = 0.0;
    for(int i = 0; i < radius; i++) {
        // Decides the distance between each sample on the Gaussian function
        if(spreadBlur <= 2.0)
            x += 3.0 / radius;
        else
            x += 6.0 / radius;

        weights[i] = exp(-0.5 * pow(x / spreadBlur, 2.0)) / (spreadBlur * sqrt(2.0 * 3.14159265));
    }

    vec2 tex_offset = textureSize(screenTexture, 0);
    tex_offset = 1.0 / tex_offset;
    vec3 result = texture(screenTexture, texCoords).rgb * weights[0];

    // Calculate horizontal blur
    // if(horizontal)
    if(pass == 1) {
        for(int i = 1; i < radius; i += 5) {
            // Take into account pixels to the right
            result += texture(screenTexture, texCoords + vec2(tex_offset.x * float(i), 0.0)).rgb * weights[i];
            // Take into account pixels on the left
            result += texture(screenTexture, texCoords - vec2(tex_offset.x * float(i), 0.0)).rgb * weights[i];
        }
    }
    // Calculate vertical blur
    else {
        for(int i = 1; i < radius; i += 5) {
            // Take into account pixels above
            result += texture(screenTexture, texCoords + vec2(0.0, tex_offset.y * float(i))).rgb * weights[i];
            // Take into account pixels below
            result += texture(screenTexture, texCoords - vec2(0.0, tex_offset.y * float(i))).rgb * weights[i];
        }
    }

    return result;
}

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) *
        43758.5453123);
}

void main() {
    // fragColor = texture(bSource, uvScreen).rgb;

    // fragColor = blur2(bSource);

    float b = 0.4;

    // if(pass == 2) b = 0.25;

    fragColor = b * blur(bSource, uvScreen).rgb;

    //if(pass == 1) {
        // fragColor *= 1.0 - 1.5*rgb2v(texture(bSource, uvScreen).rgb);
    //}

    // vec2 direction = vec2(10.0);

    // if(pass == 1) direction *= vec2(0.2, -0.75);
    // if(pass == 2) direction *= vec2(-0.96, 0.38);

    // fragColor = 2.0 * blur13(bSource, uvScreen, vec2(iResolution), direction).rgb;

    //fragColor *= 1E2;
}