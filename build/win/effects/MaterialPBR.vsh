#version 450

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 TextureCoordinate;
layout (location = 2) in vec3 FaceNormal;

out vec3 Pos;
out vec2 Texcoord;
out vec3 Normal;

uniform mat4 MVP;
uniform mat4 MatrixModel;
uniform mat3 MatrixNormal;

void main() {
    Position = MatrixModel * vec4(VertexPosition, 1.0f);
    Texcoord = TextureCoordinate;
    Normal = FaceNormal * MatrixNormal;

    gl_Position = MVP * vec4(VertexPos, 1.0f);
}