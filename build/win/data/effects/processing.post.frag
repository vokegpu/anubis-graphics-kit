#version 450 core

layout (location = 0) out vec4 FragColor;
layout (binding = 0) uniform sampler2D FramebufferTexture;
layout (binding = 1) uniform sampler2D DepthbufferTexture;

in vec4 Rect;
in vec2 Texcoord;

uniform vec4 Color;

uniform mat3 RGBtoXYZ = mat3(
    0.4124564, 0.2126729, 0.0193339,
    0.3575761, 0.7151522, 0.1191920,
    0.1804375, 0.0721750, 0.9503041
);

uniform mat3 XYZtoRGB = mat3(
    3.2404542, -0.9692660, 0.0556434,
    -1.5371385, 1.8760108, -0.2040259,
    -0.4985314, 0.0415560, 1.0572252
);

uniform float Exposure = 0.35;
uniform float White = 0.928;
uniform float AveLuminance;
uniform mat4 PreviousMVP;
uniform mat4 InverseMVP;

uniform struct {
    bool HighDynamicRange;
    float AveLuminanceHDR;
} Effects;

void main() {
    vec4 sum = texture(FramebufferTexture, Texcoord);
    vec4 depw = texture(DepthbufferTexture, Texcoord);
    vec4 c = sum;
    float dep = depw.w;

    vec4 h = vec4(Texcoord.x * 2 - 1.0f, (1.0f - Texcoord.y) * 2 - 1, dep, 1);
    vec4 d = h * InverseMVP;
    vec4 worldPos = vec4(d.xyz / d.w, 1);
    vec4 currentPos = h;

    vec4 previousPos = worldPos * PreviousMVP;
    previousPos /= previousPos.w;

    vec2 texcoord = Texcoord;
    vec2 velocity = (currentPos.xy - previousPos.xy);

    for (int i = 1; i < 5; ++i) {
        texcoord += velocity;
        vec4 color = texture(FramebufferTexture, texcoord);
        sum += color;
    }

    sum = vec4(sum.xyz / 5, 1.0f);

    if (Effects.HighDynamicRange) {
        vec3 xyzCol = RGBtoXYZ * vec3(sum);

        float xyzSum = xyzCol.x + xyzCol.y + xyzCol.z;
        vec3 xyYCol = vec3(0);
        if (xyzSum > 0.0) xyYCol = vec3(xyzCol.x / xyzSum, xyzCol.y / xyzSum, xyzCol.y);

        float l = (Exposure * xyYCol.z) / AveLuminance;
        l = (l * (1 + l / (White * White))) / (1 + l);

        xyzCol.x = (l * xyYCol.x) / (xyYCol.y);
        xyzCol.y = l;
        xyzCol.z = (l * (1 - xyYCol.x - xyYCol.y)) / xyYCol.y;

        sum = vec4(XYZtoRGB * xyzCol, 1.0);
    }

    FragColor = sum;
}