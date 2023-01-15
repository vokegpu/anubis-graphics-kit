#version 450 core

layout (location = 0) out vec4 FragColor;

in float Height;
in vec3 Pos;

uniform struct {
    vec2 Distance;
    vec3 Color;
} Fog;

void main() {
    float dist = length(Pos);
    float fogFactor = clamp((Fog.Distance.y - dist) / (Fog.Distance.y - Fog.Distance.x), 0.0, 1.0);

    vec3 finalColor = vec3((Height + 16) / 64.0f);
    finalColor = mix(Fog.Color, finalColor, fogFactor);
    FragColor = vec4(finalColor, 1.0f);
}