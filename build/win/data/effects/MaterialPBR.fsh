#version 450 core
#define PI 3.1415926535897932384626433832795

layout (location = 0) out vec4 FragColor;

in vec3 Pos;
in vec2 Texcoord;
in vec3 Normal;

uniform vec3 CameraPosition;
uniform struct {
    vec3 Color;
    bool Metal;
    float Rough;
} Material;

uniform int LoadedLightLen;
uniform struct {
    vec3 Intensity;
    bool Directional;
    vec3 POD;
} Light[100];

vec3 GammaCorrection(vec3 color) {
    return pow(color, vec3(1.0 / 2.0));
}

vec3 SchlickFresnel(float lDotH) {
    vec3 f0 = vec3(0.04);
    if (Material.Metal) {
        f0 = Material.Color;
    }

    return f0 + (1 - f0) * pow(1.0 - lDotH, 5);
}

float GeometrySmith(float dotProduct) {
    float k = (Material.Rough + 1.0) * (Material.Rough + 1.0) / 8.0;
    float denom = dotProduct * (1 - k) + k;
    return 1.0 / denom;
}

float GGxDistribution(float nDotH) {
    float alpha2 = Material.Rough * Material.Rough * Material.Rough * Material.Rough;
    float d = (nDotH * nDotH) * (alpha2 - 1) + 1;
    return alpha2 / (PI * d * d);
}

vec3 BRDFunction(vec3 n, vec3 v, int lightIndex) {
    vec3 diffuse = vec3(0.0);
    if (!Material.Metal) {
        diffuse = Material.Color;
    }

    vec3 intensity = Light[lightIndex].Intensity;
    vec3 l = vec3(0.0);

    if (Light[lightIndex].Directional) {
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
    vec3 color = vec3(1.0f);
    
    if (Material.Rough == 0.0f) {
        color = Material.Color;
    } else {
        vec3 n = normalize(Normal);
        vec3 v = normalize(-Pos);

        for (int it = 0; it < LoadedLightLen; it++) {
            color += BRDFunction(n, v, it);
        }

        color = GammaCorrection(Material.Color);
    }

    FragColor = vec4(color, 1.0f);
}