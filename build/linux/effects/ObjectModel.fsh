#version 450 core
#define PI 3.1415926535897932384626433832795

layout (location = 0) out vec4 FinalColor;

in vec3 Normal;
in vec3 Position;

uniform struct LightInfo {
    vec3 Position;
    vec3 Intensity;
    bool Incoming;
} Light[100];

uniform struct MaterialInfo {
    float Rough;
    bool Metal;
    vec3 Color;
} Material;

uniform int LoadedLightsIndex;
uniform bool DebugLighting;

vec3 ShlickFresnel(float lDotH) {
    vec3 f0 = vec3(0.04);
    if (Material.Metal) {
        f0 = Material.Color;
    }
    return f0 + (1 - f0) * pow(1.0 - lDotH, 5.0);
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

vec3 MicrofacetModel(int lightIndice, vec3 normal) {
    vec3 diffuseBrdf = vec3(0.0);
    if (!Material.Metal) {
        diffuseBrdf = Material.Color;
    }

    vec3 l = vec3(0.0), lightI = Light[lightIndice].Intensity;
    if (Light[lightIndice].Incoming) {
        l = normalize(Light[lightIndice].Position);
    } else {
        l = Light[lightIndice].Position - Position;
        float dist = length(l);
        l = normalize(l);
        lightI /= (dist * dist);
    }

    vec3 v = normalize(-Position);
    vec3 h = normalize(v + l);

    float nDotH = dot(normal, h);
    float lDotH = dot(l, normal);
    float nDotL = max(dot(normal, l), 0.0);
    float nDotV = dot(normal, v);
    vec3 specBrdf = 0.25 * ggxDistribution(nDotH) * ShlickFresnel(lDotH) * GeometrySmith(nDotL) * GeometrySmith(nDotV);

    return (diffuseBrdf + PI * specBrdf) * lightI * nDotL;
}

void main() {
    vec3 sun = vec3(0);

    if (!DebugLighting) {
        vec3 n = normalize(Normal);

        if (!gl_FrontFacing) {
            n = -n;
        }

        for (int i = 0; i < LoadedLightsIndex; i++) {
            sun += MicrofacetModel(i, n);
        }

        sun = pow(sun, vec3(1.0 / 2.2));
    } else {
        //tsun = Material.Color;
    }

    FinalColor = vec4(sun, 1.0);
}