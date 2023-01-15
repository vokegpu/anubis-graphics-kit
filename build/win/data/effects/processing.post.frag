#version 450 core

layout (location = 0) out vec4 FragColor;

in vec4 Rect;
in vec2 Texcoord;

uniform vec4 Color;
uniform sampler2D TextureActiveSlot;
uniform bool TextureEnabled;

void main() {
    vec4 Sum = Color;
    if (TextureEnabled) {
        Sum = texture(TextureActiveSlot, Texcoord);
        Sum = vec4((1.0f - Sum.rgb) - 1.0f, Sum.a * Color.a);
    }

    FragColor = Sum;
}