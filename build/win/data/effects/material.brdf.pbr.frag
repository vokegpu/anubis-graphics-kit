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

vec3 shlickFresnel(float lDotH) {
    vec3 f0 = vec3(0.04f);
    if (pbrMaterial.mSurface.x == 1) {
        f0 = pbrAlbedoColor;
    }
    return f0 + (1 - f0) * pow(1.0f - lDotH, 5.0f);
}

float geometrySmith(float dotProd) {
    float k = (pbrRoughnessFactor + 1.0f) * (pbrRoughnessFactor + 1.0f) / 8.0f;
    float denom = dotProd * (1.0f - k) + k;
    return 1.0f / denom;
}

float ggxDistribution(float nDotH) {
    float a2 = pbrRoughnessFactor * pbrRoughnessFactor * pbrRoughnessFactor * pbrRoughnessFactor;
    float d = (nDotH * nDotH) * (a2 - 1.0f) + 1.0f;
    return a2 / (PI * d * d);
}

vec3 bidirecionalReflectanceDistributionFunc(vec3 n, vec3 v, int index) {
    vec3 diffuse = vec3(0.0f);
    if (pbrMaterial.mSurface.x == 0) {
        diffuse = pbrAlbedoColor;
    }

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

    vec3 h = normalize(v + l);
    float nDotH = dot(n, h);
    float lDotH = dot(l, h);
    float nDotL = max(dot(n, l), 0.0f);
    float nDotV = dot(n, v);

    /* 0.25f == fracion of 4 */
    vec3 specular = 0.25f * ggxDistribution(nDotH) * shlickFresnel(lDotH) * geometrySmith(nDotL) * geometrySmith(nDotV);
    return (diffuse + PI * specular) * intensity * nDotL;
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