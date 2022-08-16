#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 varying_geometry_normal;
in vec3 varying_attrib_pos[];

uniform mat4 u_mat_normal;

void main() {
    vec3 side2 = varying_attrib_pos[2] - varying_attrib_pos[0];
    vec3 side0 = varying_attrib_pos[1] - varying_attrib_pos[0];
    vec3 f_norm = normalize(u_mat_normal * cross(side0, side2));

    varying_geometry_normal = f_norm;
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    varying_geometry_normal = f_norm;
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    varying_geometry_normal = f_norm;
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}