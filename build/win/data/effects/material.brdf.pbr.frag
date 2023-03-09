#version 450
#define PI 3.1415926535897932384626433832795

layout (location = 0) out vec4 vFragColor;

// PBR sampler(s)
layout (binding = 0) uniform sampler2D uSamplerAlbedo;
layout (binding = 1) uniform sampler2D uSamplerSpecular;
layout (binding = 2) uniform sampler2D uSamplerNormalMap;

struct Material {
    vec4 mSamplerFlags;
    vec4 mSurface;
    vec4 mColor;
};

// Material buffer
layout (std140, binding = 0) uniform uniformBufferMaterial {
    Material ubMaterial[512];
};

in vec3 vPos;
in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vPosModel;

uniform vec3 uCameraPos;
uniform float uAmbientColor;
uniform int uLightAmount;
uniform float uGamma = 1.0f;
uniform int uMaterialIndex;

Material material;
vec3 colorSpecular;
vec3 colorAlbedo;
vec3 colorNormalMap;

uniform struct {
    vec3 uIntensity;
    bool uDirectional;
    vec3 uVector;
} uLight[100];

uniform struct {
    vec2 uDistance;
    vec3 uColor;
} uFog;

vec3 shlickFresnel(float lDotH) {
    vec3 f0 = vec3(0.04f);
    if (material.mSurface.x == 1) {
        f0 = colorAlbedo;
    }
    return f0 + (1 - f0) * pow(clamp(1.0f - lDotH, 0.0f, 1.0f), 5.0f);
}

float geometrySmith(float dotProd) {
    float k = (material.mSurface.y + 1.0f) * (material.mSurface.y + 1.0f) / 8.0f;
    float denom = dotProd * (1.0f - k) + k;
    return 1.0f / denom;
}

float ggxDistribution(float nDotH) {
    float a2 = material.mSurface.y * material.mSurface.y * material.mSurface.y * material.mSurface.y;
    float d = (nDotH * nDotH) * (a2 - 1.0f) + 1.0f;
    return a2 / (PI * d * d);
}

vec3 bidirecionalReflectanceDistributionFunc(vec3 n, vec3 v, int index) {
    vec3 diffuse = vec3(0.0f);
    if (material.mSurface.x == 0) {
        diffuse = colorAlbedo;
    }

    vec3 intensity = uLight[index].uIntensity;
    vec3 l = vec3(0.0f);

    if (uLight[index].uDirectional) {
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
    float nDotL = max(dot(n, l), 1.0f);
    float nDotV = dot(n, v);

    /* 0.25f == fracion of 4 */
    vec3 specular = 0.25f * ggxDistribution(nDotH) * shlickFresnel(lDotH) * geometrySmith(nDotL) * geometrySmith(nDotV);
    return (diffuse + PI * specular) * intensity * nDotL;
}

void main() {
    float dist = length(vPos);
    float fogFactor = clamp((uFog.uDistance.y - dist) / (uFog.uDistance.y - uFog.uDistance.x), 0.0, 1.0);
    material = ubMaterial[uMaterialIndex];
    vec2 texCoord = vec2(vTexCoord.x, 1.0f - vTexCoord.y);

    // Not all models come with albedo, specular, and normal map textures, then set to the color.
    colorAlbedo = material.mColor.rgb;
    colorSpecular = material.mColor.rgb;
    colorNormalMap = vec3(0.0f);

    if (material.mSamplerFlags.x == 1) {
        colorAlbedo = texture(uSamplerAlbedo, texCoord).rgb;
    }

    if (material.mSamplerFlags.y == 1) {
        colorSpecular = texture(uSamplerSpecular, texCoord).rgb;
    }

    if (material.mSamplerFlags.z == 1) {
        colorNormalMap = texture(uSamplerNormalMap, texCoord).rgb;
    }

    vec3 sum = colorAlbedo * uAmbientColor;
    vec3 n = normalize(vNormal);
    vec3 v = normalize(uCameraPos - vPosModel);

    if (!gl_FrontFacing) n = -n;
    for (int index = 0; index <= uLightAmount; index++) {
        sum += bidirecionalReflectanceDistributionFunc(n, v, index);
    }

    sum = mix(uFog.uColor, sum, fogFactor);
    vFragColor = vec4(sum, 1.0f);
}