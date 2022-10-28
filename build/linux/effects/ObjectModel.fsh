#version 450 core

layout (location = 0) out vec4 FinalColor;

in vec3 Normal;
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
    vec3 diffuseBrdf = vec3(0.0);
    if (!Material.Metal) {
        diffuseBrdf = Material.Color;
    }

    vec3 l = vec3(0.0), lightI = Light[lightIndice].L;
    if (Light[lightIndice].Position.w == 0.0) {
        l = normalize(Light[lightIndice].Position.xyz);
    } else {
        l = Light[lightIndice].Position.xyz - position;
        float dist = length(l);
        l = normalize(l);
        lightI /= dist * dist;
    }

    vec3 v = normalize(-position);
    vec3 h = normalize(v + l);
    float nDotH = dot(normal, h);
    float lDotH = dot(l, normal);
    float nDotL = max(dot(normal, l), 0.0);
    float nDotV = dot(normal, v);
    vec3 specBrdf = 0.25 * ggxDistribution(nDotH) * ShlickFresnel(lDotH) * GeometrySmith(nDotL) * GeometrySmith(nDotV);
    return  (diffuseBrdf + PI * specBrdf) * lightI * nDotL;
}

vec3 DoBRDF(vec3 normal) {
    vec3 sum = vec3(0), n = normalize(normal);
    for (int i = 0; i < 3; i++) {
        sum += MicrofacetModel(i, VertexPosition, n);
    }

    return pow(sum, vec3(1.0 / 2.0));
}

void main() {
    vec3 Color = vec3(0);
    vec3 n = Normal;

    if (!gl_FrontFacing) {
        n = -n;
    }

    Color += DoBRDF(n);
    FinalColor = vec4(Color, 1.0);
}