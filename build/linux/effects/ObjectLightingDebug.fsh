#version 450 core

layout (location = 0) out vec4 FinalColor;

out vec3 Position;
uniform vec3 Color;

void main() {
    FinalColor = vec4(Position, 1);
}