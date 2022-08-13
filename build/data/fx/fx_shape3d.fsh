#version 330 core

out vec4 final_frag_color;
in vec2 varying_attrib_tex_coords;
in vec3 varying_geometry_normal;

uniform vec4 u_vec_color;
uniform sampler2D u_sampler_texture_active;
uniform bool u_bool_texture_active;
uniform vec4 u_vec_light_pos;

float clamp(float a, float b, float c) {
    return a < b ? b : (a > c ? c : 0);
}

void main() {
    vec4 fragcolor = u_vec_color;

    if (u_bool_texture_active) {
        fragcolor = texture2D(u_sampler_texture_active, varying_attrib_tex_coords) * fragcolor;
    }

    final_frag_color = fragcolor;
}