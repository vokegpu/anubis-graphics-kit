#version 450

layout (location = 0) out vec4 vFragColor;
layout (binding = 0) uniform sampler2D uTexture;
layout (binding = 1) uniform sampler2D uDepthSampler;

in vec4 vRect;
uniform vec4 uColor;

const mat3 RGBtoXYZ = mat3(
    0.4124564f, 0.2126729f, 0.0193339f,
    0.3575761f, 0.7151522f, 0.1191920f,
    0.1804375f, 0.0721750f, 0.9503041f
);

const mat3 XYZtoRGB = mat3(
    3.2404542f, -0.9692660f, 0.0556434f,
    -1.5371385f, 1.8760108f, -0.2040259f,
    -0.4985314f, 0.0415560f, 1.0572252f
);

uniform mat4 uMVP;
uniform mat4 uInverseMVP;

uniform struct {
    bool uEnabled;
    float uAverageLuminance;
    float uExposure;
    float uWhite;
} uHDR;

void main() {
    vec2 fragCoord = gl_FragCoord.xy / textureSize(uTexture, 0);
    vec4 framebufferColor = texture(uTexture, fragCoord);
    vec4 framebufferDepth = texture(uDepthSampler, fragCoord);

    float dep = framebufferDepth.r;
    vec4 h = vec4(fragCoord.x * 2 - 1, (1.0f - fragCoord.y) * 2 - 1, dep, 1.0f);
    vec4 d = uInverseMVP * h;
    vec4 w = d / d.w;

    vec4 currPos = h;
    vec4 prevPos = uMVP * w;
    prevPos = prevPos / prevPos.w;

    vec2 velocity = (currPos.xy - prevPos.xy);
    vec2 texCoord = fragCoord + velocity;
    int numSamples = 1;
    if (velocity.x == 0 && velocity.y == 0) {
        numSamples = 1;
    }

    for (int i = 1; i < numSamples; ++i) {
        texCoord += velocity;
        vec4 currColor = texture(uTexture, texCoord);
        framebufferColor += currColor;
    }

    vec4 sum = framebufferColor / numSamples;

    if (uHDR.uEnabled) {
        float white = 0.928f;
        float exposure = 0.35f;

        vec3 xyzCol = RGBtoXYZ * vec3(sum);

        float xyzSum = xyzCol.x + xyzCol.y + xyzCol.z;
        vec3 xyYCol = vec3(0.0f);
        if (xyzSum > 0.0f) xyYCol = vec3(xyzCol.x / xyzSum, xyzCol.y / xyzSum, xyzCol.y);

        float l = (exposure * xyYCol.z) / uHDR.uAverageLuminance;
        l = (l * (1 + l / (white * white))) / (1.0f + l);

        xyzCol.x = (l * xyYCol.x) / (xyYCol.y);
        xyzCol.y = l;
        xyzCol.z = (l * (1.0f - xyYCol.x - xyYCol.y)) / xyYCol.y;

        sum = vec4(XYZtoRGB * xyzCol, 1.0f);
    }

    vFragColor = sum;
}