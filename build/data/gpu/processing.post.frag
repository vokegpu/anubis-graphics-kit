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

uniform mat4 uInversePerspectiveView;
uniform mat4 uPreviousPerspectiveView;

uniform struct {
    /* HDR */
    bool  uHDREnabled;
    float uHDRAverageLuminance;
    float uHDRExposure;

    /* Motion Blur */
    bool  uMotionBlurEnabled;
    bool  uMotionBlurCameraRotated;
    float uMotionBlurIntensity;
} uEffects;

void main() {
    vec2 size = textureSize(uTexture, 0);
    vec2 fragCoord = gl_FragCoord.xy / size;
    vec4 sum = vec4(0.0f);

    if (uEffects.uMotionBlurEnabled && uEffects.uMotionBlurCameraRotated) {
        vec4 h = vec4(fragCoord.x * 2 - 1, (1.0f - fragCoord.y) * 2 - 1, texture(uDepthSampler, fragCoord).r / 1.0f, 1.0f);
        vec4 d = uInversePerspectiveView * h;
        vec4 w = d / d.w;

        vec4 currPos = h;
        vec4 prevPos = uPreviousPerspectiveView * w;
        prevPos = prevPos / prevPos.w;

        vec2 velocity = currPos.xy - prevPos.xy;
        float numSamples = 20;

        float distToCenter = length(gl_FragCoord.x - (size.x / 2)) / size.x;
        float dynmaicIntensity = clamp(length(velocity) * 0.1f, 0.0f, uEffects.uMotionBlurIntensity * 0.1f);

        for (float index = 0; index < numSamples; index++) {
            vec2 nearestTexCoord = fragCoord + (vec2(((index / (numSamples - 1.0f)) - 0.5f) * (dynmaicIntensity) * distToCenter, 0.0f));

            if (nearestTexCoord.x > 1.0f || nearestTexCoord.y > 1.0f || nearestTexCoord.x < 0.0f || nearestTexCoord.y < 0.0f) {
                nearestTexCoord = fragCoord;
            }

            sum += texture(uTexture, nearestTexCoord);
        }

        sum = sum / numSamples;
    } else {
        sum = texture(uTexture, fragCoord);
    }

    if (uEffects.uHDREnabled) {
        float white = 0.928f;
        float exposure = uEffects.uHDRExposure;

        vec3 xyzCol = RGBtoXYZ * vec3(sum);
        float xyzSum = xyzCol.x + xyzCol.y + xyzCol.z;
        vec3 xyYCol = vec3(0.0f);
        if (xyzSum > 0.0f) xyYCol = vec3(xyzCol.x / xyzSum, xyzCol.y / xyzSum, xyzCol.y);

        float l = (exposure * xyYCol.z) / uEffects.uHDRAverageLuminance;
        l = (l * (1 + l / (white * white))) / (1.0f + l);

        xyzCol.x = (l * xyYCol.x) / (xyYCol.y);
        xyzCol.y = l;
        xyzCol.z = (l * (1.0f - xyYCol.x - xyYCol.y)) / xyYCol.y;

        sum = vec4(XYZtoRGB * xyzCol, 1.0f);
    }

    vFragColor = sum;
}