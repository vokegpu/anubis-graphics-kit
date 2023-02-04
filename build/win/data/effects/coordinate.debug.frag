#version 450

layout (location = 0) out vec4 vFragColor;
in vec3 vColor;

void main() {
    vFragColor = vec4(vColor, 1.0f);
}