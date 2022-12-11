#version 450 core

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 TextureCoords;
layout(location = 2) in mat4 ModelMatrices;
layout(location = 3) in mat3 NormalMatrices;

out vec3 Position;
out vec3 TextureCoordinates;
out vec3 Normal;

uniform mat4 PerspectiveView;

void main() {
    gl_Position = PerspectiveView * ModelMatrices * vec4(VertexPosition, 1.0);
    TextureCoordinates = TextureCoords;
    Position = VertexPosition;
    Normal = normalize(NormalMatrices * Normal);
}