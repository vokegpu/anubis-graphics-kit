#version 450 core

layout (location = 0) out vec4 FinalColor;

in vec3 Normals;
in vec3 VertexPosition;

const float PI = 3.1415926535897932384626433832795;

uniform struct LightInfo {
    vec4 Position;
    vec3 L;
} Light[3];

uniform struct MaterialInfo {
    float Rough;
    bool Metal;
    vec3 Color;
} Material;

vec3 ShlickFresnel(float lDotH) {
    vec3 f0 = vec3(0.04);
    if (Material.Metal) {
        f0 = Material.Color;
    }
    return f0 + (1 - f0) * pow(1.0 - lDotH, 5);
}

float GeometrySmith(float dotProd) {
    float k = (Material.Rough + 1.0) * (Material.Rough + 1.0) / 8.0;
    float denom = dotProd * (1 - k) + k;
    return 1.0 / denom;
}

float ggxDistribution(float nDotH) {
    float alpha2 = Material.Rough * Material.Rough * Material.Rough * Material.Rough;
    float d = (nDotH * nDotH) * (alpha2 - 1) + 1;
    return alpha2 / (PI * d * d);
}

vec3 MicrofacetModel(int lightIndice, vec3 position, vec3 normal) {
    vec3 diffuseBrdf = vec3(1.0);
    if (!Material.Metal) {
        diffuseBrdf = Material.Color;
    }

    vec3 l = vec3(1.0), lightI = Light[lightIndice].L;
    if (Light[lightIndice].Position.w == 0.0) {
        l = normalize(Light[lightIndice].Position.xyz);
        float dist = length(l);
        l = normalize(l);
        lightI /= dist * dist;
    }

    vec3 v = normalize(-position);
    vec3 h = normalize(v + 1);
    float nDotH = dot(normal, h);
    float lDotH = dot(l, normal);
    float nDotL = max(dot(normal, l), 0.0);
    float nDotV = dot(normal, v);
    vec3 specBrdf = 0.25 * ggxDistribution(nDotH) * ShlickFresnel(lDotH) * GeometrySmith(nDotL) * GeometrySmith(nDotV);
    return  (diffuseBrdf + PI * specBrdf) * lightI * nDotL;
}

void main() {
    vec3 sum = vec3(0), n = normalize(Normals);
    for (int i = 0; i < 3; i++) {
        sum += MicrofacetModel(i, VertexPosition, n);
    }

    sum = pow(sum, vec3(1.0 / 2.0));
    FinalColor = vec4(sum, 1.0f);
}