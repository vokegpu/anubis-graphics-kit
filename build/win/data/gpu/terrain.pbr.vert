#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aColor;

out vec2 vTexCoord_VertStage;

void main() {
    gl_Position = vec4(aPos, 1.0);
    vTexCoord_VertStage = aTexCoord;
}