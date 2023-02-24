#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

layout (location = 3) in mat4 aInstancedModelMatrix;

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
    vec4 modelPos = vec4(1.0f);

    if (uInstanced) {
        mat4 mvp = uPerspectiveViewMatrix * aInstancedModelMatrix;
        gl_Position = mvp * vec4(aPos, 1.0f);
        modelPos = aInstancedModelMatrix * vec4(aPos, 1.0f);
        vNormal = normalize(aNormal);
    } else {
        gl_Position = uMVP * vec4(aPos, 1.0f);
        modelPos = uModelMatrix * vec4(aPos, 1.0f);
        vNormal = normalize(aNormal);
    }

    vPos = gl_Position.xyz;
    vPosModel = modelPos.xyz;
    vTexCoord = aTexCoord;
}