#version 450

layout (location = 0) out vec4 vFragColor;
layout (binding = 0) uniform sampler2D uTexture;

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

uniform struct {
    bool uEnabled;
    float uAverageLuminance;
    float uExposure;
    float uWhite;
} uHDR;

uniform struct {
    bool uEnabled;
    float uIntensity;
    bool uCameraRotated;
    mat4 uInversePerspectiveView;
    mat4 uPreviousPerspectiveView;
} uMotionBlur;

void main() {
    vec2 size = textureSize(uTexture, 0);
    vec2 fragCoord = gl_FragCoord.xy / size;
    vec4 sum = vec4(0.0f);

    if (uMotionBlur.uEnabled && uMotionBlur.uCameraRotated) {
        // @TODO Fix depth sampler to get precisely pixel coord at world.

        vec4 h = vec4(fragCoord.x * 2 - 1, (1.0f - fragCoord.y) * 2 - 1, 0.0f, 1.0f);
        vec4 d = uMotionBlur.uInversePerspectiveView * h;
        vec4 w = d / d.w;

        vec4 currPos = h;
        vec4 prevPos = uMotionBlur.uPreviousPerspectiveView * w;
        prevPos = prevPos / prevPos.w;

        vec2 velocity = currPos.xy - prevPos.xy;
        vec2 dir = normalize(velocity);
        float numSamples = 20;

        float p = abs(gl_FragCoord.x - (size.x / 2));
        float distToCenter = p / (size.x / 2);
        float aspectRatio = size.x / size.y;
        float l = clamp((length(velocity) * uMotionBlur.uIntensity) * (distToCenter), 0.0f, 1.0f);

        for (float index = 0; index < numSamples; index++) {
            vec2 nearestUv = fragCoord + (vec2(((index / (numSamples - 1)) - 0.5f) * l * aspectRatio, 0) * dir);
            sum += texture(uTexture, nearestUv);
        }

        sum = sum / numSamples;
    } else {
        sum = texture(uTexture, fragCoord);
    }

    if (uHDR.uEnabled) {
        float white = 0.928f;
        float exposure = uHDR.uExposure;

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