#version 450 core

layout (location = 0) in vec3 VertexMesh;
layout (location = 1) in vec2 TextureCoordsMesh;
layout (location = 2) in vec3 NormalsMesh;

out vec3 Normal;
out vec3 VertexPosition;

uniform mat4 MatrixCameraView;
uniform mat4 MatrixPerspective;
uniform mat4 MatrixModel;
uniform mat3 MatrixNormal;

void main() {
    vec4 ModelTransform = MatrixModel * vec4(VertexMesh, 1.0f);
    gl_Position = MatrixPerspective * MatrixCameraView * ModelTransform;

    Normal = normalize(MatrixNormal * NormalsMesh);
    VertexPosition = ModelTransform.xyz;
}