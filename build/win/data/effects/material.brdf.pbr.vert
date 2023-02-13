#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

layout (location = 3) in mat4 aInstancedModelMatrix;
layout (locaiton = 4) in mat3 aInstancedNormalMatrix;

out vec3 vPos;
out vec3 vPosModel;
out vec2 vTexCoord;
out vec3 vNormal;

uniform bool uInstanced = false;
uniform mat4 uMVP;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;
uniform mat4 uPerspectiveViewMatrix;

void main() {
    if (uInstanced) {
        mat4 mvp = uPerspectiveViewMatrix * aInstancedModelMatrix;
        gl_Position = mvp * vec4(aPos, 1.0f);
        vec4 modelPos = aInstancedModelMatrix * vec4(aPos, 1.0f);
        vNormal = normalize(aInstancedNormalMatrix * aNormal);
    } else {
        gl_Position = uMVP * vec4(aPos, 1.0f);
        vec4 modelPos = uModelMatrix * vec4(aPos, 1.0f);
        vNormal = normalize(uNormalMatrix * aNormal);
    }

    vPos = gl_Position.xyz;
    vPosModel = modelPos.xyz;
    vTexCoord = aTexCoord;
}