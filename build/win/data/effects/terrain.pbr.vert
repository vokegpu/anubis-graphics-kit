#version 450 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 TextureCoordinates;
layout (location = 2) in vec3 VertexNormal;
layout (location = 3) in vec3 VertexColor;

uniform mat4 MVP;

out vec2 TexCoord;
out vec3 Color;
out vec3 Pos;

void main() {
    gl_Position = MVP * vec4(VertexPosition, 1.0);
    TexCoord = TextureCoordinates;
    Color = VertexColor;
    Pos = gl_Position.xyz;
}