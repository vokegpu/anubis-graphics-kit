#version 450

layout (location = 0) out vec4 vFragColor;

in vec3 vPos;
in vec2 vTexCoord;
in vec3 vNormal;

uniform vec3 uColor;
uniform vec3 uDirectionLightMoon;
uniform vec3 uSkyColor;

uniform float uStarsLuminance;
uniform bool uStarsRendering;

float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 albedoBlinnPhong() {
    //return mix(uColor, uSkyColor, dot(vNormal, uDirectionLightMoon));
    vec3 albedoColor = max(uColor * max(dot(vNormal, uDirectionLightMoon), 0.0f), uSkyColor);
    return albedoColor;
}

#define LIGHT_ASTROO_PROBABILITY 0.7
#define SUN_LIGHT_INTERPOLATION_FACTOR 2.0

void main() {
    if (!uStarsRendering) {
        vFragColor = vec4(albedoBlinnPhong(), 1.0f);
    } else {
        float f = length(noise2(vec2(length((rand(vec2(-22.0f, 5000.0f))) * 0.1 - (-vPos.zz)))));

        float interpFactor = -1.0f;
        if (uStarsLuminance < SUN_LIGHT_INTERPOLATION_FACTOR) {
            interpFactor = uStarsLuminance / SUN_LIGHT_INTERPOLATION_FACTOR;
        }

        if (interpFactor >= 0.0f && interpFactor < 0.2f && f < LIGHT_ASTROO_PROBABILITY) {
            discard;
        } else if (interpFactor > 0.2f && rand(vPos.xz) > 0.5) {
            discard;
        }

        if (f < LIGHT_ASTROO_PROBABILITY) {
            vFragColor = vec4(0.5f - (length(noise2(vec2(length((rand(vec2(-22.0f, 666.0f))) * 0.2 - (-vPos.zy)))))), f, 
                                      length(noise2(vec2(length((rand(vec2(-22.0f, 40.0f))) * 0.1 - (-vPos.zy))))), 1.0f);
        } else {
            vFragColor = vec4(1.0f);
        }

        if (interpFactor >= 0.0f) {
            vFragColor = mix(vFragColor, vec4(1.0f), 1.0f - interpFactor);
            vFragColor.a = max(interpFactor, 0.09f);
        }
    }
}