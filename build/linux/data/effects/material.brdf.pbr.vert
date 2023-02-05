#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 vPos;
out vec2 vTexCoord;
out vec3 vNormal;

uniform mat4 uMVP;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0f);
    vec4 modelPos = uModelMatrix * vec4(aPos, 1.0f);

    vPos = modelPos.xyz;
    vTexCoord = aTexCoord;
    vNormal = normalize(uNormalMatrix * aNormal);
}