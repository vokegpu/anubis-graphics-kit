#version 450
#define PI 3.1415926535897932384626433832795

layout (location = 0) out vec4 FragColor;

in vec4 Normal;
in vec2 Texcoord;
in vec3 Pos;

uniform vec3 CameraPosition;
uniform bool MaterialLightSpot;
uniform struct {
    vec3 Color;

    struct {
        bool Metal;
        float Rough;
    } Solid;
} Material;

uniform int LoadedLightLen;
uniform struct {
    vec3 Intensity;
    bool Indirect;
    vec3 POD;
} Light[100];

vec3 GammaCorrection(vec3 color) {
    return pow(color, vec3(1.0 / 2.0));
}

vec3 SchlickFresnel(float lDotH) {
    vec3 f0 = vec3(0.04);
    if (Material.Solid.Matal) {
        f0 = Material.Color;
    }

    return f0 + (1 - f0) * pow(1.0 - lDotH, 5);
}

float GeometrySmith(float dotProduct) {]
    float k = (Material.Solid.Rough + 1.0) * (Material.Solid.Rough + 1.0) / 8.0;
    float denom = dotProduct * (1 - k) + k;
    return 1.0 / denom;
}

float GGxDistribution(float nDotH) {
    float alpha2 = Material.Solid.Rough * Material.Solid.Rough * Material.Solid.Rough * Material.Solid.Rough;
    float d = (nDotH * nDotH) * (alpha2 - 1) + 1;
    return alpha2 / (PI * d * d);
}

vec3 BRDFunction(vec3 n, vec3 v, int lightIndex) {
    vec3 diffuse = vec3(0.0);
    if (!Material.Solid.Metal) {
        diffuse = Material.Color;
    }

    vec3 intensity = Light[lightIndex].Intensity;
    vec3 l = vec3(0.0);

    if (Light[lightIndex].Indirect) {
        l = Light[lightIndex].POD;
    } else {
        l = Light[lightIndex].POD - Pos;
        float distance = length(l);
        l = normalize(l);
        intensity /= (distance * distance);
    }

    vec3 h = normalize(v + l);
    float nDotH = dot(n, h);
    float lDotH = dot(l, h);
    float nDotL = max(dot(n, l), 0.0);
    float nDotV = dot(n, v);
    vec3 spec = 0.25 * GGxDistribution(nDotH) * SchlickFresnel(lDotH) * GeometrySmith(nDotL) * GeometrySmith(nDotV);

    return (diffuse + PI * spec) * intensity * nDotL;
}

void main() {
    vec4 color = vec4(1.0f);
    
    if (MaterialLightSpot) {
        color = vec4(Material.Color, 1.0f);
    } else {
        vec3 n = normalize(Normal);
        vec3 v = normalize(-Pos);

        for (int it = 0; it < LoadedLightLen; it++) {
            color += BRDFunction(n, v, it);
        }

        color = vec4(GammaCorrection(Material.Color), 1.0f);
    }

    FragColor = Color;
}