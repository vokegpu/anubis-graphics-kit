#version 450 core

layout (location = 0) out vec4 FinalColor;

in vec3 Normals;
in vec3 VertexPosition;

void main() {
    FinalColor = vec4(1.0f);
}