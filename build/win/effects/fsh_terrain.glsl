#version 450 core

layout (location = 0) out vec4 OutColor;

in vec3 TextureCoordinates;
in vec3 Position;
in vec3 Normal;

uniform vec3 Color;

void main() {
    OutColor = vec4(Color, 1.0); 
}