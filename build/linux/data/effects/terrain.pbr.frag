#version 450
#define PI 3.1415926535897932384626433832795
#define TERRAIN_HEIGHT_SIZE 128.0f

#define SAND 0
#define STONE 1
#define ROCK 2

layout (location = 0) out vec4 vFragColor;
layout (binding = 1) uniform sampler2D uTextureAtlas;

in vec3 vPos;
in float vHeight;
in vec2 vTessCoord;
in vec3 vNormal;
in vec3 vPosModel;

uniform vec3 uCameraPos;
uniform float uAmbientColor;
uniform float uAmbientLuminance;
uniform int uLightAmount;

uniform struct {
    vec4 uTexCoord;
} uAtlas[12];

uniform struct {
    vec2 uDistance;
    vec3 uColor;
} uFog;

uniform struct {
    vec3 uColor;
    bool uMetal;
    float uRough;
} uMaterial;

uniform struct {
    vec3 uIntensity;
    bool uDirectioanl;
    vec3 uVector;
} uLight[100];

vec3 mCurrentMaterialRGB;

vec3 schlickFresnel(float lDotH) {
    vec3 f0 = vec3(0.04f);
    if (uMaterial.uMetal) {
        f0 = mCurrentMaterialRGB;
    }
    return f0 + (1.0f - f0) * pow(1.0f - lDotH, 5.0f);
}

float ggxDistrubution(float nDotH) {
    float a2 = uMaterial.uRough * uMaterial.uRough * uMaterial.uRough * uMaterial.uRough;
    float d = ((nDotH * nDotH) * (a2 - 1.0f) + 1.0f);
    return a2 / (PI * d * d);
}

float geometryDistribution(float dotProd) {
    float k = (uMaterial.uRough + 1.0f) * (uMaterial.uRough + 1.0f) / 8.0f;
    return 1.0f / (dotProd * (1.0f - k) + k);
}

vec3 bidirecionalReflectanceDistributionFunc(vec3 n, vec3 v, int index) {
    vec3 diffuse = vec3(0.0f);
    if (!uMaterial.uMetal) {
        diffuse = mCurrentMaterialRGB;
    }

    vec3 intensity = uLight[index].uIntensity;
    vec3 l = vec3(0.0f);

    if (uLight[index].uDirectioanl) {
        l = normalize(uLight[index].uVector);
    } else {
        l = uLight[index].uVector - vPosModel;
        float dist = length(l);
        l = normalize(l);
        intensity /= dist * dist;
    }

    vec3 h = normalize(l + v);
    float nDotH = dot(n, h);
    float lDotH = dot(l, h);
    float nDotL = max(dot(n, l), 0.0f);
    float nDotV = dot(n, v);

    /* 0.25f == same as divid by 4 */
    vec3 specular = 0.25f * ggxDistrubution(nDotH) * schlickFresnel(lDotH) * geometryDistribution(nDotL) * geometryDistribution(nDotV);
    return (diffuse + PI * specular) * intensity * nDotL;
}

vec2 transform2Modal(vec4 atlas) {
    return (vTessCoord * atlas.zw) + atlas.xy;
}

void main() {
    float dist = sqrt(vPos.x * vPos.x + vPos.y * vPos.y + vPos.z * vPos.z);
    float fogFactor = clamp((uFog.uDistance.y - dist) / (uFog.uDistance.y - uFog.uDistance.x), 0.0f, 1.0f);

    float g = vHeight / 64.0f;
    mCurrentMaterialRGB = texture(uTextureAtlas, transform2Modal(uAtlas[SAND].uTexCoord)).rgb;
    vec3 sum = (mCurrentMaterialRGB * g) * (uAmbientColor / uAmbientLuminance);

    vec3 n = normalize(vNormal);
    vec3 v = normalize(uCameraPos - vPosModel);

    for (int index = 0; index <= uLightAmount; index++) {
        sum += bidirecionalReflectanceDistributionFunc(n, v, index);
    }

    sum = mix(uFog.uColor, sum, fogFactor);
    vFragColor = vec4(sum, 1.0f);
}