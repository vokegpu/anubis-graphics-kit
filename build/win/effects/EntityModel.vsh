#version 450 core

layout (location = 0) in vec3 VertexMesh;
layout (location = 1) in vec2 TextureCoordsMesh;
layout (location = 2) in vec3 NormalsMesh;

out vec3 Normals;

uniform mat4 MatrixCameraView;
uniform mat4 MatrixPerspective;
uniform mat4 MatrixModel;
uniform mat3 MatrixNormals;

void main() {
    gl_Position = MatrixPerspective * MatrixCameraView *  MatrixModel * vec4(1.0f);
}