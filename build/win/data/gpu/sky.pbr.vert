#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 vPos;
out vec2 vTexCoord;
out vec3 vNormal;
out float vStarsLuminance;
out int vStarsRendering;

uniform mat4 uMVP;
uniform float uStarsLuminance;
uniform bool uStarsRendering;

float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    if (!uStarsRendering) {
        gl_Position = uMVP * vec4(aPos, 1.0f);
    } else {
        gl_Position = uMVP * vec4(aPos + (uStarsLuminance * 0.001f), 1.0f);
        gl_PointSize = clamp(rand(aPos.zx * sin(clamp(uStarsLuminance, 0.1f, 50000.0f))) * (2.43443543f), 1.0f, 1.43443543f);
    }

    vStarsLuminance = uStarsLuminance;
    vStarsRendering = int(uStarsRendering);

    vPos = aPos;
    vTexCoord = aTexCoord;
    vNormal = aNormal;
}