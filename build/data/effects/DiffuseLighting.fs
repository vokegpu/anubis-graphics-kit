#version 330 core

layout (location = 0) out vec4 FragColor;

uniform vec4 LigthSpot;
uniform vec3 Kd;
uniform vec3 Ld;
uniform vec3 Camera;

in vec3 Normal;
in vec4 Pos;

vec3 LightItensity;

void main() {
	vec3 ambient = 0.1 * Kd;

	vec3 tnorm = Normal;
	vec3 s = normalize(vec3(LigthSpot - Pos));
	vec3 diffuse = max(dot(s, tnorm), 0.0f) * Kd;

	vec3 viewDir = normalize(Camera - Pos.xyz);
	vec3 reflectDir = reflect(-s, tnorm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	vec3 specular = 0.5f * spec * Kd;

	LightItensity = (ambient + diffuse + specular) * Ld;
	FragColor = vec4(LightItensity, 1.0f);
}