#version 450 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 VertexTextureCoords;
layout (location = 2) in vec3 VertexNormal;

out vec3 Normal;
out vec3 Position;

uniform mat4 MVP;
uniform mat4 ModelMatrix;
uniform mat3 MatrixNormal;

void main() {
    Normal = normalize(MatrixNormal * VertexNormal);
    Position = (ModelMatrix * vec4(VertexPosition, 1.0f)).xyz;
    gl_Position = MVP * vec4(Position, 1.0f);
}