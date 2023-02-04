#version 450 core
#define PI 3.1415926535897932384626433832795

layout (location = 0) out vec4 vFragColor;

in vec3 vPos;
in vec2 vTexCoord;
in vec3 vNormal;

uniform vec3 uCameraPos;
uniform float uAmbientColor;
uniform int uLightAmount;
uniform float uGamma = 1.0f;

vec3 mMaterialColor;

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

uniform struct {
    vec2 uDistance;
    vec3 uColor;
} uFog;

vec3 shlickFresnel(float lDotH) {
    vec3 f0 = vec3(0.04f);
    if (uMaterial.uMetal) {
        f0 = mMaterialColor;
    }
    return f0 + (1 - f0) * pow(clamp(1.0f - lDotH, 0.0f, 1.0f), 5.0f);
}

float geometrySmith(float dotProd) {
    float k = (uMaterial.uRough + 1.0f) * (uMaterial.uRough + 1.0f) / 8.0f;
    float denom = dotProd * (1.0f - k) + k;
    return 1.0f / denom;
}

float ggxDistribution(float nDotH) {
    float a2 = uMaterial.uRough * uMaterial.uRough * uMaterial.uRough * uMaterial.uRough;
    float d = (nDotH * nDotH) * (a2 - 1.0f) + 1.0f;
    return a2 / (PI * d * d);
}

vec3 bidirecionalReflectanceDistributionFunc(vec3 n, vec3 v, int index) {
    vec3 diffuse = vec3(0.0f);
    if (!uMaterial.uMetal) {
        diffuse = mMaterialColor;
    }

    vec3 intensity = uLight[index].uIntensity;
    vec3 l = vec3(0.0f);

    if (uLight[index].uDirectional) {
        l = normalize(uLight[index].uVector);
    } else {
        l = uLight[index].uVector - vPos;
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
    mMaterialColor = uMaterial.uColor;
    vec3 sum = mMaterialColor * uAmbientColor;

    if (uMaterial.uRough == -1) {
        sum = mMaterialColor;
    } else {
        vec3 n = normalize(vNormal);
        vec3 v = normalize(uCameraPos - vPos);

        if (!gl_FrontFacing) n = -n;
        for (int index = 0; index <= uLightAmount; index++) {
            sum += bidirecionalReflectanceDistributionFunc(n, v, index);
        }
    }

    sum = mix(uFog.uColor, sum, fogFactor);
    vFragColor = vec4(sum, 1.0f);
}