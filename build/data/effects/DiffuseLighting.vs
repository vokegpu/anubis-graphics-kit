#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 0) in vec3 FaceNormals;

out vec3 LightItensity;

uniform vec4 LigthSpot;
uniform vec3 Kd;
uniform vec3 Ld;

uniform mat4 ModelViewMatrix;
uniform mat4 CameraViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;

void main() {
	vec3 tnorm = normalize(NormalMatrix * VertexPosition);
	vec4 eyeCoords = ModelViewMatrix * vec4(VertexPosition, 1.0f);
	vec3 s = normalize(vec3(LigthSpot - eyeCoords));

	LightItensity = Ld * Kd * max(dot(s, tnorm), 0.0f);
	gl_Position = ProjectionMatrix * CameraViewMatrix * ModelViewMatrix * vec4(VertexPosition, 1.0f);
}