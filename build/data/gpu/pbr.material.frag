#version 450
#define PI 3.1415926535897932384626433832795

layout (location = 0) out vec4 vFragColor;

// PBR sampler(s)
layout (binding = 0) uniform sampler2D uSamplerAlbedo;
layout (binding = 1) uniform sampler2D uSamplerRoughness;
layout (binding = 2) uniform sampler2D uSamplerNormalMap;

struct Material {
    vec4 mSamplerFlags; // (albedo, rough, normal, useless)
    vec4 mSurface; // (metal, roughness factor, useless, useless)
    vec4 mColor; // (r, g, b, useless)
};

struct Light {
    vec4 uPos;
    vec4 uIntensity;
};

layout (std140, binding = 0) uniform uniformBufferMaterial {
    Material ubMaterial[512];
    Light ubLight[64];
};

in vec3 vPos;
in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vPosModel;
in vec3 vTangent;
in vec3 vBitangent;

uniform vec3  uCameraPos;
uniform float uAmbientColor;
uniform int   uLightAmount;
uniform float uGamma = 1.0f;
uniform int   uMaterialIndex;

Material pbrMaterial;
float    pbrRoughnessFactor;
vec3     pbrAlbedoColor;
vec3     pbrNormalColor;
mat3     tbnMatrix;

Light pbrLight;

uniform struct {
    vec2 uDistance;
    vec3 uColor;
} uFog;

void main() {
    float dist = length(vPos);
    float fogFactor = clamp((uFog.uDistance.y - dist) / (uFog.uDistance.y - uFog.uDistance.x), 0.0, 1.0);
    pbrMaterial = ubMaterial[uMaterialIndex];
    vec2 texCoord = vec2(vTexCoord.x, 1.0f - vTexCoord.y);

    // Not all materials have albedo, specular, and normal map textures.
    pbrAlbedoColor = pbrMaterial.mColor.rgb;
    pbrRoughnessFactor = pbrMaterial.mSurface.y;
    pbrNormalColor = vec3(0.0f);
    vec3 n = vec3(0.0f);

    if (pbrMaterial.mSamplerFlags.x == 1) {
        pbrAlbedoColor = texture(uSamplerAlbedo, texCoord).rgb;
    }

    if (pbrMaterial.mSamplerFlags.y == 1) {
        pbrRoughnessFactor = texture(uSamplerRoughness, texCoord).r;
    }

    if (pbrMaterial.mSamplerFlags.z == 1) {
        pbrNormalColor = texture(uSamplerNormalMap, texCoord).rgb * 2.0f - 1.0f;
        tbnMatrix = mat3(vTangent, vBitangent, normalize(vNormal));
        n = normalize(tbnMatrix * pbrNormalColor);
    } else {
        n = normalize(vNormal);
    }

    vec3 sum = pbrAlbedoColor * uAmbientColor;
    vec3 v = normalize(uCameraPos - vPosModel);

    if (!gl_FrontFacing) n = -n;
    for (int index = 0; index <= uLightAmount; index++) {
        pbrLight = ubLight[index];
        sum += pbrAlbedoColor;
    }

    sum = mix(uFog.uColor, sum, fogFactor);
    vFragColor = vec4(sum, 1.0f);
}