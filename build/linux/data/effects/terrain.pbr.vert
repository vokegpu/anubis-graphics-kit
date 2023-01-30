#version 450 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 TextureCoordinates;
layout (location = 2) in vec3 VertexNormal;
layout (location = 3) in vec3 VertexColor;

out vec2 TexCoord;
out vec3 Pos;

void main() {
    gl_Position = vec4(VertexPosition, 1.0);
    TexCoord = TextureCoordinates;
    Pos = gl_Position.xyz;
}