#version 450

layout (location = 0) in vec2 aPos;

uniform vec4 uRectangle;
uniform mat4 uOrthographicMatrix;
out vec4 vRect;

void main() {
    gl_Position = uOrthographicMatrix * vec4((uRectangle.zw * aPos) + uRectangle.xy, 0.0f, 1.0f);
    vRect = uRectangle;
}