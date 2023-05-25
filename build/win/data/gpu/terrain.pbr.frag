#version 450
#define PI 3.1415926535897932384626433832795
#define TERRAIN_HEIGHT_SIZE 128.0f

#define SAND 0
#define ROCK 1
#define STONE 2

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
uniform bool uIsNight;

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
    bool uDirectional;
    vec3 uVector;
} uLight[100];

vec3 mCurrentMaterialRGB;

vec3 fSchlick(float lDotH, vec3 f0) {
    f0 = uMaterial.uMetal ? uMaterial.uColor : f0;
    return f0 + (1.0f - f0) * pow(1.0f - lDotH, 5.0f);
}

float fSchlick(float dotProd, float f0, float f90) {
    return f0 + (f90 - f0) * pow(1.0 - dotProd, 5.0);
}

float smithGeometryShadow(float nDotV, float nDotL) {
    float a = uMaterial.uRough * uMaterial.uRough;
    float v = nDotL * sqrt(nDotV * nDotV * (1.0f - a) + a);
    float l = nDotV * sqrt(nDotL * nDotL * (1.0f - a) + a);
    return 0.5f / (v + l);
}

float fdBurley(float nDotV, float nDotL, float lDotH) {
    float f90 = 0.5f + 2.0f * uMaterial.uRough * lDotH * lDotH;
    float lightScatter = fSchlick(nDotL, 1.0f, f90);
    float viewScatter = fSchlick(nDotV, 1.0f, f90);
    return lightScatter * viewScatter * (1.0f / PI);
}

float ggxDistribution(float nDotH) {
    float a = uMaterial.uRough * uMaterial.uRough;
    float k = (nDotH * nDotH) * (a - 1.0f) + 1.0f;
    return a / (PI * k * k);
}

float geometryDistribution(float dotProd) {
    float a = uMaterial.uRough * uMaterial.uRough;
    float k = (a + (1.0f - a) * dotProd * dotProd);
    return dotProd * 2.0f / k;
}

vec3 bidirecionalReflectanceDistributionFunc(vec3 n, vec3 v, int index) {
    vec3 diffuse = (1.0f - float(uMaterial.uMetal)) * uMaterial.uColor;
    vec3 intensity = uLight[index].uIntensity;
    vec3 l = vec3(0.0f);

    if (uLight[index].uDirectional) {
        l = normalize(uLight[index].uVector);
    } else {
        l = uLight[index].uVector - vPosModel;
        float dist = length(l);
        l = normalize(l);
        intensity /= dist * dist;
    }

    vec3 h = normalize(l + v);
    float nDotH = clamp(dot(n, h), 0.0f, 1.0f);
    float lDotH = clamp(dot(l, h), 0.0f, 1.0f);
    float nDotL = clamp(dot(n, l), 0.0f, 1.0f);
    float nDotV = dot(n, v);

    float d = ggxDistribution(nDotH);
    vec3 f = fSchlick(lDotH, vec3(0.04f));
    float gs = smithGeometryShadow(nDotV, nDotL);

    vec3 fr = (d * gs) * f;
    vec3 fd = diffuse * (1.0f / PI);

    /* 0.25f == fracion of 4 */
    //vec3 specular = 0.25f * ggxDistribution(nDotH) * shlickFresnel(lDotH) * smithGeometryShadow(nDotV, nDotL);
    return (fd + fr) * intensity * nDotL;
}

vec2 transform2Modal(vec4 atlas) {
    return (vTessCoord * atlas.zw) + atlas.xy;
}

void main() {
    float dist = sqrt(vPos.x * vPos.x + vPos.y * vPos.y + vPos.z * vPos.z);
    float fogFactor = clamp((uFog.uDistance.y - dist) / (uFog.uDistance.y - uFog.uDistance.x), 0.0f, 1.0f);

    float g = vHeight / 64.0f;
    int choosenType = 0;

    if (g < 0.3f) {
        choosenType = ROCK;
    } else {
        choosenType = SAND;
    }

    mCurrentMaterialRGB = texture(uTextureAtlas, transform2Modal(uAtlas[choosenType].uTexCoord)).rgb;
    vec3 sum = mCurrentMaterialRGB;
    float ambientColor = uAmbientColor;

    if (uIsNight) {
        vec3 nightColor = vec3(0.0156862745f, 0.02745098039f, 0.12549019607f);
        sum = mix(mCurrentMaterialRGB, nightColor, 1.0f - (uAmbientColor / uAmbientLuminance));
        ambientColor = clamp(uAmbientColor, 0.4f, 1.0f);
    }

    sum = (sum * g) * (ambientColor / uAmbientLuminance);

    vec3 n = normalize(vNormal);
    vec3 v = normalize(uCameraPos - vPosModel);

    for (int index = 0; index <= uLightAmount; index++) {
        sum += bidirecionalReflectanceDistributionFunc(n, v, index);
    }

    sum = mix(uFog.uColor, sum, fogFactor);
    vFragColor = vec4(sum, 1.0f);
}