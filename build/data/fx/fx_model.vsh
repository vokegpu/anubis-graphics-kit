#version 330 core

layout (location = 0) in vec3 attrib_pos;
layout (location = 1) in vec2 attrib_tex_coords;

out vec2 varying_attrib_tex_coords;
out vec4 varying_attrib_light_pos;

uniform mat4 u_mat4_model;
uniform mat4 u_mat4_perspective;
uniform mat4 u_mat4_view;
uniform vec4 u_vec4_ambient_light;

void main() {
    gl_Position = u_mat4_perspective * u_mat4_view * u_mat4_model * vec4(attrib_pos, 1.0);

    varying_attrib_tex_coords = attrib_tex_coords;
    varying_attrib_light_pos = u_vec4_ambient_light;
}