#version 450

layout (location = 0) out vec4 vFragColor;

in vec3 vPos;
in vec2 vTexCoord;
in vec3 vNormal;

void main() {
    vFragColor = vec4(1.0f);
}