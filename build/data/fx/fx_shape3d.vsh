#version 330 core

layout (location = 0) in vec3 attrib_pos;
layout (location = 1) in vec2 attrib_tex_coords;

out vec2 varying_attrib_tex_coords;
out vec3 varying_attrib_pos[3];

uniform mat4 u_mat_model;
uniform mat4 u_mat_perspective;
uniform mat4 u_mat_camera_view;

int sizeof_varying_attrib_pos = 0;

void main() {
    gl_Position = u_mat_perspective * u_mat_camera_view * u_mat_model * vec4(attrib_pos, 1.0);

    varying_attrib_tex_coords = attrib_tex_coords;
    varying_attrib_pos = gl_TessCoord;
}