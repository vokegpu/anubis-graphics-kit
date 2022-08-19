#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 TextCoords;
layout (location = 2) in vec3 FaceNormals;

uniform mat4 ModelViewMatrix;
uniform mat4 CameraViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;

out vec3 Normal;
out vec4 Pos;

void main() {
	gl_Position = ProjectionMatrix * CameraViewMatrix * ModelViewMatrix * vec4(VertexPosition, 1.0f);
	Pos = ModelViewMatrix * vec4(VertexPosition, 1.0f);
	Normal = normalize(NormalMatrix * FaceNormals);
}