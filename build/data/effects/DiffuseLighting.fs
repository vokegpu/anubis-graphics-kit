#version 330 core

in vec3 LightItensity;
layout (location = 0) out vec4 FragColor;

void main() {
	FragColor = vec4(LightItensity, 1.0f);
}