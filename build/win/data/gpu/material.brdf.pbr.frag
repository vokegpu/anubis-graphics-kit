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

// Material buffer
layout (std140, binding = 0) uniform uniformBufferMaterial {
    Material ubMaterial[512];
};

in vec3 vPos;
in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vPosModel;
in vec3 vTangent;
in vec3 vBitangent;

uniform vec3 uCameraPos;
uniform float uAmbientColor;
uniform int uLightAmount;
uniform float uGamma = 1.0f;
uniform int uMaterialIndex;

Material pbrMaterial;
float pbrRoughnessFactor;
vec3 pbrAlbedoColor;
vec3 pbrNormalColor;
mat3 tbnMatrix;

uniform struct {
    vec3 uIntensity;
    bool uDirectional;
    vec3 uVector;
} uLight[100];

uniform struct {
    vec2 uDistance;
    vec3 uColor;
} uFog;

// @TODO add metal factor & roughness based in texture (albedo, specular, normalmap)

vec3 fSchlick(float lDotH, vec3 f0) {
    f0 = pbrMaterial.mSurface.x == 1 ? pbrAlbedoColor : f0;
    return f0 + (1.0f - f0) * pow(1.0f - lDotH, 5.0f);
}

//float fSchlick(float dotProd, float f0, float f90) {
//    return f0 + (f90 - f0) * pow(1.0 - dotProd, 5.0);
//}

// float fdBurley(float nDotV, float nDotL, float lDotH) {
//     float f90 = 0.5f + 2.0f * pbrRoughnessFactor * lDotH * lDotH;
//     float lightScatter = fSchlick(nDotL, 1.0f, f90);
//     float viewScatter = fSchlick(nDotV, 1.0f, f90);
//     return lightScatter * viewScatter * (1.0f / PI);
// }

float smithGeometryShadow(float nDotV, float nDotL) {
    float a = pbrRoughnessFactor * pbrRoughnessFactor;
    float v = nDotL * sqrt(nDotV * nDotV * (1.0f - a) + a);
    float l = nDotV * sqrt(nDotL * nDotL * (1.0f - a) + a);
    return 0.5f / (v + l);
}

float ggxDistribution(float nDotH) {
    float a = pbrRoughnessFactor * pbrRoughnessFactor;
    float p = (nDotH * nDotH) * (a - 1.0f) + 1.0f;
    return a / (PI * p * p);
}

vec3 bidirecionalReflectanceDistributionFunc(vec3 n, vec3 v, int index) {
    vec3 diffuse = (1.0f - pbrMaterial.mSurface.x) * pbrAlbedoColor;
    vec3 intensity = uLight[index].uIntensity;
    vec3 l = vec3(0.0f);

    if (uLight[index].uDirectional && false) {
        l = normalize(uLight[index].uVector);
    } else {
        l = uLight[index].uVector - vPosModel;
        float dist = length(l);
        l = normalize(l);
        intensity /= (dist * dist);
    }

    vec3 h = normalize(l + v);
    float nDotH = clamp(dot(n, h), 0.0f, 1.0f);
    float lDotH = clamp(dot(l, h), 0.0f, 1.0f);
    float nDotL = clamp(dot(n, l), 0.0f, 1.0f);
    float nDotV = abs(dot(n, v)) + 1e-5;

    float d = ggxDistribution(nDotH);
    vec3 f = fSchlick(lDotH, vec3(0.04f));
    float gs = smithGeometryShadow(nDotV, nDotL);

    vec3 fr = (d * gs) * f;
    vec3 fd = diffuse * (1.0f / PI);

    /* 0.25f == fracion of 4 */
    //vec3 specular = 0.25f * ggxDistribution(nDotH) * shlickFresnel(lDotH) * smithGeometryShadow(nDotV, nDotL);
    return (fd + PI * fr) * intensity * nDotL;
}

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
        sum += bidirecionalReflectanceDistributionFunc(n, v, index);
    }

    sum = mix(uFog.uColor, sum, fogFactor);
    vFragColor = vec4(sum, 1.0f);
}