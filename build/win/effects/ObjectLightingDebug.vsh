#version 450 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexTextureCoords;
layout (location = 2) in vec3 VertexNormal;

out vec3 Position;

uniform mat4 MVP;
uniform mat4 ViewModelMatrix;

vec4 VertexPosition4f;

void main() {
    VertexPosition4f = vec4(VertexPosition, 1.0);
    gl_Position = MVP * VertexPosition4f;

    Position = (ViewModelMatrix * VertexPosition4f).xyz;
}