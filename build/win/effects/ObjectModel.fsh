#version 450 core
#define PI 3.1415926535897932384626433832795

layout (location = 0) out vec4 FinalColor;

in vec3 Normal;
in vec3 Position;

uniform struct LightInfo {
    vec3 Position;
    vec3 Intensity;
    bool Incoming;
    bool Blinn;

    vec3 La;
    vec3 Ld;
    vec3 Ls;
} Light[100];

uniform struct MaterialInfo {
    float Rough;
    bool Metal;
    vec3 Color;

    vec3 Ka;
    vec3 Kd;
    vec3 Ks;

    int Shininess;
} Material;

uniform int LoadedLightsIndex;
uniform bool DebugLighting;
uniform vec3 CameraPosition;

vec3 ShlickFresnel(float lDotH) {
    vec3 f0 = vec3(0.04);
    if (Material.Metal) {
        f0 = Material.Color;
    }
    return f0 + (1.0 - f0) * pow(clamp(1.0 - lDotH, 0.0f, 1.0f), 5.0);
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

vec3 MicrofacetModel(vec3 v, int lightIndice, vec3 n) {
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

    vec3 h = normalize(v + l);

    float nDotH = dot(n, h);
    float lDotH = dot(l, h);
    float nDotL = max(dot(n, l), 0.0);
    float nDotV = dot(n, v);
    vec3 specBrdf = 0.25 * ggxDistribution(nDotH) * ShlickFresnel(lDotH) * GeometrySmith(nDotL) * GeometrySmith(nDotV);

    return (diffuseBrdf + PI * specBrdf) * lightI * nDotL;
}

vec3 BlinnPhongModel(vec3 v, int lightIndice, vec3 n) {
    float str = 0.1;
    vec3 ambient = str * Light[lightIndice].Intensity;

    vec3 lightDir = normalize(Light[lightIndice].Position - CameraPosition);
    float diff = max(dot(n, lightDir), 0.0);
    vec3 diffuse = diff * Light[lightIndice].Intensity;

    float sspec = 0.5f;
    vec3 viewdir = normalize(CameraPosition - Position);
    vec3 ref = reflect(-lightDir, n);
    float spec = pow(max(dot(viewdir, ref), 0.0), Material.Shininess);
    return (ambient + diff + spec) * Material.Color;
}

void main() {
    vec3 sun = vec3(0);

    if (!DebugLighting) {
        vec3 n = normalize(Normal);
        vec3 v = normalize(CameraPosition - Position);

        for (int i = 0; i < LoadedLightsIndex; i++) {
            if (Light[i].Blinn) {
                sun += BlinnPhongModel(v, i, n);
            } else {
                sun += MicrofacetModel(v, i, n);
            }
        }
    } else {
        sun = Material.Color;
    }

    FinalColor = vec4(sun, 1.0);
}