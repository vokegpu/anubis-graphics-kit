#version 450 core

layout (location = 0) out vec4 FragColor;

in vec4 Rect;
in vec2 Texcoord;

uniform vec4 Color;
uniform sampler2D ActiveTexture;
uniform bool TextureEnabled;

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

uniform struct {
    bool HighDynamicRange;
    float AveLuminanceHDR;
} Effects;

void main() {
    vec4 sum = Color;
    if (TextureEnabled) {
        sum = texture(ActiveTexture, Texcoord);
        sum = vec4(sum.rgb, sum.a * Color.a);
    }

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

        vec4 toneMapColor = vec4(XYZtoRGB * xyzCol, 1.0);
        sum = toneMapColor;
    }

    FragColor = sum;
}