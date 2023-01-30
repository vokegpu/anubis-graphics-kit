#version 450 core

layout (location = 0) in vec2 VertexPosition;
layout (location = 1) in vec2 Texcoordinate;

uniform vec4 Rectangle;
uniform mat4 MatrixPerspective;

out vec2 Texcoord;
out vec4 Rect;

void main() {
    gl_Position = MatrixPerspective * vec4((Rectangle.zw * VertexPosition) + Rectangle.xy, 0.0f, 1.0f);
    Texcoord = Texcoordinate;
    Rect = Rectangle;
}