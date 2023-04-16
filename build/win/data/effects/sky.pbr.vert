#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 vPos;
out vec2 vTexCoord;
out vec3 vNormal; 

uniform mat4 uMVP;

void main() {
    gl_Position = uMVP * vec4(vPos, 1.0f);

    vPos = aPos;
    vTexCoord = aTexCoord;
    vNormal = aNormal;
}